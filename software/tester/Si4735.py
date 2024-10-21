import digitalio
import board
import busio
import time

SET_PROPERTY = 0x12
GET_PROPERTY = 0x13
SI473X_ADDR = 0x63
POWER_UP = 0x01
RX_VOLUME = 0x4000
RX_HARD_MUTE = 0x4001
GET_REV = 0x10
FM_TUNE_FREQ = 0x20

high_byte = lambda x: (x >> 8) & 0xff
low_byte = lambda x: x & 0xff

MIN_DELAY_WAIT_SEND_LOOP = 300

class Si4735:
	volume = 40
	freq = 9010
	freq_inc = 100

	def __init__(self, reset_pin, CTSIEN, GPO2OEN, XOSCEN, FUNC):
		self.reset_pin = digitalio.DigitalInOut(reset_pin)
		self.reset_pin.direction = digitalio.Direction.OUTPUT
		
		self.i2c = busio.I2C(board.SCL, board.SDA)
		self.reset()
		time.sleep(0.5)

		while not self.i2c.try_lock():
			pass

		self.addr = self.i2c.scan()
		if not len(self.addr):
			raise RuntimeError("Si4735 I2C address not found.")
		else:
			print(f"Si4735 I2C address found at 0x{self.addr[0]:02X}.")
			self.addr = self.addr[0]

		PATCH = 0 # boot normally
		self.radio_power_up_response = self.radio_power_up(CTSIEN, GPO2OEN, PATCH, XOSCEN, FUNC)
		if self.radio_power_up_response == bytearray(b'\xc0'):
			raise RuntimeError("Si4735 radio power up failed.")
		# self.print_bytearray(self.radio_power_up_response, "Radio power up response")
		
		self.firmware = self.get_firmware()
		# self.print_bytearray(self.firmware, "Firmware")

		self.set_volume(self.volume)
		# self.print_bytearray(self.get_volume_from_ic(), "Volume")

		self.freq_response = self.set_frequency(self.freq)
		# self.print_bytearray(self.freq_response, "Frequency response")
		# self.print_bytearray(self.get_frequency(), "Frequency")

	def reset(self):
		time.sleep(0.01)
		self.reset_pin.value = 0
		time.sleep(0.01)
		self.reset_pin.value = 1
		time.sleep(0.01)

	def send_property(self, property_number, parameter):
		arg2 = high_byte(property_number)
		arg3 = low_byte(property_number)
		arg4 = high_byte(parameter)
		arg5 = low_byte(parameter)
		data = bytes([SET_PROPERTY, 0x00, arg2,	arg3, arg4, arg5])
		self.i2c.writeto(self.addr, data)

	def get_property(self, property_number):
		arg2 = high_byte(property_number)
		arg3 = low_byte(property_number)
		data = bytes([GET_PROPERTY, 0x00, arg2, arg3])
		response = bytearray(4)
		self.i2c.writeto_then_readfrom(self.addr, data, response)
		return response

	def radio_power_up(self, CTSIEN, GPO2OEN, PATCH, XOSCEN, FUNC):
		arg1 = (CTSIEN << 7) | (GPO2OEN << 6) | (PATCH << 5) | (XOSCEN << 4) | (FUNC)
		arg2 = 5 # hard coding FM
		data = bytes([POWER_UP, arg1, arg2])
		response = bytearray(1)
		self.i2c.writeto(self.addr, data)
		time.sleep(0.5)
		self.i2c.readfrom_into(self.addr, response)
		return response

	def set_volume(self, volume):
		self.send_property(RX_VOLUME, volume)
		self.volume = volume

	def get_volume_from_ic(self):
		data = self.get_property(RX_VOLUME)
		last_two_bytes = data[-2:]
		int_value = (last_two_bytes[0] << 8) | last_two_bytes[1]
		return int_value

	def get_volume(self):
		return self.volume

	def volume_up(self):
		self.volume += 1
		self.set_volume(self.volume)

	def volume_down(self):
		self.volume -= 1
		self.set_volume(self.volume)

	def set_mute(self, off):
		self.send_property(RX_HARD_MUTE, 3 if off else 0)

	def get_mute_status(self):
		return self.get_property(RX_HARD_MUTE)

	def get_firmware(self):
		data = bytes([GET_REV])
		self.i2c.writeto(self.addr, data)
		response = bytearray(9)
		self.i2c.readfrom_into(self.addr, response)
		return response

	def print_bytearray(self, barr, desc):
		if isinstance(barr, int):
			print(desc, barr)
			return
		elif not isinstance(barr, bytearray):
			raise TypeError("Input must be a bytearray")
		formatted_strings = []
		for byte in barr:
			binary_str = f'{byte:08b}'
			labels = ' '.join(f'D{7 - i}' for i in range(8))
			bits = '  '.join(bit for bit in binary_str)
			labeled_str = f'{labels}\n{bits}'
			formatted_strings.append(labeled_str)
		print(desc + "\n" + '\n\n'.join(formatted_strings) + "\n")

	def set_frequency(self, freq):
		arg2 = high_byte(freq)
		arg3 = low_byte(freq)
		data = bytes([FM_TUNE_FREQ, 0x03, arg2, arg3, 0x00])
		self.i2c.writeto(self.addr, data)
		time.sleep(0.01)
		response = bytearray(1)
		self.i2c.readfrom_into(self.addr, response)
		return response

	def get_frequency(self):
		return self.freq

	def frequency_up(self):
		set_frequency(self.freq + self.freq_inc)

	def frequency_down(self):
		set_frequency(self.freq - self.freq_inc)
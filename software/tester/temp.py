import time

high_byte = lambda x: (x >> 8) & 0xff
low_byte = lambda x: x & 0xff

volume_upper_limit = 63
volume_lower_limit = 0

def set_fm_freq(i2c, addr, freq):
	arg2 = high_byte(freq)
	arg3 = low_byte(freq)
	result = bytearray(1)
	i2c.writeto(addr, bytes([0x20, 0x00, arg2, arg3, 0x00]))
	time.sleep(0.2)
	i2c.readfrom_into(addr, result)
	print(f"Result from FM tune is {result}.")

def set_am_freq(i2c, addr, freq):
	arg2 = high_byte(freq)
	arg3 = low_byte(freq)
	result = bytearray(1)
	i2c.writeto(addr, bytes([0x40, 0x00, arg2, arg3, 0x00, 0x00]))
	time.sleep(0.2)
	i2c.readfrom_into(addr, result)
	print(f"Result from AM tune is {result}.")

def inc_freq(i2c, addr, freq):
	set_fm_freq(i2c, addr, freq + 1)
	return freq + 1

def dec_freq(i2c, addr, freq):
	set_fm_freq(i2c, addr, freq - 1)
	return freq - 1

def power_on(i2c, addr):
	result = bytearray(1)
	i2c.writeto(addr, bytes([0x01, 0x10, 0x05]))
	time.sleep(0.5)
	i2c.readfrom_into(addr, result)
	print(f"Result from power up is {result}")

def reset(i2c, reset_pin):
	reset_pin.value = True
	print("Released radio's reset")
	time.sleep(.1)
	
	addr = i2c.scan()
	if not len(addr):
		raise RuntimeError("Device address not found.")
	else:
		print(f"I2C device found at 0x{addr[0]:02X}.")
		addr = addr[0]
	return addr

def set_volume(i2c, addr, volume):
	arg4 = high_byte(volume)
	arg5 = low_byte(volume)
	i2c.writeto(addr, bytes([0x12, 0x00, 0x40, 0x00, arg4, arg5]))
	time.sleep(0.2)

def inc_volume(i2c, addr, volume):
	if volume == volume_upper_limit:
		time.sleep(0.2)
		return volume
	set_volume(i2c, addr, volume + 1)
	return volume + 1

def dec_volume(i2c, addr, volume):
	if volume == volume_lower_limit:
		time.sleep(0.2)
		return volume
	set_volume(i2c, addr, volume - 1)
	return volume - 1
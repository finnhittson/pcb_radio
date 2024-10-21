import time
import board
import busio
import digitalio
import temp

bounce_time = 0.2
freq = 9010
volume = 30

reset_pin = digitalio.DigitalInOut(board.D27)
reset_pin.direction = digitalio.Direction.OUTPUT
i2c = busio.I2C(board.SCL, board.SDA)

freq_up_pin = digitalio.DigitalInOut(board.D43)
freq_up_pin.direction = digitalio.Direction.INPUT
freq_up_pin.pull = digitalio.Pull.UP

freq_down_pin = digitalio.DigitalInOut(board.D44)
freq_down_pin.direction = digitalio.Direction.INPUT
freq_down_pin.pull = digitalio.Pull.UP

volume_up_pin = digitalio.DigitalInOut(board.D41)
volume_up_pin.direction = digitalio.Direction.INPUT
volume_up_pin.pull = digitalio.Pull.UP

volume_down_pin = digitalio.DigitalInOut(board.D42)
volume_down_pin.direction = digitalio.Direction.INPUT
volume_down_pin.pull = digitalio.Pull.UP

while 1:
	while not i2c.try_lock():
		pass
	addr = temp.reset(i2c, reset_pin)
	temp.power_on(i2c, addr)
	temp.set_fm_freq(i2c, addr, freq)
	print(f"Freq: {freq}")
	temp.set_volume(i2c, addr, volume)

	while 1:
		if not freq_up_pin.value:
			freq = temp.inc_freq(i2c, addr, freq)
			print(f"Freq: {freq}")
		elif not freq_down_pin.value:
			freq = temp.dec_freq(i2c, addr, freq)
			print(f"Freq: {freq}")
		elif not volume_up_pin.value:
			volume = temp.inc_volume(i2c, addr, volume)
			print(f"Vol: {volume}")
		elif not volume_down_pin.value:
			volume = temp.dec_volume(i2c, addr, volume)
			print(f"Vol: {volume}")

import time
import board
import busio
import digitalio
import temp
import lcd_display

bounce_time = 0.2
prev_freq = 9010
prev_vol = 30
current_freq = None
current_vol = None

lcd = lcd_display.LCDDisplay()
lcd.update_display("Welcome to my\nradio Steve!")
time.sleep(3)
lcd.clear()
message = lambda freq, vol : f"Freq: {freq}\nVol: {vol}"
lcd.update_display(message(round(prev_freq / 100, 2), prev_vol))

i2c, reset_pin, freq_up_pin, freq_down_pin, vol_up_pin, vol_down_pin = temp.init()

while 1:
	while not i2c.try_lock():
		pass
	addr = temp.reset(i2c, reset_pin)
	temp.power_on(i2c, addr)
	temp.set_fm_freq(i2c, addr, prev_freq)
	print(f"Freq: {prev_freq}")
	temp.set_volume(i2c, addr, prev_vol)
	print(f"Vol: {prev_vol}")

	while 1:
		if not freq_up_pin.value:
			current_freq = temp.inc_freq(i2c, addr, prev_freq)
			print(f"Freq: {current_freq}")
		elif not freq_down_pin.value:
			current_freq = temp.dec_freq(i2c, addr, prev_freq)
			print(f"Freq: {current_freq}")

		elif not vol_up_pin.value:
			current_vol = temp.inc_volume(i2c, addr, prev_vol)
			print(f"Vol: {current_vol}")
		elif not vol_down_pin.value:
			current_vol = temp.dec_volume(i2c, addr, prev_vol)
			print(f"Vol: {current_vol}")

		if current_vol is not None and current_vol != prev_vol:
			prev_vol = current_vol
			lcd.update_display(message(round(prev_freq / 100, 2), prev_vol))
		if current_freq is not None and current_freq != prev_freq:
			prev_freq = current_freq
			lcd.update_display(message(round(prev_freq / 100, 2), prev_vol))

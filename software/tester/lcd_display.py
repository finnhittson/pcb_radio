import board
import character_lcd
import digitalio

class LCDDisplay:
	def __init__(self):
		self.lcd_rs = digitalio.DigitalInOut(board.D59)
		self.lcd_en = digitalio.DigitalInOut(board.D60)
		self.lcd_d7 = digitalio.DigitalInOut(board.D64)
		self.lcd_d6 = digitalio.DigitalInOut(board.D63)
		self.lcd_d5 = digitalio.DigitalInOut(board.D62)
		self.lcd_d4 = digitalio.DigitalInOut(board.D61)
		self.lcd_columns = 16
		self.lcd_rows = 2

		self.lcd = character_lcd.Character_LCD_Mono(
			self.lcd_rs, 
			self.lcd_en, 
			self.lcd_d4, 
			self.lcd_d5, 
			self.lcd_d6, 
			self.lcd_d7, 
			self.lcd_columns, 
			self.lcd_rows
		)

	def update_message(self, message):
		self.lcd.message = message

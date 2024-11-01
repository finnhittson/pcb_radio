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

		self.display = [[' ' for _ in range(16)] for _ in range(2)]

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

	def update_character(self, row, col, char):
		if 0 <= row < 2 and 0 <= col < 16:
			self.display[row][col] = char
		else:
			print("Error: Row or column out of bounds.")

	def clear(self):
		self.display = [[' ' for _ in range(16)] for _ in range(2)]
		message_to_display = self.__str__()
		self.lcd.message = message_to_display

	def write_string(self, string):
		self.clear()
		lines = string.split('\n')
		if (len(lines) > 2):
			raise SystemExit("Too many new line characters.")
		for row, line in enumerate(lines):
			for col, char in enumerate(line):
				if col < 16:
					self.update_character(row, col, char)
				else:
					raise SystemExit("Too many characters before new line.")

	def __str__(self):
		return '\n'.join(''.join(row) for row in self.display)

	def update_display(self, message):
		self.write_string(message)
		message_to_display = self.__str__()
		self.lcd.message = message_to_display
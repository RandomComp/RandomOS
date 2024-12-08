from PIL import Image, ImageDraw

from pyperclip import copy

img = Image.open("rus.gif").convert("RGB")

width, height = img.size

pixs = list(img.getdata())

draw = ImageDraw.Draw(img)

result = ""

num = 0

for sx in range(0, width, 9):
	for y in range(16):
		num2 = 0

		for x in range(8):
			if (pixs[sx + x + (y * width)] != (0, 0, 0)):
				num2 += 1

		if (num2 == 8): continue

		result += f"\tRUS[{num}][{y}] = 0b"

		for x in range(8):
			if (pixs[sx + x + (y * width)] == (0, 0, 0)):
				result += "1"

			else: result += "0"

		result += ";\n\n"

	if (num >= 33):
		break

	num += 1

	draw.point((sx, 0), (255, 0, 0))

	draw.point((sx + 8, 15), (0, 255, 0))

copy(result)

img.show()

img.save("rus.gif")
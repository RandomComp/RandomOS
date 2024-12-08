from PIL import Image, ImageDraw

from pyperclip import copy

img = Image.open("ascii.gif").convert("RGB")

width, height = img.size

pixs = list(img.getdata())

draw = ImageDraw.Draw(img)

result = ""

num = -12

for sx in range(88, width, 48):
	for sy in range(25, height - 12, 12):
		if (num >= 0):
			for y in range(12):
				num2 = 0

				for x in range(8):
					num2 += pixs[sx + x + ((sy + y) * width)][0] / 255

				if (num2 == 0): continue

				result += f"\tASCII[{num + 4}][{y + 4}] = 0b"

				for x in range(8):
					if (pixs[sx + x + ((sy + y) * width)] == (255, 255, 255)):
						result += "1"

					else: result += "0"

				result += ";\n\n"

		num += 1

		if (num > 94):
			copy(result)

			break

		draw.point((sx, sy), (255, 0, 0))

		draw.point((sx + 8, sy + 12), (0, 255, 0))

	else: continue

	break

img.show()
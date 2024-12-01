from PIL import Image, ImageDraw

img = Image.open("Random rcyt.png").convert("RGBA")

width, height, = img.size

pixs = list(img.getdata())

draw = ImageDraw.Draw(img)

for x in range(width):
	for y in range(height):
		pos = x + (y * width)

		color = list(pixs[pos])

		colorwb = int(sum(color) / len(color))

		if colorwb > 230:
			color[0] = 0

			color[1] = 0

			color[2] = 0

		draw.point((x, y), tuple(color))
img.save("bg.png")

img.show()
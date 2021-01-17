from PIL import Image
from IPython.display import display


def main():
    img = Image.open("splatmap.png")
    pixels = img.load()

    for i in range(img.size[0]):
        for j in range(img.size[1]):
            (r, g, b, a) = pixels[i, j]
            alphaFloat = a / 255.0
            if alphaFloat < 0.01:
                r = int(r * alphaFloat)
                g = int(g * alphaFloat)
                b = int(b * alphaFloat)
            pixels[i, j] = (r, g, b, a)
    # img.show()
    img.save('test-splatmap3.png')


if __name__ == '__main__':
    main()

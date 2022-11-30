import numpy as np
from PIL import Image
import io

with open("test.txt", "r") as image_file:

    data = []

    original_path = image_file.readline().replace("\n", "")

    width, height = image_file.readline().split()

    width = int(width)
    height = int(height)

    for _ in range(height):
        line = []
        for _ in range(width):
            R, G, B = image_file.readline().split()
            R = int(R)
            G = int(G)
            B = int(B)
            line.append((R, G, B))
        data.append(line)

    data = np.array(data, dtype=np.uint8)

    image = Image.fromarray(data)
    original = Image.open(original_path)

    image.show()
    original.show()
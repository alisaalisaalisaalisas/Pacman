import struct

w, h = 64, 8
bg = (0, 0, 0)
px = [[bg for _ in range(w)] for __ in range(h)]


def put_tile(tx, color, kind):
    ox = tx * 8
    for y in range(8):
        for x in range(8):
            gx = ox + x
            r, g, b = bg
            dx = x - 3.5
            dy = y - 3.5

            if kind == "pac_closed":
                if dx * dx + dy * dy <= 3.3 * 3.3:
                    r, g, b = color
            elif kind == "pac_open":
                if dx * dx + dy * dy <= 3.3 * 3.3:
                    if dx > 0 and abs(dy) < dx * 0.9:
                        r, g, b = bg
                    else:
                        r, g, b = color
            else:
                if y >= 3:
                    r, g, b = color
                else:
                    if dx * dx + dy * dy <= 3.3 * 3.3:
                        r, g, b = color

                # simple eyes
                if (x == 2 and y == 3) or (x == 5 and y == 3):
                    r, g, b = (255, 255, 255)

            px[y][gx] = (r, g, b)


put_tile(0, (255, 230, 40), "pac_open")
put_tile(1, (255, 230, 40), "pac_closed")
put_tile(2, (255, 60, 60), "ghost")
put_tile(3, (255, 140, 200), "ghost")
put_tile(4, (60, 230, 230), "ghost")
put_tile(5, (255, 170, 40), "ghost")
put_tile(6, (60, 60, 255), "ghost")
put_tile(7, (220, 220, 255), "ghost")

row_bytes = w * 3
img_size = row_bytes * h
file_size = 14 + 40 + img_size

bf = struct.pack("<2sIHHI", b"BM", file_size, 0, 0, 54)
bi = struct.pack("<IIIHHIIIIII", 40, w, h, 1, 24, 0, img_size, 2835, 2835, 0, 0)

out_path = "pacman_sfml/assets/sprites/atlas.bmp"
with open(out_path, "wb") as f:
    f.write(bf)
    f.write(bi)
    for y in range(h - 1, -1, -1):
        for x in range(w):
            r, g, b = px[y][x]
            f.write(struct.pack("BBB", b, g, r))

print("wrote", out_path)

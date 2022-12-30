#!/usr/bin/python3

import os
import sys

database = sys.argv[1]
database_output = database + "_output"

os.system(f"find {database} -type f -name '*.jpg' > {database}.txt")

os.mkdir(f"{database_output}")
with open(f"{database}.txt", "r") as f:
    data = f.readlines()
    l = len(data)
    result = []
    i = 1
    for line in data:

    #     print(f'Processing {line}')
        input_fp = line.replace("\n", "")
        progress = (i/l)*100
        
        input_fn = f"{os.path.basename(input_fp)}"

        # Input -> Txt
        output_fn = input_fn.replace(".jpg", ".txt")
        output_fp = f"{database_output}/{output_fn}"
        # print(f"./jpeg {input_fp} {output_fp}")
        os.system(f"./src/output/jpeg {input_fp} {output_fp}")
        
        # Input -> WebP
        encoded_fp = output_fp.replace(".txt", ".webp")
        cmd = f"ffmpeg -i {input_fp} -hide_banner -loglevel error -y -c:v libwebp -lossless 0 -compression_level 0 {encoded_fp}"
        # print(cmd)
        os.system(cmd)

        # WebP -> JPG
        decoded_fp = output_fp.replace(".txt", ".jpg")
        decode_cmd = f"convert {encoded_fp} {decoded_fp}"
        # print(decode_cmd)
        os.system(decode_cmd)

        # JPG -> Txt
        d_fp = output_fp.replace(".txt", "_decoded.txt")
        d_cmd = f"./src/output/jpeg {decoded_fp} {d_fp}"
        # print(d_cmd)
        os.system(d_cmd)

        result.append((d_fp, output_fp))

        print(f"Processing {progress}%", end="\r")
        i += 1

with open(f"{database}.txt", "w+") as f:
    for p1, p2 in result:
        f.write(f"{p1} {p2}\n")

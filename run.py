import os

os.system("find CID2013 -type f -name '*.jpg' > database.txt")

with open("database.txt", "r") as f:
    data = f.readlines()
    l = len(data)
    result = []
    i = 1
    for line in data:

    #     print(f'Processing {line}')
        input_fp = line.replace("\n", "")
        progress = (i/l)*100
        
        input_fn = os.path.basename(input_fp)

        # Input -> Txt
        output_fn = input_fn.replace(".jpg", ".txt")
        output_fp = f"output/{output_fn}"
        # print(f"./jpeg {input_fp} {output_fp}")
        os.system(f"./jpeg {input_fp} {output_fp}")
        
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
        d_cmd = f"./jpeg {decoded_fp} {d_fp}"
        # print(d_cmd)
        os.system(d_cmd)

        result.append((d_fp, output_fp))

        print(f"Processing {progress}%", end="\r")
        i += 1

with open("database.txt", "w+") as f:
    for p1, p2 in result:
        f.write(f"{p1} {p2}\n")

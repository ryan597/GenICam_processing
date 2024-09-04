import cv2
import os
import glob
import argparse
import typing

def convert(files: list[str]) -> None:
    count = 0
    for i in files:
        try:
            src = cv2.imread(i, -1)
            rgb = cv2.cvtColor(src, cv2.COLOR_BayerGR2RGB)
            gray = cv2.cvtColor(rgb, cv2.COLOR_RGB2GRAY)
            cv2.imwrite(f"processed/{i}", gray, params=(cv2.IMWRITE_TIFF_COMPRESSION, 1))
            count += 1
            if count % 100 == 0:
                print(f"Processed: {count}\t {i}")
        except:
            print(f"Failed on {i}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(prog="Convert",
                                     description="Convert all tiff images")

    parser.add_argument("--folder")
    args = parser.parse_args()
    files = glob.glob(args.folder + "/*/*/*.tiff")
    folders = glob.glob(args.folder + "/*/*")
    for i in folders:
        os.makedirs("processed/" + i, exist_ok=True)

    convert(files)




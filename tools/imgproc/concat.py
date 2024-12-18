import argparse
import cv2
import numpy as np
import os


DIRECTIONS = ["horizontal", "vertical"]


def concatenate_images(images, direction="horizontal"):
    if not images:
        raise ValueError("The images list is empty.")
    
    # Read all images
    loaded_images = [cv2.imread(image_path) for image_path in images]
    
    # Check if all images were loaded successfully
    if any(img is None for img in loaded_images):
        print("One or more images could not be loaded. \n")
        sys.exit(0)
    
    # Determine concatenation direction
    if direction == 'horizontal':
        concatenated_image = np.hstack(loaded_images)
    elif direction == 'vertical':
        concatenated_image = np.vstack(loaded_images)
    else:
        print("[ERROR] Invalid direction. Use 'horizontal' or 'vertical'. \n")
        sys.exit(0)
    
    return concatenated_image


def _collect_files(directory, extensions):
    all_files = []

    for root, _, files in os.walk(directory):
        for file in files:
            if any(file.lower().endswith(ext) for ext in extensions):
                file_path = os.path.join(root, file)
                all_files.append(file_path)

    return all_files


def _check_args(args):
    if not (args.direction in DIRECTIONS):
        print("[ERROR] Valid direction option. Supported: %s" % ", ".join(DIRECTIONS))
        sys.exit(0)
    if (args.width <= 0) or (args.height <= 0):
        print("[ERROR] Input width and height must > 0.")
        sys.exit(0)


def _create_parser():
    parser = argparse.ArgumentParser(formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument("-i", "--input-dir", help="Dir for input images. \n", required=True)
    parser.add_argument("-o", "--output", default="./output.png", help="Path to save image. \n")
    parser.add_argument("-d", "--direction", help="Direction to concatenate. \n" + 
        "Suport directions: %s" % ", ".join(DIRECTIONS), required=True)
    parser.add_argument("-w", "--width", default=512, help="Width value to resize to. \n", type=int)
    # `-h` option conflig `--help`
    parser.add_argument("--height", default=512, help="Height value to resize to. \n", type=int)

    return parser


def _main():
    parser = _create_parser()
    args = parser.parse_args()
    _check_args(args)

    extensions = [".png"]

    all_images = _collect_files(args.input_dir, extensions)

    concatenated_image = concatenate_images(all_images, args.direction)

    cv2.imwrite(args.output, concatenated_image)
    print(f"Concatenated image saved as {args.output}")


if __name__ == "__main__":
    # _main()

    aaa = ["/Users/alex/minicraft/assets/tex/transparent_image.png", "/Users/alex/minicraft/assets/tex/texture.png"]
    concatenated_image = concatenate_images(aaa, "vertical")

    cv2.imwrite("tex.png", concatenated_image)

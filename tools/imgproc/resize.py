import argparse
import cv2

def resize_image(args):
    image = cv2.imread(args.input)

    if image is None:
        raise FileNotFoundError(f"The image at path {args.input} could not be found.")

    # Resize the image to the specified width and height
    resized_image = cv2.resize(image, (args.width, args.height))

    return resized_image


def _check_args(args):
    if args.width <= 0 or args.height <= 0:
        print("[ERROR] Input width and height must > 0.")
        sys.exit(0)


def _create_parser():
    parser = argparse.ArgumentParser(formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument("-i", "--input", help="Path to input image. \n", required=True)
    parser.add_argument("-o", "--output", default="./output.png", help="Path to save resize image. \n")
    parser.add_argument("-w", "--width", default=512, help="Width value to resize to. \n", type=int)
    # `-h` option conflig `--help`
    parser.add_argument("--height", default=512, help="Height value to resize to. \n", type=int)

    return parser


def _main():
    parser = _create_parser()
    args = parser.parse_args()
    _check_args(args)
    resized_image = resize_image(args)

    cv2.imwrite(args.output, resized_image)
    print(f"Resized image saved as {args.output}")


if __name__ == "__main__":
    _main()

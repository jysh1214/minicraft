import argparse
import cv2


def create_parser():
    parser = argparse.ArgumentParser()
    parser.add_argument("-p", "--path", default="./resources", help="")


def main():
    parser = create_parser()

if __name__ == "__main__":
    main()

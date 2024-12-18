import cv2
import numpy as np


def create_transparent_image(width, height, color=(0, 0, 0, 0)):
    """
    Create a transparent image.

    :param width: Width of the image
    :param height: Height of the image
    :param color: Color of the image (B, G, R, A)
    :return: Transparent image
    """
    # Create an empty image with 4 channels (RGBA)
    image = np.zeros((height, width, 4), dtype=np.uint8)
    
    # Fill the image with the specified color
    image[:] = color
    
    return image

# Example usage
color = (0, 0, 0, 0)  # Semi-transparent red

transparent_image = create_transparent_image(512 * 3, 512, color)

# Save the image
output_path = 'transparent_image.png'
cv2.imwrite(output_path, transparent_image)

print(f"Transparent image saved as {output_path}")

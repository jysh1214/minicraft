#include <span>

#include <GL/glew.h>
#include <opencv2/opencv.hpp>

#include "noise.h"

void normalize(std::span<GLchar> out, std::span<GLfloat> in) {
  for (GLuint i = 0; i < in.size(); ++i) {
    out[i] = static_cast<GLchar>((in[i] + 1.0f) * 0.5 * 255.0f);
  }
}

int main() {
  const GLuint h = 8192;
  const GLuint w = 8192;
  std::vector<GLfloat> noise(h * w);
  NoiseGenerator<NoiseType::OpenSimplex2>::noise2D(noise, h, w, 76531);
  std::vector<GLchar> noisePixel(h * w, 0);
  normalize(noisePixel, noise);
  cv::Mat noiseImg(h, w, CV_8UC1, noisePixel.data());
  cv::imwrite("output.png", noiseImg);

  return 0;
}

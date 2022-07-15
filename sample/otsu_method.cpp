#include <math.h>
#include <simpleBMP.h>
/**
 * Otsu's method is an algorithm for gray-scale image thresholding
 * See https://en.wikipedia.org/wiki/Otsu%27s_method
 * @param p_img[in,out] the image to be thresholded, it will be updated in place
 */
void Otsu(simpleBMP::BMPImg *p_img) {
  auto &img = *p_img;
  int numofpixal =
      img.info_header.biWidth * img.info_header.biHeight * img.Channel();
  double pi[256] = {0};
  double px = 1.0 / numofpixal;
  for (int row = 0; row < img.info_header.biHeight; ++row) {
    auto line = img.AtLine(row);
    for (int col = 0; col < img.info_header.biWidth; ++col) {
      int value = line[col];
      pi[value] += px;
    }
  }

  double p1k[256] = {0};
  double mk[256] = {0};
  double cumsumpi = 0;
  double mg = 0;
  for (int i = 0; i < 255; i++) {
    cumsumpi += pi[i];
    p1k[i] = cumsumpi;
    mg += i * pi[i];
    mk[i] = mg;
  }

  int numofbestk = 1;
  int bestk = 0;
  double besttaub = 0;
  double taubk[256] = {0};
  for (int i = 0; i < 255; i++) {
    taubk[i] = pow(mg * p1k[i] - mk[i], 2) / (p1k[i]) / (1 - p1k[i]);
    if (taubk[i] > besttaub) {
      bestk = i;
      besttaub = taubk[i];
      numofbestk = 1;
    } else if (taubk[i] == besttaub) {
      bestk += i;
      numofbestk += 1;
    }
  }
  bestk /= numofbestk;
  for (int row = 0; row < img.info_header.biHeight; ++row) {
    auto line = img.AtLine(row);
    for (int col = 0; col < img.info_header.biWidth; ++col) {
      int value = line[col];
      line[col] = ((uint8_t)(value > bestk)) * 255;
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <path_to_input_image>\n", argv[0]);
    return 1;
  }
  simpleBMP::BMPImg grayscale_img;
  grayscale_img.LoadImage(argv[1]);
  assert(grayscale_img.info_header.biBitCount == 8 &&
         "Only support 8-bit gray-scale image");
  Otsu(&grayscale_img);

  std::string path = argv[1];
  path += ".otsu.bmp";
  grayscale_img.SaveImage(path);
  return 0;
}

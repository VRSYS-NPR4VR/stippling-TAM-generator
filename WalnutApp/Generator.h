#pragma once

#include <vector>
#include <functional>
#include <string>

#include <opencv2/opencv.hpp>

#include "Walnut/Image.h"

class Generator {
public:
  Generator();
  std::vector<std::shared_ptr<Walnut::Image>> generate_TAM(int level, int max_res, std::string path, cv::Mat texture, int stippling_dot_size, std::vector<float> const& tone_values);
};
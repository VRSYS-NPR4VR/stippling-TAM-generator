#pragma once

#include <vector>
#include <functional>
#include <string>

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"

#include "Walnut/Image.h"

class Generator {
public:
  Generator();
  std::vector<std::shared_ptr<Walnut::Image>> generate_TAM(int level, int max_res, std::string path);
  //std::vector <std::shared_ptr<cv::Mat>> const get_tam();
  //int c = 0;
//private:
//  std::vector<int> a;
//  int b[10] = { 0,1,2,3,4,5,6,7,8,9 };
//  std::vector<std::shared_ptr<cv::Mat>> tam;
};
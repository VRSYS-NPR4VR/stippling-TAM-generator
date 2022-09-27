#include "Generator.h"
#include <iostream>
#include "Walnut/Image.h"

using namespace std;
using namespace cv;

Generator::Generator()
{
  /*std::shared_ptr<Walnut::Image> test = std::make_shared<Walnut::Image>("LogoV3.png");
  std::vector <std::shared_ptr<Walnut::Image>> tam{test};*/
}
std::vector<std::shared_ptr<Walnut::Image>> Generator::generate_TAM(int level, int max_res, std::string path)
{
  ////////////////////////////////////
  //cout << "Hello, World!" << endl;
  //cout << "OpenCV version is " << CV_VERSION << endl;
  //cout << "max_res" << max_res << endl;
  //Mat img = imread("stippling_brush_texture.png");
  //namedWindow("image", WINDOW_AUTOSIZE);
  //imshow("image", img);
  //waitKey(30);
  //imwrite(path+"/img.png", img);
  ///////////////////////////////////
  /* Generate white images according to level
  *  
  */
  std::vector<std::shared_ptr<Walnut::Image>> tam;

  //set resolutions
  int x3_res = max_res / 2;
  int x2_res = x3_res / 2;
  int x1_res = x2_res / 2;

  for (int i = 0; i < level; i++) {
    //create images
    Mat x1(50, 50, CV_64F);
    Mat x2(x1_res, x1_res, CV_64F);
    Mat x3(x3_res, x3_res, CV_64F);
    Mat x4(max_res, max_res, CV_64F);

    //set to white
    x1.setTo(Scalar(255, 255, 255));
    x2.setTo(Scalar(255, 255, 255));
    x3.setTo(Scalar(255, 255, 255));
    x4.setTo(Scalar(255, 255, 255));

    //Generate path
    auto x1_path = path + "/" + std::to_string(i) + "_x" + std::to_string(x1_res) + ".png";
    auto x2_path = path + "/" + std::to_string(i) + "_x" + std::to_string(x2_res) + ".png";
    auto x3_path = path + "/" + std::to_string(i) + "_x" + std::to_string(x3_res) + ".png";
    auto x4_path = path + "/" + std::to_string(i) + "_x" + std::to_string(max_res) + ".png";

    //Write to file
    imwrite(x1_path, x1);
    imwrite(x2_path, x2);
    imwrite(x3_path, x3);
    imwrite(x4_path, x4);

    //add to TAM
    tam.push_back(std::make_shared<Walnut::Image>(x1_path));
    tam.push_back(std::make_shared<Walnut::Image>(x2_path));
    tam.push_back(std::make_shared<Walnut::Image>(x3_path));
    tam.push_back(std::make_shared<Walnut::Image>(x4_path));

    std::cout << "generator:" << std::endl;
    std::cout << "tam.size" << tam.size() << std::endl;
    std::cout << "level" << level << std::endl;
  }

  return tam;
}

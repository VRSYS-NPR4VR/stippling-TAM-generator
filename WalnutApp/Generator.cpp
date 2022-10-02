#include "Generator.h"
#include <iostream>
#include "Walnut/Image.h"
#include <random>
#include <chrono>

using namespace std;
using namespace cv;

Generator::Generator(){ }

double generate_mean(Mat input)
{
  //std::cout << "generator:" << std::endl;
  //std::cout << input.size << std::endl;
  double mean = 0.0;
  for (int i = 0; i < input.rows; i++) 
  {
    for (int j = 0; j < input.cols; j++)
    {
      mean += input.at<cv::Vec3d>(i, j)[0] + input.at<cv::Vec3d>(i, j)[1] + input.at<cv::Vec3d>(i, j)[2];
      /*std::cout << "rgb at: " << i << "," << j << ": " << input.at<cv::Vec3d>(i, j)[0] << input.at<cv::Vec3d>(i, j)[1] << input.at<cv::Vec3d>(i, j)[2] << std::endl;*/
    }
  }
  mean = mean / /*input.rows*/(input.rows * input.cols);
  //std::cout << "mean after div:" << std::endl;
  //std::cout << mean << std::endl;
  return mean;
}

void overlay_PNG(Mat& back, Mat const& front, cv::Point2i location)
{
  for (int i = 0; i < front.rows; i++)
  {
    for (int j = 0; j < front.cols; j++)
    {
      /*std::cout << "brga at: " << i << "," << j << ": " << front.at<cv::Vec4d>(i, j)[0] << ": " << front.at<cv::Vec4d>(i, j)[1] << ": " << front.at<cv::Vec4d>(i, j)[2] << ": " << front.at<cv::Vec4d>(i, j)[3] << std::endl;*/
      if (front.at<Vec4d>(i, j)[3] != 0) 
      {
        back.at<Vec3d>(i + location.x, j + location.y)[0] = front.at<Vec4d>(i, j)[0];
        back.at<Vec3d>(i + location.x, j + location.y)[1] = front.at<Vec4d>(i, j)[1];
        back.at<Vec3d>(i + location.x, j + location.y)[2] = front.at<Vec4d>(i, j)[2];
      }
    }
  }
}

std::vector<std::shared_ptr<Walnut::Image>> Generator::generate_TAM(int level, int max_res, std::string path, Mat texture, int stippling_dot_size, std::vector<float> const& tone_values)
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
  std::cout << max_res / 2 / 2 / 2 << std::endl;
  std::cout << max_res / 6 << std::endl;
  int l = max_res;
  for (int i = 0; i < 3; i++) {
    l = l / 2;
  }
  std::cout << max_res / 6 << std::endl;
  std::vector<std::shared_ptr<Walnut::Image>> tam;
  std::vector<std::shared_ptr<Mat>> cv_tam;
  //convert to right format ?implement: error if not quadratic?
  texture.convertTo(texture, CV_64FC4);
  resize(texture, texture, Size(stippling_dot_size, stippling_dot_size), INTER_LINEAR);

  //set resolutions
  int x3_res = max_res / 2;
  int x2_res = x3_res / 2;
  int x1_res = x2_res / 2;

  for (int i = 0; i < level; i++) {

    /*leftmost column*/
    if (tam.size() == 0)
    {
      std::cout << "tam.size" << tam.size() << std::endl;
      std::cout << "cv_tam.size" << cv_tam.size() << std::endl;

      //create images
      Mat x1(x1_res, x1_res, CV_64FC3);
      Mat x2(x2_res, x2_res, CV_64FC3);
      Mat x3(x3_res, x3_res, CV_64FC3);
      Mat x4(max_res, max_res, CV_64FC3);

      std::vector<Mat> mats({ x1,x2,x3,x4 });

      //set to white
      for (auto& element : mats) {
        element.setTo(Scalar(255, 255, 255));
        //for (int i = 0; i < element.rows; i++)
        //{
        //  for (int j = 0; j < element.cols; j++)
        //  {
        //    element.at<cv::Vec4d>(i, j)[0] = 255; 
        //    element.at<cv::Vec4d>(i, j)[1] = 255;
        //    element.at<cv::Vec4d>(i, j)[2] = 255;
        //    element.at<cv::Vec4d>(i, j)[3] = 0;
        //    /*std::cout << "rgb at: " << i << "," << j << ": " << element.at<cv::Vec3d>(i, j)[0] << " " << element.at<cv::Vec3d>(i, j)[1] << " " << element.at<cv::Vec3d>(i, j)[2] << std::endl;*/
        //  }
        //}
      }

      auto copy_tex = texture;
      //auto mean = generate_mean(mats[i]);
      //main generation loop
      for (int j = 0; j < mats.size(); j++) {
        double tone_value = 1000.0;
        //obtain a random number from hardware
        std::random_device rd;
        //seed the generator
        std::mt19937 gen(rd());
        /*Mat t(mats[j].cols, mats[j].rows, CV_64FC4);
        t.setTo(Scalar(255, 255, 255));*/
        while ((int)tone_value > (int)tone_values[0]) {
          //place textures randomly until desired tone value is reached
          std::uniform_int_distribution<> distr(0, mats[j].cols - texture.cols);
          /*copy_tex.copyTo(mats[j](Rect(distr(gen), distr(gen), texture.cols, texture.rows)));*/
          overlay_PNG(mats[j], copy_tex, Point(distr(gen), distr(gen)));
          tone_value = generate_mean(mats[j]);
          //tone_value -= 100;
          /*std::cout << "mean:" << std::endl;
          std::cout << (int)tone_value << std::endl;
          std::cout << ">" << std::endl;
          std::cout << (int)tone_values[0] << std::endl;*/
        }
        //t.copyTo(mats[j]);
        /*std::cout << "------------------------------------------" << std::endl;*/
      }
      /*namedWindow("image", WINDOW_AUTOSIZE);
      imshow("image", x4);
      waitKey(30);*/

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

      cv_tam.push_back(std::make_shared<Mat>(x1));
      cv_tam.push_back(std::make_shared<Mat>(x2));
      cv_tam.push_back(std::make_shared<Mat>(x3));
      cv_tam.push_back(std::make_shared<Mat>(x4));
    }
    /*other columns*/
    else
    {
      std::cout << "tam.size" << tam.size() << std::endl;
      std::cout << "cv_tam.size" << cv_tam.size() << std::endl;
      //copy left column
      Mat x1 = *cv_tam[cv_tam.size()-4];
      Mat x2 = *cv_tam[cv_tam.size() - 3];
      Mat x3 = *cv_tam[cv_tam.size() - 2];
      Mat x4 = *cv_tam[cv_tam.size() - 1];

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

      cv_tam.push_back(std::make_shared<Mat>(x1));
      cv_tam.push_back(std::make_shared<Mat>(x2));
      cv_tam.push_back(std::make_shared<Mat>(x3));
      cv_tam.push_back(std::make_shared<Mat>(x4));
    }

    /*std::cout << "generator:" << std::endl;
    std::cout << "tam.size" << tam.size() << std::endl;
    std::cout << "level" << level << std::endl;*/
  }

  return tam;
}

#include "Generator.h"
#include <iostream>
#include "Walnut/Image.h"
#include <random>
#include <chrono>
#include <omp.h>

using namespace std;
using namespace cv;

Generator::Generator(){ }

void overlay_PNG(Mat& back, Mat const& front, cv::Point2i location)
{
  #pragma omp parallel for
  for (int i = 0; i < front.rows; i++)
  {
    for (int j = 0; j < front.cols; j++)
    {
      if (front.at<Vec4d>(i, j)[3] != 0)
      {
        if (i + location.x < back.rows && j + location.y < back.cols && j + location.y >= 0  && i + location.x >= 0)
        {
          auto front_b = front.at<Vec4d>(i, j)[0];
          auto front_r = front.at<Vec4d>(i, j)[1];
          auto front_g = front.at<Vec4d>(i, j)[2];

          auto back_b = back.at<Vec3d>(i + location.x, j + location.y)[0];
          auto back_r = back.at<Vec3d>(i + location.x, j + location.y)[1];
          auto back_g = back.at<Vec3d>(i + location.x, j + location.y)[2];

          auto alpha = front.at<Vec4d>(i, j)[3] / 255;
          auto oneminusalpha = 1 - alpha;

          back.at<Vec3d>(i + location.x, j + location.y)[0] = ((front_b * alpha) + (oneminusalpha * back_b));
          back.at<Vec3d>(i + location.x, j + location.y)[1] = ((front_r * alpha) + (oneminusalpha * back_r));
          back.at<Vec3d>(i + location.x, j + location.y)[2] = ((front_g * alpha) + (oneminusalpha * back_g));
        }
      }
    }
  }
}

double generate_mean(Mat input)
{

  double mean = 0.0;
  #pragma omp parallel for reduction(+ : mean)
  for (int i = 0; i < input.rows; i++) 
  {
    for (int j = 0; j < input.cols; j++)
    {
      mean += input.at<cv::Vec3d>(i, j)[0] + input.at<cv::Vec3d>(i, j)[1] + input.at<cv::Vec3d>(i, j)[2];
    }
  }
  mean = mean / (input.rows * input.cols);
  return mean;
}

std::vector<std::shared_ptr<Walnut::Image>> Generator::generate_TAM(int level, int max_res, std::string path, Mat texture, int stippling_dot_size, std::vector<float> const& tone_values)
{
  std::vector<std::shared_ptr<Walnut::Image>> tam;
  std::vector<std::shared_ptr<Mat>> cv_tam;

  //convert texture to right format ?implement: error if not quadratic?
  texture.convertTo(texture, CV_64FC3);
  resize(texture, texture, Size(stippling_dot_size, stippling_dot_size), INTER_LINEAR);

  //set resolutions
  int x3_res = max_res / 2;
  int x2_res = x3_res / 2;
  int x1_res = x2_res / 2;
  for (int i = 0; i < level; i++) {
    std::vector<std::vector<Point>> points(4);
    /*leftmost column*/
    if (tam.size() == 0)
    {
      //create images
      Mat x1(x1_res, x1_res, CV_64FC3);
      Mat x2(x2_res, x2_res, CV_64FC3);
      Mat x3(x3_res, x3_res, CV_64FC3);
      Mat x4(max_res, max_res, CV_64FC3);

      std::vector<Mat> mats({ x1,x2,x3,x4 });

      //set to white
      for (auto& element : mats) {
        element.setTo(Scalar(255, 255, 255));
      }

      auto copy_tex = texture;
      for (int j = 0; j < mats.size(); j++) {
        double tone_value = generate_mean(mats[j]);
        while ((int)tone_value > (int)tone_values[i]) {
          //obtain a random number from hardware
          random_device rd;
          //seed the generator
          default_random_engine gen{ rd() };
          //mt19937 gen(rd());
          //place textures randomly until desired tone value is reached
          uniform_int_distribution<> distr(0 - texture.cols/2, mats[j].cols/* - texture.cols*/);
          //place points from coarser level
          if (j != 0)
          {
            for (int i = 0; i < points[j - 1].size(); i++)
            {
              Point new_pos(points[j - 1][i].x * 2, points[j - 1][i].y * 2);
              overlay_PNG(mats[j], copy_tex, new_pos);
              points[j].push_back(new_pos);
            }
          }
          auto pos = Point(distr(gen), distr(gen));
          overlay_PNG(mats[j], copy_tex, pos);
          points[j].push_back(pos);

          tone_value = generate_mean(mats[j]);
        }
      }

      //Generate path
      auto x1_path = path + "/" + std::to_string(i) + "_x" + std::to_string(x1_res) + ".png";
      auto x2_path = path + "/" + std::to_string(i) + "_x" + std::to_string(x2_res) + ".png";
      auto x3_path = path + "/" + std::to_string(i) + "_x" + std::to_string(x3_res) + ".png";
      auto x4_path = path + "/" + std::to_string(i) + "_x" + std::to_string(max_res) + ".png";

      //Write to file
      cv::imwrite(x1_path, x1);
      cv::imwrite(x2_path, x2);
      cv::imwrite(x3_path, x3);
      cv::imwrite(x4_path, x4);

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
      //copy left column
      Mat x1 = *cv_tam[cv_tam.size()-4];
      Mat x2 = *cv_tam[cv_tam.size() - 3];
      Mat x3 = *cv_tam[cv_tam.size() - 2];
      Mat x4 = *cv_tam[cv_tam.size() - 1];

      std::vector<Mat> mats({ x1,x2,x3,x4 });

      auto copy_tex = texture;
      //main generation loop
      for (int j = 0; j < mats.size(); j++) {
        double tone_value = generate_mean(mats[j]);
        while ((int)tone_value > (int)tone_values[i]) {
          //obtain a random number from hardware
          random_device rd;
          //seed the generator
          default_random_engine gen{ rd() };
          //place textures randomly until desired tone value is reached
          uniform_int_distribution<> distr(0 - texture.cols / 2, mats[j].cols/* - texture.cols*/);
          //place points from coarser level
          if (j != 0)
          {
            for (int i = 0; i < points[j - 1].size(); i++)
            {
              Point new_pos(points[j - 1][i].x * 2, points[j - 1][i].y * 2);
              overlay_PNG(mats[j], copy_tex, new_pos);
              points[j].push_back(new_pos);
            }
          }
          auto pos = Point(distr(gen), distr(gen));
          overlay_PNG(mats[j], copy_tex, pos);
          points[j].push_back(pos);

          tone_value = generate_mean(mats[j]);
        }
      }

      //Generate path
      auto x1_path = path + "/" + std::to_string(i) + "_x" + std::to_string(x1_res) + ".png";
      auto x2_path = path + "/" + std::to_string(i) + "_x" + std::to_string(x2_res) + ".png";
      auto x3_path = path + "/" + std::to_string(i) + "_x" + std::to_string(x3_res) + ".png";
      auto x4_path = path + "/" + std::to_string(i) + "_x" + std::to_string(max_res) + ".png";

      //Write to file
      cv::imwrite(x1_path, x1);
      cv::imwrite(x2_path, x2);
      cv::imwrite(x3_path, x3);
      cv::imwrite(x4_path, x4);

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
  }
  return tam;
}

#include "pipeline.hpp"
#include <opencv2/core/mat.hpp>
#include <iostream>

cv::Mat 
Pipeline::run(const cv::Mat &image)
{
  std::cout << __PRETTY_FUNCTION__ << " -> run" << std::endl; 
  cv::Mat result;
  for(const auto &module : mModules)
  {
    result = module->run(image);
  }
  std::cout << __PRETTY_FUNCTION__ << " -> done" << std::endl; 
  return result;
}
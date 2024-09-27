#include "pipeline.hpp"
#include <opencv2/core/mat.hpp>
#include <iostream>

cv::Mat 
Pipeline::run(const cv::Mat &image)
{
  std::cout << __PRETTY_FUNCTION__ << " -> run" << std::endl; 
  cv::Mat result = image;
  for(const auto &module : mModules)
  {    
    result = module->run(image, result);    
  }
  std::cout << __PRETTY_FUNCTION__ << " -> done" << std::endl; 
  return result;
}

void 
Pipeline::reset() noexcept
{
  std::cout << __PRETTY_FUNCTION__ << " -> reset" << std::endl; 
  for(const auto &module : mModules)
  {
    module->reset();
  }
  std::cout << __PRETTY_FUNCTION__ << " -> done" << std::endl;
}
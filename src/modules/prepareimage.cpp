#include "prepareimage.hpp"

#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>

cv::Mat PrepareImage::run(const cv::Mat &baseImage, const cv::Mat &image)
{
  std::cout << __PRETTY_FUNCTION__ << " -> run" << std::endl;
  cv::Mat gray, blurred, edged, result; 
  
  cv::cvtColor(image, gray, mColorConversionCode);  

  if(getDebugMode()) cv::imwrite("./prepare_image_0.jpg", gray);
  std::cout << __PRETTY_FUNCTION__ << " -> cvtColor to conversionCode: " << mColorConversionCode << std::endl;

  cv::GaussianBlur(gray, blurred, mKSize, mSigmaX, mSigmaY);
  if(getDebugMode()) cv::imwrite("./prepare_image_1.jpg", blurred);
  std::cout << __PRETTY_FUNCTION__ << " -> GaussianBlur with ksize: " << mKSize << " sigmaX: " << mSigmaX << " sigmaY: " << mSigmaY << std::endl;

  cv::Canny(blurred, edged, mThreshold1, mThreshold2, mApertureSize);

  if(getDebugMode()) cv::imwrite("./prepare_image_2.jpg", edged);

  cv::adaptiveThreshold(edged, result, 255, cv::ADAPTIVE_THRESH_MEAN_C,
                          cv::THRESH_BINARY_INV, 11, 3);

  std::cout << __PRETTY_FUNCTION__ << " -> done" << std::endl;
  
  return edged;
}
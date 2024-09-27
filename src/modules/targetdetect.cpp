#include "targetdetect.hpp"

#include <algorithm>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <stdexcept>

cv::Mat TargetDetect::run(const cv::Mat &image)
{
  std::cout << __PRETTY_FUNCTION__ << " -> run" << std::endl;
  std::vector<std::vector<cv::Point>> squares;

  cv::Mat gray, blurred, edged; 
  
  cv::cvtColor(image, gray, mColorConversionCode);  

  if(getDebugMode()) cv::imwrite("./target_detect_0.jpg", gray);
  std::cout << __PRETTY_FUNCTION__ << " -> cvtColor to conversionCode: " << mColorConversionCode << std::endl;

  cv::GaussianBlur(gray, blurred, mKSize, mSigmaX, mSigmaY);
  if(getDebugMode()) cv::imwrite("./target_detect_1.jpg", blurred);
  std::cout << __PRETTY_FUNCTION__ << " -> GaussianBlur with ksize: " << mKSize << " sigmaX: " << mSigmaX << " sigmaY: " << mSigmaY << std::endl;

  cv::Canny(blurred, edged, mThreshold1, mThreshold2, mApertureSize);

  if(getDebugMode()) cv::imwrite("./target_detect_2.jpg", edged);
  

  std::vector<std::vector<cv::Point>> contours;
  cv::findContours(edged, contours, mMode, mMethod);

  std::cout << __PRETTY_FUNCTION__ << " -> found contours: " << contours.size() << std::endl;

  std::vector<cv::Point> approx;
    for (size_t i = 0; i < contours.size(); i++) {
        // Aproksymacja konturu do wielokąta
        cv::approxPolyDP(contours[i], approx, cv::arcLength(contours[i], true) * 0.01, true);

        if (approx.size() == mCornersSize && cv::isContourConvex(approx)) {
            double area = cv::contourArea(approx);
            if (area > mMinArea) { // ignorujemy małe obszary
                std::cout << i << " -> " << area << std::endl;
                squares.push_back(approx);
            }
        }
    }

  if(squares.empty()) {
      std::cerr << __PRETTY_FUNCTION__ << " -> no squares found" << std::endl;
      throw std::logic_error("No squares found");
  }

  const auto &squareIt = std::find_if(squares.begin(), squares.end(), [](const auto &square) {
      return !square.empty();
  });

  if(squareIt == squares.end()) {
      std::cerr << __PRETTY_FUNCTION__ << " -> no square found" << std::endl;
      throw std::logic_error("No square found");
  }

  const auto square = *squareIt;

  std::vector<cv::Point2f> src_points;
  for (int i = 0; i < 4; i++) {
      src_points.push_back(cv::Point2f(square[i].x, square[i].y));
  }
      
  std::vector<cv::Point2f> dst_points = {
      cv::Point2f(0, 0),
      cv::Point2f(mResultImageX, 0),
      cv::Point2f(mResultImageX, mResultImageY),
      cv::Point2f(0, mResultImageY)
  };

  cv::Mat transform_matrix = cv::getPerspectiveTransform(src_points, dst_points);

  cv::Mat transformed_image;
  cv::warpPerspective(image, transformed_image, transform_matrix, cv::Size(mResultImageX, mResultImageY));

  return transformed_image;
}
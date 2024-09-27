#ifndef TARGETDETECT_HPP
#define TARGETDETECT_HPP

#include "imodule.hpp"

#include <opencv2/imgproc.hpp>

class TargetDetect final : public IModule
{
  public:
    cv::Mat run(const cv::Mat &image) override;

  private:
    cv::ColorConversionCodes mColorConversionCode = cv::COLOR_BGR2GRAY;
    cv::Size mKSize {5, 5};
    double mSigmaX = 1.5;
    double mSigmaY = 0;
    double mThreshold1 = 50;
    double mThreshold2 = 150;
    int mApertureSize = 3;

  private:
    cv::RetrievalModes mMode = cv::RETR_LIST;
    cv::ContourApproximationModes mMethod = cv::CHAIN_APPROX_SIMPLE;

  private:
    int mCornersSize = 4;
    int mMinArea = 1000;

  private:
    float mResultImageX = 640.0;
    float mResultImageY = 640.0;  
};

#endif
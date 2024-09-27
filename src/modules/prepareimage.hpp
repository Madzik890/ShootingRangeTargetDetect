#ifndef PREPAREIMAGE_HPP
#define PREPAREIMAGE_HPP

#include "imodule.hpp"

#include <opencv2/imgproc.hpp>

class PrepareImage final : public IModule
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
};


#endif
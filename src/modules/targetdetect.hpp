#ifndef TARGETDETECT_HPP
#define TARGETDETECT_HPP

#include "imodule.hpp"

#include <opencv2/imgproc.hpp>

class TargetDetect final : public IModule
{
  public:
    cv::Mat run(const cv::Mat &baseImage, const cv::Mat &image) override;

  public:  
    void setRawMode(const bool rawMode) noexcept { mRawMode = rawMode; }
    void setResultImageX(const float resultImageX) noexcept { mResultImageX = resultImageX; }
    void setResultImageY(const float resultImageY) noexcept { mResultImageY = resultImageY; }

    bool getRawMode() const noexcept { return mRawMode; }    
    float getResultImageX() const noexcept { return mResultImageX; }
    float getResultImageY() const noexcept { return mResultImageY; }

  private:
    bool mRawMode {true};

  private:
    cv::RetrievalModes mMode = cv::RETR_LIST;
    cv::ContourApproximationModes mMethod = cv::CHAIN_APPROX_SIMPLE;

  private:
    int mCornersSize = 4;
    int mMinArea = 1000;

  private:
    float mResultImageX = -1;
    float mResultImageY = -1;  
};

#endif
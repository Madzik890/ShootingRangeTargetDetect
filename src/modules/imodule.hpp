#ifndef IMODULE_HPP
#define IMODULE_HPP

#include <opencv2/core/mat.hpp>

class IModule 
{
  public:
    IModule() = default;
    virtual ~IModule() = default;

  public:
    virtual cv::Mat run(const cv::Mat &image) = 0;

  public:
    void setDebugMode(bool debugMode) noexcept { mDebugMode = debugMode; }
    bool getDebugMode() const noexcept { return mDebugMode; }

  private:
    bool mDebugMode { false };
};

#endif
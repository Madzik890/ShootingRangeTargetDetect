#ifndef IMODULE_HPP
#define IMODULE_HPP

#include <opencv2/core/mat.hpp>
#include <iostream>

class IModule 
{
  public:
    IModule() = default;
    virtual ~IModule() = default;

  public:
    virtual cv::Mat run(const cv::Mat &baseImage, const cv::Mat &image) = 0;
    virtual void reset() noexcept { std::cout << __PRETTY_FUNCTION__ << " -> called in base class" << std::endl;};

  public:
    void setDebugMode(bool debugMode) noexcept { mDebugMode = debugMode; }
    bool getDebugMode() const noexcept { return mDebugMode; }

  private:
    bool mDebugMode { false };
};

#endif
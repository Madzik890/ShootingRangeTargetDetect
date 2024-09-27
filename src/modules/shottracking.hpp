#ifndef SHOTTRACKING_HPP
#define SHOTTRACKING_HPP

#include "imodule.hpp"
#include <boost/optional/optional.hpp>
#include <opencv2/core/mat.hpp>
#include <boost/optional.hpp>

class ShotTracking final : public IModule
{
  public:
    cv::Mat run(const cv::Mat &baseImage, const cv::Mat &image) override;
    void reset() noexcept override;

  public:
    float getScore() const noexcept { return mScore; }

  private:
    boost::optional<cv::Mat> mBaseImage;
    boost::optional<cv::Mat> mPreviousImage;
    float mScore = 0;

  private:
    cv::Mat template_img_greyscale, template_img_blur, template_img_binary;
    void prepareImage(cv::Mat &img,
                      cv::Mat &img_greyscale,
                      cv::Mat &img_blur,
                      cv::Mat &img_binary);

    void drawTargetContours(cv::Mat template_img_binary, cv::Mat &result_plot);

    void getTargetCentreRadi(cv::Mat template_img_greyscale,
                             cv::Point &target_centre,
                             float &radius);

    void getShotContours(cv::Mat input_img_blur,
                         cv::Mat previous_img_blur,
                         std::vector<std::vector<cv::Point>> &shot_contours);

    void getShotLocation(std::vector<std::vector<cv::Point>> shot_contours,
                         cv::Point &shot_location);

    float computeScore(cv::Point target_centre,
                       cv::Point shot_location,
                       float radius);
};

#endif
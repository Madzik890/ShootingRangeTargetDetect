#include "shottracking.hpp"
#include <boost/none.hpp>
#include <ios>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

namespace util {
// Constants are all upper case
const cv::Scalar GREEN = cv::Scalar(69, 255, 83);
const cv::Scalar LIGHT_GREEN = cv::Scalar(204, 255, 204);
const cv::Scalar DARK_GREEN = cv::Scalar(0, 120, 0);
const cv::Scalar RED = cv::Scalar(67, 57, 249);
const cv::Scalar DARK_RED = cv::Scalar(0, 0, 180);
const cv::Scalar GREY = cv::Scalar(50, 50, 50);
const cv::Scalar WHITE = cv::Scalar(255, 255, 255);
const cv::Scalar BLACK = cv::Scalar(0, 0, 0);
const cv::Scalar BACKGROUND = cv::Scalar(32, 32, 32);
}

cv::Mat 
ShotTracking::run(const cv::Mat &baseImage, const cv::Mat &img)
{
  std::cout << __PRETTY_FUNCTION__ << " -> run" << std::endl;

  if(!mBaseImage) {
    mBaseImage = img;
    prepareImage(*mBaseImage, template_img_greyscale, template_img_blur,template_img_binary);
    if(!mPreviousImage) mPreviousImage = mBaseImage;
  }

  std::cout << __PRETTY_FUNCTION__ << " -> mBaseImage empty: " << std::boolalpha << mBaseImage->empty() << std::endl;
  std::cout << __PRETTY_FUNCTION__ << " -> template_img_greyscale empty: " << std::boolalpha << template_img_greyscale.empty() << std::endl;
  std::cout << __PRETTY_FUNCTION__ << " -> template_img_blur empty: " << std::boolalpha << template_img_blur.empty() << std::endl;
  std::cout << __PRETTY_FUNCTION__ << " -> template_img_binary empty: " << std::boolalpha << template_img_binary.empty() << std::endl;

  cv::Mat image = img, result_plot;
  cv::Mat input_img_greyscale, input_img_blur, input_img_binary;
  cv::Mat previous_img_greyscale, previous_img_blur, previous_img_binary;

  prepareImage(image, input_img_greyscale, input_img_blur,
                input_img_binary);
  prepareImage(*mPreviousImage, previous_img_greyscale, previous_img_blur,
                previous_img_binary);

  result_plot = cv::Mat(img.size(), CV_8UC1, cv::Scalar(0, 0, 0));

  drawTargetContours(template_img_binary, result_plot);

  if(getDebugMode()) cv::imwrite("./shottracking_image_0.jpg", image);
  if(getDebugMode()) cv::imwrite("./shottracking_prev_image_0.jpg", *mPreviousImage);

  float radius;
  cv::Point target_centre = cv::Point(0, 0);
  getTargetCentreRadi(template_img_greyscale, target_centre, radius);

  std::cout << __PRETTY_FUNCTION__ << " -> radius: " << radius << std::endl;
  // get shot contours
  std::vector<std::vector<cv::Point>> shot_contours;
  getShotContours(input_img_blur, previous_img_blur, shot_contours);

  if(getDebugMode()) cv::imwrite("./shottracking_input_img_blur_0.jpg", input_img_blur);
  if(getDebugMode()) cv::imwrite("./shottracking_prev_img_blur_0.jpg", previous_img_blur);

  std::cout << __PRETTY_FUNCTION__ << " -> shot_contours size: " << shot_contours.size() << std::endl;

  std::sort(shot_contours.begin(), shot_contours.end(),
            [](const std::vector<cv::Point> &a, const std::vector<cv::Point> &b) {
                return cv::contourArea(a) > cv::contourArea(b);
  });

  // get shot location
  cv::Point shot_location = cv::Point(0, 0);
  getShotLocation(shot_contours, shot_location);
  std::cout << __PRETTY_FUNCTION__ << " -> shot_location xc: " << shot_location.x << " y: " << shot_location.y << std::endl;
  mScore = computeScore(target_centre, shot_location, radius);


  std::stringstream ss;

  // draw target centre
  cv::circle(result_plot, target_centre, 2, util::WHITE, 3, cv::LINE_AA);
  //cv::circle(*mBaseImage, target_centre, 2, util::DARK_RED, 3, cv::LINE_AA);

  // add target centre location values
  ss << "CENTRE (" << target_centre.x << ", " << target_centre.y << ")";
  cv::putText(result_plot, ss.str(),
              cv::Point(target_centre.x + 20, target_centre.y),
              cv::FONT_HERSHEY_SIMPLEX, 1, util::WHITE, 3);
  /*cv::putText(*mBaseImage, ss.str(),
              cv::Point(target_centre.x + 20, target_centre.y),
              cv::FONT_HERSHEY_SIMPLEX, 1, util::DARK_RED, 3);*/

  // draw outermost circle on result_plot
  cv::circle(result_plot, target_centre, radius, util::WHITE, 4, cv::LINE_AA);
  //cv::circle(*mBaseImage, target_centre, radius, util::DARK_RED, 4,
    //          cv::LINE_AA);

  // if the there is a valid score, draw it out on result plot
  if (mScore != 0) {
      // draw shot contour
      for (size_t idx = 0; idx < shot_contours.size(); idx++) {
          cv::drawContours(result_plot, shot_contours, idx, util::GREY, 3);
      }

      // draw shot location
      cv::drawMarker(result_plot, shot_location, util::LIGHT_GREEN,
                      cv::MARKER_CROSS, 20, 3);

      // add shot location values
      ss.str(std::string());   // clear stringstream
      ss << "Location: (" << shot_location.x << ", " << shot_location.y
          << ")";
      cv::putText(result_plot, ss.str(),
                  cv::Point(shot_location.x + 20, shot_location.y),
                  cv::FONT_HERSHEY_SIMPLEX, 1, util::LIGHT_GREEN, 3);
  }

  // add scores
  ss.str(std::string());   // clear stringstream
  ss << "SCORE: " << std::fixed << mScore;
  cv::putText(result_plot, ss.str(), cv::Point(20, 60),
              cv::FONT_HERSHEY_SIMPLEX, 1.5, util::LIGHT_GREEN, 3);

  std::cout << __PRETTY_FUNCTION__ << " -> done" << std::endl;
  mPreviousImage = image;
  return result_plot;
}

void 
ShotTracking::reset() noexcept
{
  mScore = 0; 
  mBaseImage = boost::none;
  mPreviousImage = boost::none;
}

void filterImage(cv::Mat &img,
                 cv::Mat &img_greyscale,
                 cv::Mat &img_blur,
                 cv::Mat &img_thresh) {
    cv::cvtColor(img, img_greyscale, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(img_greyscale, img_blur, cv::Size(15, 15), 0);
    cv::adaptiveThreshold(img_blur, img_thresh, 255, cv::ADAPTIVE_THRESH_MEAN_C,
                          cv::THRESH_BINARY_INV, 11, 3);
}

void 
ShotTracking::prepareImage(cv::Mat &img,
                                cv::Mat &img_greyscale,
                                cv::Mat &img_blur,
                                cv::Mat &img_binary) {

    /*// transform image into a squre ratio
    cv::resize(img, img, cv::Size(img.rows, img.rows));

    // Setup a rectangle to define the region of interest
    int width = img.size().width;
    int start = std::floor(width / 15);
    int end = width - start;

    // Crop the full image to the region of interest
    img = img(cv::Range(start, end), cv::Range(start, end));*/

    filterImage(img, img_greyscale, img_blur, img_binary);
}

void 
ShotTracking::drawTargetContours(cv::Mat template_img_binary,
                                      cv::Mat &result_plot) {
    std::vector<std::vector<cv::Point>> template_img_contours;
    // detect contour from the target model
    cv::findContours(template_img_binary, template_img_contours, cv::RETR_TREE,
                     cv::CHAIN_APPROX_TC89_KCOS);

    // loop through contours detected from the binary image
    for (size_t idx = 0; idx < template_img_contours.size(); idx++) {
        // polygon approximations
        std::vector<cv::Point> approx;
        cv::approxPolyDP(template_img_contours[idx], approx,
                         0.01 * cv::arcLength(template_img_contours[idx], true),
                         true);

        // detect other irregular shapes (numbers, circles)
        if (approx.size() >= 10 and
            cv::contourArea(template_img_contours[idx]) > 10) {
            cv::drawContours(result_plot, template_img_contours, idx,
                             cv::Scalar(50, 50, 50), 3);
        }
    }
}

void ShotTracking::getTargetCentreRadi(cv::Mat template_img_greyscale,
                                       cv::Point &target_centre,
                                       float &radius) {
    // detecting circle
    std::vector<cv::Vec3f> circles;

    // blur the model again with diff params for detecting circles
    cv::Mat img_blur_tmp;
    cv::blur(template_img_greyscale, img_blur_tmp, cv::Size(5, 5));

    int max_radii = 0, n = 1;

    while (max_radii <= 800) {

        cv::HoughCircles(
            img_blur_tmp, circles, cv::HOUGH_GRADIENT, 0.1, img_blur_tmp.rows / 1,
            100, 100, 100,
            max_radii   // change the max_radius to detect larger circles
        );

        for (size_t i = 0; i < circles.size(); i++) {
            // [x, y, radii] 3 items in circles[i]
            cv::Vec3i c = circles[i];
            // calculate an avearge of target centre on fly
            target_centre.x = target_centre.x + (c[0] - target_centre.x) / n;
            target_centre.y = target_centre.y + (c[1] - target_centre.y) / n;
            radius = c[2];
            ++n;
        }
        max_radii += 50;
    }
}

void ShotTracking::getShotContours
(
  cv::Mat input_img_blur,
  cv::Mat previous_img_blur,
  std::vector<std::vector<cv::Point>> &shot_contours) {

  cv::Mat img_diff;
  
  // calculate differences
  cv::absdiff(input_img_blur, previous_img_blur, img_diff);

  // convert the diff image into binary format
  cv::threshold(img_diff, img_diff, 50, 255, cv::THRESH_TOZERO);

  constexpr const auto factor = 0.01;

  // Rysowanie czarnego prostokąta (wypełnionego)
  cv::rectangle(img_diff, 
  cv::Point(0, 0),                // Lewy górny róg
  cv::Point(0 + img_diff.cols * factor, img_diff.rows), // Prawy dolny róg
  cv::Scalar(0, 0, 0),                       // Kolor (czarny)
  cv::FILLED);         

  cv::rectangle(img_diff, 
  cv::Point(0, 0),
  cv::Point(0 + img_diff.cols, img_diff.rows * factor), 
  cv::Scalar(0, 0, 0), 
  cv::FILLED);     

  cv::rectangle(img_diff, 
  cv::Point(0, img_diff.rows - img_diff.rows * factor),
  cv::Point(0 + img_diff.cols, img_diff.rows), 
  cv::Scalar(0, 0, 0), 
  cv::FILLED);         

  cv::rectangle(img_diff, 
  cv::Point(img_diff.cols - img_diff.cols * factor, 0),                // Lewy górny róg
  cv::Point(img_diff.cols, img_diff.rows), // Prawy dolny róg
  cv::Scalar(0, 0, 0),                       // Kolor (czarny)
  cv::FILLED);     

  if(getDebugMode()) cv::imwrite("./shottracking_image_diff_0.jpg", img_diff);

  // obtain contours of the shot    
  cv::findContours(img_diff, shot_contours, cv::RETR_TREE,
                     cv::CHAIN_APPROX_SIMPLE);
}

void getContourCentre(std::vector<cv::Point> &contour, cv::Point &centre) {
    cv::Moments mo = cv::moments(contour);
    centre = cv::Point(mo.m10 / mo.m00, mo.m01 / mo.m00);
}

void ShotTracking::getShotLocation(
    std::vector<std::vector<cv::Point>> shot_contours,
    cv::Point &shot_location) {
    // compute the shot centre as the centre of an irrigular shape
    for (auto &&contour : shot_contours) {
        cv::Point contour_centre;
        getContourCentre(contour, contour_centre);
        shot_location.x = shot_location.x + contour_centre.x * 1.1;
        shot_location.y = shot_location.y + contour_centre.y * 1.1;
    }

    // get a average centre of all contours
    std::cout << shot_location.x << " / " << shot_contours.size() << std::endl;
    std::cout << shot_location.y << " / " << shot_contours.size() << std::endl;
    if(shot_contours.size() == 0) {
        shot_location.x = 0;
        shot_location.y = 0;
        return;
    }
    shot_location.x = shot_location.x / shot_contours.size();
    shot_location.y = shot_location.y / shot_contours.size();
}

float ShotTracking::computeScore(cv::Point target_centre,
                                 cv::Point shot_location,
                                 float radius) {
    // euclidean distance between the centre and the shot
    double shot_distance = cv::norm(target_centre - shot_location);

    // distance between each ring
    float ring_distance = radius / 10;

    // highest mark - number of rings away from the centre
    float score = 11 - (shot_distance / ring_distance);

    // discard negative score
    score = score < 0 ? 0 : score;

    return score;
}
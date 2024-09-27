#include <iostream>
#include <exception>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>

#include "pipeline.hpp"
#include "modules/prepareimage.hpp"
#include "modules/targetdetect.hpp"
#include "modules/shottracking.hpp"

static constexpr const auto DEBUG_MODE = true;

int main()
{
    Pipeline pipeline;
    pipeline.setDebugMode(DEBUG_MODE);

    {
        const auto prepareImage = pipeline.createModule<PrepareImage>();
    }

    {
        const auto targetDetect = pipeline.createModule<TargetDetect>(); 
        targetDetect->setRawMode(true);       
        targetDetect->setResultImageX(640);
        targetDetect->setResultImageY(640);
    }
        
    const auto shotTracking = pipeline.createModule<ShotTracking>();
    auto score = shotTracking->getScore();
    for(int i = 0; i < 8; i++)
    {
        const std::string path = "../images/shot_" + std::to_string(i) + ".JPG";
        cv::Mat image = cv::imread(path);
        if (image.empty()) {
            std::cerr << "Nie można wczytać obrazu!" << std::endl;
            return -1;
        }

        try {
            const auto result = pipeline.run(image);
            score += shotTracking->getScore();
            std::cout << __PRETTY_FUNCTION__ << " -> score: " << shotTracking->getScore() << " totalScore: " << score << std::endl;
            cv::imshow("Done", result);
            cv::waitKey(0);
        }
        catch(const std::exception &ex) {
            std::cerr << "Exception: " << ex.what() << std::endl;
            return -2;
        }
    }

    return 0;
}
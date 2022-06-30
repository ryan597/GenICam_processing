#include <iostream>
#include <string>
#include <omp.h>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"

auto main(int argc, char** argv) -> int
{
    std::string folder = argv[1];
    int frames = std::atoi(argv[2]);
    int count{};
#pragma omp parallel for shared(folder, frames, count)
    for (int i=1; i<=frames; i++)
    {
        std::string filename = folder + std::to_string(i) + ".tiff";
        cv::Mat src = cv::imread(filename, -1);
        cv::Mat rgb, gray;
        cv::cvtColor(src, rgb, cv::COLOR_BayerGR2RGB);
        cv::cvtColor(rgb, gray, cv::COLOR_RGB2GRAY);
        cv::imwrite(filename, gray);
        count++;
        std::cout << "Processed: " << count << '\n';
    }
}

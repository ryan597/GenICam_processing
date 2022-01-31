#include <iostream>
#include <string>

#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/videoio.hpp"

class VideoProcessor
{
private:
    cv::VideoWriter m_writer;
    std::string m_filename;
    std::array<char, 4> m_codec;
    int m_Index;
    int m_digits;
    double m_framerate;
    bool m_isColour;
    std::string m_extension;


public:
    auto writeFrame(cv::Mat& pImg) -> void;
    auto setOutput(const std::string& filename,
                   const std::array<char, 4> codec={'X', 'V', 'I', 'D'},
                   const double framerate=20.0,
                   const bool isColour=true) -> void;
};

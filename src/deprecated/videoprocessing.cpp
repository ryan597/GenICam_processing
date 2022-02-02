#include "Include/videoprocessing.hpp"

auto VideoProcessor::writeFrame(cv::Mat& pImg)  -> void
{
    m_writer.write(pImg);
}


auto VideoProcessor::setOutput(const std::string& filename,
                               const std::array<char, 4> codec,
                               const double framerate=20.0,
                               const bool isColour=true) -> void
{
    m_filename = filename;
    m_codec = codec;
    m_framerate = framerate;
    m_isColour = isColour;
}

// Class holds metadata, stores imgs in buffers until time to write.
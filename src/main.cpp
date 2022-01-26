#include <iostream>
#include <string>
#include <chrono>

#include <omp.h>

#include <cvb/image.hpp>
#include <cvb/device_factory.hpp>
//#include <cvb/utilities/system_info.hpp>
#include <cvb/driver/stream.hpp>

#include "cameraconfig.hpp"


auto argsHelper() -> void
{
    std::cout << "Usage: $ ./main MAX_IMAGES IMAGE_DIRECTORY (BUFFERS WIDTH HEIGHT)\n"
              << "Example: ./main 1000 data/test (100 2560 2048)\n";
}


auto main(int argc, char** argv) -> int
{
    if (argc < 2)
    {
        argsHelper();
        return 1;
    }

    // Read config params
    const int numImages = std::atoi(argv[1]);
    const std::string imageDir = (argc > 2) ? argv[2] : "data/test/";
    const int buffers = (argc > 3) ? std::atoi(argv[3]) : 100;
    const int width = (argc > 4) ? std::atoi(argv[4]) : 2560;  // For reducing the AOI and thus resolution
    const int height = (argc > 5) ? std::atoi(argv[5]) : 2048;
    const float framerate = (argc > 6) ? std::atoi(argv[6]) : 20.0;

    std::cout << "Initialising device...\n";
    auto device = findDevice();
    auto triggerSoftware = configureSettings(device, width, height, framerate);

    auto stream = device->Stream();
    // Increase buffer count, all images kept in RAM and written to memory after,
    // only needs to run once as this overwrites the on-device settings
    //stream->RingBuffer()->ChangeCount(buffers, Cvb::DeviceUpdateMode::UpdateDeviceImage);

    stream->Start();
    std::cout << "Acquisition started...\n";
    int count = 0;
    std::string imagePath = "";
    std::string paddedCount = "";
    // Camera internal tick frequency per second
    constexpr int deviceTickFrequency = 62500000;

    std::cout.precision(10);
    while (numImages - count > 0)
    {
        // Triggering of cameras
        while (stream->Statistics(Cvb::StreamInfo::NumBuffersPending) < buffers) 
        {
            triggerSoftware->Execute();
            std::cout << stream->Statistics(Cvb::StreamInfo::NumBuffersPending) << "\n";
        }

        //std::cout << "processing frames\n";
        // Now save images and free up the buffers again
        #pragma omp parallel for num_threads(6) private(imagePath) shared(count)
        for (int i=0; i < buffers; i++)
        {
            auto waitResult = stream->WaitFor(std::chrono::seconds(10));
            if (waitResult.Status == Cvb::WaitStatus::Timeout)
            {
                throw std::runtime_error("acquisition timeout");
            }
            auto acquisitionTime = waitResult.Image->RawTimestamp() / deviceTickFrequency;
            imagePath = imageDir + std::to_string(acquisitionTime) + ".bmp";

            //waitResult.Image->Save(imagePath);
            std::cout << "Frames: " << std::to_string(count) << "\t|| "
                      << "Captured at " << acquisitionTime << " seconds\n";
            #pragma omp atomic
            count++;
        }
    }

    stream->Stop();
    std::cout << "Finished.\n";
}

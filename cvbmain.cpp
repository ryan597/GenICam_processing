#include <iostream>
#include <string>

//#include <cvb/image.hpp>
#include <cvb/device_factory.hpp>
//#include <cvb/utilities/system_info.hpp>
#include <cvb/driver/stream.hpp>

#include "cvbconfig.hpp"

auto argsHelper() -> void
{
    std::cout << "Usage: $ ./main MAX_IMAGES IMAGE_DIRECTORY ACQUISITION_MODE PIXEL_FORMAT\n"
              << "Example: ./main 1000 data/test Continuous Mono8\n";
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
    std::string imageDir = (argc > 2) ? argv[2] : "data";
    //int width{};  // For reducing the AOI and thus resolution
    //int height{};

    std::cout << "Initialising device...\n";

    // open a device
    auto deviceList = Cvb::DeviceFactory::Discover(Cvb::DiscoverFlags::IgnoreVins);
    if (deviceList.empty())
    {
        std::cout << "No device available... Program exit.\n";
        return 1;
    }

    auto device = Cvb::DeviceFactory::Open(deviceList[0].AccessToken());
    std::cout << "Device open\n";
    if (device->StreamCount() == 0)
    {
        std::cout << "No stream available... Program exit.\n";
        return 1;
    }

    EnablePacketResend(device);
    DiscardCorruptFrames(device);
    // get the first stream of the device
    auto stream = device->Stream();
    stream->Start();
    std::cout << "Acquisition started...\n";

    unsigned int count = 0;

    for (int i = 0; i < numImages; i++)
    {
        auto waitResult = stream->WaitFor(std::chrono::seconds(10));

        if (waitResult.Status == Cvb::WaitStatus::Timeout)
            {
                throw std::runtime_error("acquisition timeout");
            }
        count++;
        std::string padded_count = std::to_string(count);
        padded_count.insert(0, 8 - padded_count.length(), '0');

        std::string imagePath = imageDir + padded_count + ".png";

        waitResult.Image->Save(imagePath);

        if (count % 100 == 0)
        {
            std::cout << "Frames " + std::to_string(count) + "\n";
        }
    }

    stream->Stop();
    std::cout << "Finished.\n";
}


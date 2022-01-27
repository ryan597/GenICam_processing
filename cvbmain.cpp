#include <iostream>
#include <string>


#include <cvb/device_factory.hpp>
#include <cvb/utilities/system_info.hpp>
#include <cvb/driver/stream.hpp>


auto argsHelper() -> void
{
    std::cout << "Usage: $ ./main MAX_IMAGES IMAGE_DIRECTORY ACQUISITION_MODE PIXEL_FORMAT\n"
              << "Example: ./main 1000 data/test Continuous Mono8\n";
}


auto main(int argc, char** argv) -> int
{

    // Read config params
    const unsigned int numImages = std::atoi(argv[1]);
    std::string imageDir = (argc > 2) ? argv[2] : "data";
    //std::string acquisitionMode = (argc > 3) ? argv[3] : "Continuous";
    //bool isSingleShot = (acquisitionMode == "SingleFrame");
    //std::string pixelFormat = (argc > 4) ? argv[4] : "";
    int width{};  // For reducing the AOI and thus resolution
    int height{};

    auto path = InstallPath();
    path += CVB_LIT("drivers/GenICam.vin");

    // open a device
    auto device = DeviceFactory::Open(path);

    // get the first stream of the device
    auto stream = device->Stream();
    stream->Start();

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
    }
}
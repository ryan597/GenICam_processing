#include <iostream>
#include <string>
//#include <memory>
#include <ctime>

#include <cvb/image.hpp>
#include <cvb/device_factory.hpp>
//#include <cvb/utilities/system_info.hpp>
#include <cvb/driver/stream.hpp>

#include "cvbconfig.hpp"
//#include "videoprocessing.cpp"


auto argsHelper() -> void
{
    std::cout << "Usage: $ ./main MAX_IMAGES IMAGE_DIRECTORY (BUFFERS WIDTH HEIGHT)\n"
              << "Example: ./main 1000 data/test (100 2560 2048)\n";
}

auto findDevice() -> Cvb::DevicePtr
{
    /*
     * Discover all devices connected, exits if none available
     */
    auto deviceList = Cvb::DeviceFactory::Discover(Cvb::DiscoverFlags::IgnoreVins, std::chrono::seconds(5));
    if (deviceList.empty())
    {
        std::cout << "No device available... Program exit.\n";
        exit(1);
    }
    auto device = Cvb::DeviceFactory::Open(deviceList[0].AccessToken());
    std::cout << "Device open\n";
    if (device->StreamCount() == 0)
    {
        std::cout << "No stream available... Program exit.\n";
        exit(1);
    }
    return device;
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
    const int buffers = (argc > 3) ? std::atoi(argv[3]) : 100;
    const int width = (argc > 4) ? std::atoi(argv[4]) : 2560;  // For reducing the AOI and thus resolution
    const int height = (argc > 5) ? std::atoi(argv[5]) : 2048;

    std::cout << "Initialising device...\n";
    auto device = findDevice();

    // GenICam Settings
    // Set to software trigger to fill buffers
    auto deviceNodeMap = device->NodeMap(CVB_LIT("Device"));
    auto triggerModeNode = deviceNodeMap->Node<Cvb::EnumerationNode>("TriggerMode");
    auto triggerSourceNode = deviceNodeMap->Node<Cvb::EnumerationNode>("TriggerSource");
    auto triggerSoftwareNode = deviceNodeMap->Node<Cvb::CommandNode>("TriggerSoftware");
    auto acquisitionFramerateNode = deviceNodeMap->Node<Cvb::FloatNode>("AcquisitionFrameRate");
    auto widthNode = deviceNodeMap->Node<Cvb::FloatNode>("Width");
    auto heightNode = deviceNodeMap->Node<Cvb::FloatNode>("Height");
    //auto offsetXNode = deviceNodeMap->Node<Cvb::FloatNode>("OffsetX");
    //auto offsetYNode = deviceNodeMap->Node<Cvb::FloatNode>("OffsetY");

    triggerModeNode->SetValue("Off");
    triggerSourceNode->SetValue("Software");
    acquisitionFramerateNode->SetValue(20.0);

    EnablePacketResend(device);
    DiscardCorruptFrames(device);

    auto stream = device->Stream();
    // Increase buffer count, all images kept in RAM and written to memory after
    //stream->RingBuffer()->ChangeCount(buffers, Cvb::DeviceUpdateMode::UpdateDeviceImage);

    stream->Start();
    std::cout << "Acquisition started...\n";
    int count = 0;
    std::string imagePath = "";
    std::string paddedCount = "";

    std::cout.precision(20);
    while (numImages - count > 0)
    {
        // Triggering of cameras
        //while (stream->Statistics(Cvb::StreamInfo::NumBuffersPending) < buffers) {
        //    triggerSoftwareNode->Execute();
        //    std::cout << stream->Statistics(Cvb::StreamInfo::NumBuffersPending) << "\n";
        //}

        std::cout << "processing frames\n";
        // Now save images and free up the buffers again
        while (stream->Statistics(Cvb::StreamInfo::NumBuffersPending) > 0)
        {
            paddedCount = std::to_string(count);
            paddedCount.insert(0, 8 - paddedCount.length(), '0');
            imagePath = imageDir + paddedCount + ".bmp";
            auto waitResult = stream->WaitFor(std::chrono::seconds(2));
            auto acquisitionTime = waitResult.Image->RawTimestamp() / 62500000;
            if (waitResult.Status == Cvb::WaitStatus::Timeout)
            {
                throw std::runtime_error("acquisition timeout");
            }
            //waitResult.Image->Save(imagePath);
            std::cout << "Frames: " << std::to_string(count) << "\t|| "
                      << "Captured at " << acquisitionTime << " seconds\n";
            count++;
        }
    }

    //VideoProcessor recorder;
    //recorder.setOutput("/data/test/vid.avi");
    //auto pImg = cv::Mat(waitResult.Image->Height(), waitResult.Image->Width(), CV_8UC3, waitResult.ImagePlane.Planes());
    //recorder.writeFrame(pImg);

    stream->Stop();
    std::cout << "Finished.\n";
}


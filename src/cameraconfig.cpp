#include "cameraconfig.hpp"

/*
 * Find connected devices and return a pointer to the first.
 * Checks that streams are also available.
 */
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

/*
 * Sets the device image size, framerate and triggering mode.
 * Enable packet resend and discarding of corrupt frames.
 */
auto configureSettings(Cvb::DevicePtr device, const int width, const int height, const float framerate) -> void
{
    // GenICam Settings
    // Set to software trigger to fill buffers
    auto deviceNodeMap = device->NodeMap(CVB_LIT("Device"));
    auto triggerModeNode = deviceNodeMap->Node<Cvb::EnumerationNode>("TriggerMode");
    auto triggerSourceNode = deviceNodeMap->Node<Cvb::EnumerationNode>("TriggerSource");
    auto triggerSoftwareNode = deviceNodeMap->Node<Cvb::CommandNode>("TriggerSoftware");
    auto acquisitionFramerateNode = deviceNodeMap->Node<Cvb::FloatNode>("AcquisitionFrameRate");
    auto widthNode = deviceNodeMap->Node<Cvb::FloatNode>("Width");
    auto heightNode = deviceNodeMap->Node<Cvb::FloatNode>("Height");
    //auto pixelFormatNode = deviceNodeMap->Node<Cvb::EnumerationNode>("PixelFormat");
    // auto offsetXNode = deviceNodeMap->Node<Cvb::FloatNode>("OffsetX");
    // auto offsetYNode = deviceNodeMap->Node<Cvb::FloatNode>("OffsetY");

    triggerModeNode->SetValue("Off");
    triggerSourceNode->SetValue("Software");
    acquisitionFramerateNode->SetValue(framerate);
    //pixelFormatNode->SetValue("BAYGR8");

    if (auto dataStreamPtr = DataStreamNodeMap(device))
    {
        EnablePacketResend(dataStreamPtr);
        DiscardCorruptFrames(dataStreamPtr);
    }
    else
    {
        std::cout << "Data Stream pointer is null... Failed to enable packet resend & discarding corrupt frames\n";
    }
}

/*
 * Return the DataStream Node Map from passed device pointer
 */
auto DataStreamNodeMap(Cvb::DevicePtr device) -> Cvb::NodeMapPtr
{
    // auto nodeMaps = dev->NodeMaps();  // All node maps
    return device->NodeMap("TLDataStream");
}

/*
 * Reports the number of lost frames during the current acquisition period.
 */
auto LostFrames(Cvb::NodeMapPtr dataStreamPtr) -> int
{
    auto node = dataStreamPtr->Node<Cvb::IntegerNode>(Cvb::String("Cust::NumBuffersLost"));
    std::cout << "Number of lost frames: " << node->Value() << '\n';
    return node->Value();
}

/*
 * Enables packet resend in DataStream NodeMap
 */
auto EnablePacketResend(Cvb::NodeMapPtr dataStreamPtr) -> void
{

    if (auto node = dataStreamPtr->Node<Cvb::BooleanNode>(Cvb::String("EnablePacketResend")))
    {
        node->SetValue(true);
        std::cout << "Successfully enabled packet resend.\n";
    }
}

/*
 * Discards corrupt frames in DataStream NodeMap
 */
auto DiscardCorruptFrames(Cvb::NodeMapPtr dataStreamPtr) -> void
{
    if (auto node = dataStreamPtr->Node<Cvb::BooleanNode>(Cvb::String("PassCorruptFrames")))
    {
        try
        {
            node->SetValue(false);
            std::cout << "Successfully disabled passing corrupt frames.\n";
        }
        catch (const std::exception &e)
        {
            std::cout << "Pass Corrupt Frames: Error disabling feature!\n";
        }
    }
}

#include <iostream>
#include <cvb/device_factory.hpp>

auto findDevice() -> Cvb::DevicePtr;
auto configureSettings(Cvb::DevicePtr device, const int width, const int height, const float framerate) -> Cvb::GenApi::CommandNodePtr;
auto DataStreamNodeMap(Cvb::DevicePtr dev) -> Cvb::NodeMapPtr;
auto LostFrames(Cvb::NodeMapPtr dataStreamPtr) -> int;
auto EnablePacketResend(Cvb::NodeMapPtr dataStreamPtr) -> void;
auto DiscardCorruptFrames(Cvb::NodeMapPtr dataStreamPtr) -> void;

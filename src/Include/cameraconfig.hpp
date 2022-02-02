#include <iostream>
#include <cvb/device_factory.hpp>

auto findDevice() -> Cvb::DevicePtr;
auto configureSettings(Cvb::DevicePtr device, const int width, const int height, const float framerate) -> void;
auto DataStreamNodeMap(Cvb::DevicePtr dev) -> Cvb::NodeMapPtr;
auto LostFrames(Cvb::NodeMapPtr dataStreamPtr) -> int;
auto EnablePacketResend(Cvb::NodeMapPtr dataStreamPtr) -> void;
auto DiscardCorruptFrames(Cvb::NodeMapPtr dataStreamPtr) -> void;

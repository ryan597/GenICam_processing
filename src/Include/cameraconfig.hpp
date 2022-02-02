#include <iostream>
#include <cvb/device_factory.hpp>

auto findDevice() -> Cvb::DevicePtr;
auto configureSettings(Cvb::DevicePtr device, const int width, const int height, const float framerate) -> void;
auto DataStreamNodeMap(Cvb::DevicePtr dev) -> Cvb::NodeMapPtr;
auto LostFrames(Cvb::DevicePtr dev) -> int64_t;
auto EnablePacketResend(Cvb::DevicePtr dev) -> void;
auto DiscardCorruptFrames(Cvb::DevicePtr dev) -> void;
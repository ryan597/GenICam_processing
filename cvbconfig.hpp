#include <iostream>

#include <cvb/device_factory.hpp>

auto DataStreamNodeMap(Cvb::DevicePtr dev) -> Cvb::NodeMapPtr;
auto LostFrames(Cvb::DevicePtr dev) -> int64_t;
auto EnablePacketResend(Cvb::DevicePtr dev) -> void;
auto DiscardCorruptFrames(Cvb::DevicePtr dev) -> void;

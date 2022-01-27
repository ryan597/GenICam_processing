#include <iostream>

#include <cvb/device_factory.hpp>

Cvb::NodeMapPtr DataStreamNodeMap(Cvb::DevicePtr dev);
auto LostFrames(Cvb::DevicePtr dev) -> int64_t;
auto EnablePacketResend(Cvb::DevicePtr dev) -> void;
auto DiscardCorruptFrames(Cvb::DevicePtr dev) -> void;

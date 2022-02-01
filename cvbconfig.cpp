#include "cvbconfig.hpp"

// Change framerate and resolutions plus other parameters.


/*
 * Return the DataStream Node Map from passed device pointer
 */
auto DataStreamNodeMap(Cvb::DevicePtr dev) -> Cvb::NodeMapPtr
{
    //auto nodeMaps = dev->NodeMaps();  // All node maps
    return dev->NodeMap("TLDataStream");
}

auto LostFrames(Cvb::DevicePtr dev) -> int64_t
{
    if (auto dataStreamNodeMap = DataStreamNodeMap(dev))
    {
        auto node = dataStreamNodeMap->Node<Cvb::IntegerNode>(Cvb::String("Cust::NumBuffersLost"));
        std::cout << "Number of lost frames: " << node->Value() << '\n';
        return node->Value();
    }
    return 0;
}

/*
 * Enables packet resend in DataStream NodeMap
 */
void EnablePacketResend(Cvb::DevicePtr dev)
{
  if (auto dataStreamNodeMap = DataStreamNodeMap(dev))
  {
    if (auto node = dataStreamNodeMap->Node<Cvb::BooleanNode>(Cvb::String("EnablePacketResend")))
    {
      node->SetValue(true);
      std::cout << "Successfully enabled packet resend.\n";
    }
  }
}

/*
 * Discards corrupt frames in DataStream NodeMap
 */
void DiscardCorruptFrames(Cvb::DevicePtr dev)
{
  if (auto dataStreamNodeMap = DataStreamNodeMap(dev))
  {
    if (auto node = dataStreamNodeMap->Node<Cvb::BooleanNode>(Cvb::String("PassCorruptFrames")))
    {
      try
      {
        node->SetValue(false);
        std::cout << "Successfully disabled passing corrupt frames.\n";
      }
      catch (const std::exception & e)
      {
        std::cout << "Pass Corrupt Frames: Error disabling feature!\n";
      }
    }
  }
}

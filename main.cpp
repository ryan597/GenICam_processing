#include "mvIMPACT_CPP/mvIMPACT_acquire.h"
#include "mvIMPACT_CPP/mvIMPACT_acquire_GenICam.h"
#include "utils.hpp"

auto argsHelper() -> void
{
    std::cout << "Usage: $ ./main MAX_IMAGES IMAGE_DIRECTORY ACQUISITION_MODE PIXEL_FORMAT\n"
              << "Example: ./main 1000 data/test Continuous Mono8\n";
}

auto main(int argc, char** argv) -> int
{
    if (argc < 4)
    {
        argsHelper();
        return 1;
    }
    // Read config params
    int numImages = std::atoi(argv[1]);
    std::string imageDir = argv[2];
    std::string acquisitionMode = argv[3];
    bool isSingleShot = (acquisitionMode == "SingleFrame");
    std::string pixelFormat = argv[4];
    int width;  // For reducing the AOI and thus resolution
    int height;

    // Device initialization
    mvIMPACT::acquire::DeviceManager devMgr;
    if( devMgr.deviceCount() == 0 )
    {
        std::cout << "No device found! Unable to continue!\nPress enter to exit";
        char ch = std::getchar();
        return 1;
    }

    std::cout << "Initializing the device. This might take some time...\n";
    // create an interface to the first device found
    mvIMPACT::acquire::Device* pDev = devMgr[0];
    try
    {
        pDev->open();
        std::cout << "Using interface layout " << pDev->interfaceLayout.readS() << "\n";
    }
    catch(mvIMPACT::acquire::ImpactAcquireException& e )
    {
        // this e.g. might happen if the same device is already opened in another process...
        std::cout << "An error occurred while opening the device)\n"
                  << "Press any key to end the application...\n";
        char ch = getchar();
        return 0;
    }

    switch(pDev->interfaceLayout.read())
    {
    case mvIMPACT::acquire::TDeviceInterfaceLayout::dilGenICam:
    {
        mvIMPACT::acquire::GenICam::ImageFormatControl ifc(pDev);
        mvIMPACT::acquire::GenICam::AcquisitionControl ac(pDev);
        if ( width > 0 )
        {
            ifc.width.write(width);
        }
        if (height > 0)
        {
            ifc.height.write(height);
        }
        if (!pixelFormat.empty())
        {
            ifc.pixelFormat.writeS(pixelFormat);
        }
        if (!acquisitionMode.empty())
        {
            ac.acquisitionMode.writeS(acquisitionMode);
        }
        acquisitionMode = ac.acquisitionMode.readS();
        std::cout << "Device set to: " << ifc.pixelFormat.readS() << " " << ifc.width.read() << "x" << ifc.height.read() << "\n";
        break;
    }
    case mvIMPACT::acquire::TDeviceInterfaceLayout::dilDeviceSpecific:
    {
        CameraSettingsBase cs(pDev);
        if(width > 0)
        {
            cs.aoiWidth.write(width);
        }
        if(height > 0)
        {
            cs.aoiHeight.write(height);
        }
        std::cout << "Device set to: " << cs.aoiWidth.read() << "x" << cs.aoiHeight.read() << "\n";
        break;
    }
    default:
    {
        std::cout << "Interface layout not recognised...";
        return 2;
    }
    }

    capture(pDev, isSingleShot, imageDir, numImages);
    return 0;
}

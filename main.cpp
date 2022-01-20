#include <iostream>
#include <memory>
#include "mvIMPACT_CPP/mvIMPACT_acquire.h"

auto main(int argc, char* argv[]) -> int
{
    const int MAX_IMAGES = 1000;

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
    }
    catch(mvIMPACT::acquire::ImpactAcquireException& e )
    {
        // this e.g. might happen if the same device is already opened in another process...
        std::cout << "An error occurred while opening the device)\n"
                  << "Press any key to end the application...\n";
        char ch = getchar();
        return 0;
    }

    mvIMPACT::acquire::FunctionInterface fi(pDev);
    fi.acquisitionStart();
    for(int i = 0; i < MAX_IMAGES; i++)
    {
        auto requestNr = fi.imageRequestWaitFor(10000);
        auto pRequest = fi.getRequest(requestNr);
        std::cout << "Image captured: "
                  << pRequest->imageOffsetX.read()
                  << "x" << pRequest->imageOffsetY.read()
                  << "@" << pRequest->imageWidth.read()
                  << "x" << pRequest->imageHeight.read()
                  << "\n";
    }
    fi.acquisitionStop();
    std::cout << "Finished...\n";
    return 0;
}
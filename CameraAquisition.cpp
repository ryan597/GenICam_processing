// Main functions for interacting with the GeniCam device to begin aquisition.

/*
###############################################################################

        Written by Ryan Smith
        ryan.smith@ucdconnect.ie
        github.com/ryan597/GenICam_processing

###############################################################################
*/

#include <iostream>

#include <mvIMPACT_CPP/mvIMPACT_acquire.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace mvIMPACT::acquire;


void threadFn(Device* pDev)
{
    // create an instance of the function interface for this device
    // (this would also initialise a device if necessary)
    FunctionInterface fi(pDev);
    // send the request for one image down to the driver
    fi.imageRequestSingle();

    // pre-fill the capture queue. There can be more than 1 queue for some devices, but for this sample
    // we will work with the default capture queue. If a device supports more than one capture or result
    // queue, this will be stated in the manual. If nothing is mentioned about it, the device supports one
    // queue only. Request as many images as possible. If there are no more free requests 'DEV_NO_FREE_REQUEST_AVAILABLE'
    // will be returned by the driver.
    int result = DMR_NO_ERROR;
    SystemSettings ss( pThreadParameter->pDev );
    const int REQUEST_COUNT = ss.requestCount.read();
    for( int i=0; i<REQUEST_COUNT; i++ )
    {
    result = fi.imageRequestSingle();
    if( result != DMR_NO_ERROR )
    {
        std::cout << "Error while filling the request queue: " << ImpactAcquireException::getErrorCodeAsString( result ) << std::endl;
    }
    }

    // run thread loop
    const Request* pRequest = 0;
    int requestNr = -1;
    while( !boTerminated )
    {
    // wait for results from the default capture queue
    requestNr = fi.imageRequestWaitFor( 500 );
    if( fi.isRequestNrValid( requestNr ) )
    {
        pRequest = fi.getRequest(requestNr);
        if( pRequest->isOK() )
        {
        // display/process/store or do whatever you like with the image here
        }
        else
        {
        std::cout << "Error: Request result " << pRequest->requestResult.readS() << std::endl;
        }
        // unlock the buffer in order to allow the driver to use it again.
        // Afterwards the image buffer data becomes invalid for the user!
        fi.imageRequestUnlock( requestNr );
        // send a new request down to the driver.
        fi.imageRequestSingle();
    }
    else
    {
        std::cout << "Error: There has been no request in the queue!" << std::endl;
    }
    }

    // free resources, clear all queues of the acquisition engine
    fi.imageRequestReset( 0, 0 );


    int requestNr = fi.imageRequestWaitFor( timeout_ms );

    if( fi.isRequestNrValid( requestNr ) )
    {
        // processing, displaying whatever
        fi.imageRequestUnlock( requestNr );
    }

}


int main(){

    DeviceManager devMgr;
    // Access the first available genicam device
    Device* pDev = devMgr[0];  // getDeviceFromUserInput( devMgr );
    if( !pDev )
    {
        std::cout << "Unable to continue!";
        std::cout << "Press [ENTER] to end the application" << std::endl;
        std::cin.get();
        return 0;
    }

    std::cout << "Initialising the device. This might take some time..." << std::endl;
    try
    {
        pDev->open();
    }
    catch( const ImpactAcquireException& e )
    {
        // this e.g. might happen if the same device is already opened in another process...
        std::cout << "An error occurred while opening device " << pDev->serial.read()
             << "(error code: " << e.getErrorCodeAsString() << ")." << std::endl
             << "Press [ENTER] to end the application..." << std::endl;
        std::cin.get();
        return 0;
    }



    return 0;
}
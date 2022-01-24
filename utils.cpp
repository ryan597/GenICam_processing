#include "utils.hpp"


auto getPixelFormat(mvIMPACT::acquire::Request* pRequest) -> int
{
    int openCVDataType {};
    switch(pRequest->imagePixelFormat.read())
    {
        case ibpfMono8:
            openCVDataType = CV_8UC1;
            break;
        case ibpfMono10:
        case ibpfMono12:
        case ibpfMono14:
        case ibpfMono16:
            openCVDataType = CV_16UC1;
            break;
        case ibpfMono32:
            openCVDataType = CV_32SC1;
            break;
        case ibpfBGR888Packed:
        case ibpfRGB888Packed:
            openCVDataType = CV_8UC3;
            break;
        case ibpfRGBx888Packed:
            openCVDataType = CV_8UC4;
            break;
        case ibpfRGB101010Packed:
        case ibpfRGB121212Packed:
        case ibpfRGB141414Packed:
        case ibpfRGB161616Packed:
            openCVDataType = CV_16UC3;
            break;
        case ibpfMono12Packed_V1:
        case ibpfMono12Packed_V2:
        case ibpfBGR101010Packed_V2:
        case ibpfRGB888Planar:
        case ibpfRGBx888Planar:
        case ibpfYUV422Packed:
        case ibpfYUV422_10Packed:
        case ibpfYUV422_UYVYPacked:
        case ibpfYUV422_UYVY_10Packed:
        case ibpfYUV422Planar:
        case ibpfYUV444Packed:
        case ibpfYUV444_10Packed:
        case ibpfYUV444_UYVPacked:
        case ibpfYUV444_UYV_10Packed:
        case ibpfYUV444Planar:
        case ibpfYUV411_UYYVYY_Packed:
            std::cout << "ERROR! Don't know how to render this pixel format (" << pRequest->imagePixelFormat.readS() << ") in OpenCV! Select another one e.g. by writing to mvIMPACT::acquire::ImageDestination::pixelFormat!\n";
            std::exit( 42 );
            break;
        default:
            break;
    }
    return openCVDataType;
}


auto print_statistics(mvIMPACT::acquire::Device* pDev, mvIMPACT::acquire::Statistics stats, unsigned int count) -> void
{
    std::cout << "Info from " << pDev->serial.read()
              << ": FPS: " << stats.framesPerSecond.readS()
              << ", ErrorCount: " << stats.errorCount.readS()
              << ", CaptureTime (s): " << stats.captureTime_s.readS()
              << ", Image count: " << count
              << "\n";
}


auto capture(mvIMPACT::acquire::Device* pDev, bool isSingleShot, std::string imageDir, const unsigned int numImages) -> void
{
    mvIMPACT::acquire::Statistics stats(pDev);
    mvIMPACT::acquire::FunctionInterface fi(pDev);
    mvIMPACT::acquire::SystemSettings ss(pDev);
    // Do not convert Bayer data to reduce CPU load
    mvIMPACT::acquire::ImageProcessing ip(pDev);
    if (ip.colorProcessing.isValid())
    {
        ip.colorProcessing.write(mvIMPACT::acquire::TColorProcessingMode::cpmRaw);
    }
    mvIMPACT::acquire::TDMR_ERROR result = mvIMPACT::acquire::TDMR_ERROR::DMR_NO_ERROR;
    fi.acquisitionStart();
    int requestCount = 0;
    if (isSingleShot)
    {
        fi.imageRequestSingle();
        requestCount++;
    }
    else
    {
        // Send all requests now
        while ((result = static_cast<mvIMPACT::acquire::TDMR_ERROR>(fi.imageRequestSingle())) == mvIMPACT::acquire::TDMR_ERROR::DMR_NO_ERROR)
        {
            requestCount++;
        }
    }
    std::cout << requestCount << " buffer" << ( ( requestCount > 1 ) ? "s" : "" ) << " requested\n";

    if( ( result != DMR_NO_ERROR ) && ( result != DEV_NO_FREE_REQUEST_AVAILABLE ) )
    {
        std::cout << "'FunctionInterface.imageRequestSingle' returned with an unexpected result: "
                  << result << "("
                  << mvIMPACT::acquire::ImpactAcquireException::getErrorCodeAsString(result) << ")\n";
    }
    if( ss.requestCount.hasMaxValue() )
    {
        std::cout << "Maximum request count: " << ss.requestCount.getMaxValue() << "\n";
    }
    const unsigned int timeout_ms = 1000;
    unsigned int count = 0;
    bool isTerminated = false;
    int requestNr{};
    mvIMPACT::acquire::Request* pRequest = nullptr;
    mvIMPACT::acquire::Request* pPreviousRequest = nullptr;
    while (!isTerminated)
    {
        requestNr = fi.imageRequestWaitFor(timeout_ms);
        if (fi.isRequestNrValid(requestNr))
        {
            pRequest = fi.getRequest(requestNr);
            if (pRequest->isOK())
            {
                count++;
                if (count % 100 == 0)
                {
                    print_statistics(pDev, stats, count);
                }
                // Write image to disk
                //int dataType = getPixelFormat(pRequest);
                //cv::Mat image(cv::Size(pRequest->imageWidth.read(), pRequest->imageHeight.read()), dataType, pRequest->imageData.read());
                std::string padded_count = std::to_string(count);
                padded_count.insert(0, 8 - padded_count.length(), '0');
                pRequest->save(imageDir + "/" + padded_count + ".png", mvIMPACT::acquire::TImageFileFormat::iffPNG);
                //imwrite(imageDir + "/" + padded_count + ".png", image);
                //cv::waitKey(5);
            }
            else
            {
                std::cout << "Error: " << pRequest->requestResult.readS() << "\n";
            }

            // Unlock the buffer and send request back to function interface
            if (pPreviousRequest)
            {
                pPreviousRequest->unlock();
            }
            pPreviousRequest = pRequest;
            if (!isSingleShot)
            {
                fi.imageRequestSingle();
            }
        }
        // Termination condition
        if (isSingleShot || count >= numImages)
        {
            isTerminated = true;
        }
    }
    // Stop acquision if continuous mode and clean up queues
    if (!isSingleShot)
    {
        fi.acquisitionStop();
    }
    fi.imageRequestReset(0, 0);
}

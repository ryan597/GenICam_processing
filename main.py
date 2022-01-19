import os
import platform
import sys
import ctypes
import cv2
import numpy
import argparse
from PIL import Image

# import all the stuff from mvIMPACT Acquire into the current scope
from mvIMPACT import acquire
from mvIMPACT.Common import exampleHelper

"""
TO-DO: 
    Argument parsing
    Refactoring - remove helpers for parsed args and built-in funcs
"""

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-p", "--path", help="Path of directory to save images")
    parser.add_argument("-n", "--num_images", help="Max number of images to capture")
    parser.add_argument("-d", "--device", help="Device number to select, 0 for connected device, 1 for virtual device")

    args = parser.parse_args()
    path = args.path
    max_frames = int(args.num_images)
    device_number = int(args.device)

    # Select device, virtual devices available
    devMgr = acquire.DeviceManager()
    pDev = devMgr.getDevice(device_number)
    pDev.open()

    fi = acquire.FunctionInterface(pDev)
    statistics = acquire.Statistics(pDev)
    
    while fi.imageRequestSingle() == acquire.DMR_NO_ERROR:
        print("Buffer queued")
    pPreviousRequest = None

    fi.acquisitionStart()
    for i in range(max_frames):
        requestNr = fi.imageRequestWaitFor(10000)
        if fi.isRequestNrValid(requestNr):
            pRequest = fi.getRequest(requestNr)
            if pRequest.isOK:
                channelType = numpy.uint16 if pRequest.imageChannelBitDepth.read() > 8 else numpy.uint8
                if i%100 == 0:  # statistics every 100 frames
                    print("Info from " + pDev.serial.read() +
                             ": FPS: " + statistics.framesPerSecond.readS() +
                             ", Frame Count: " + statistics.frameCount.readS() +
                             ", Error Count: " + statistics.errorCount.readS() +
                            ", Capture Time (s): " + statistics.captureTime_s.readS())
                
                cbuf = (ctypes.c_char * pRequest.imageSize.read()).from_address(int(pRequest.imageData.read()))
                arr = numpy.fromstring(cbuf, dtype = channelType)
                arr.shape = (pRequest.imageHeight.read(), pRequest.imageWidth.read(), pRequest.imageChannelCount.read()+1)
                img = arr[:,:,:3]

                cv2.imwrite(f"data/{path}/{i}.png", img)

            if pPreviousRequest != None:
                pPreviousRequest.unlock()
            pPreviousRequest = pRequest
            fi.imageRequestSingle()
        else:
            print("imageRequestWaitFor failed (" + str(requestNr) + ", " + acquire.ImpactAcquireException.getErrorCodeAsString(requestNr) + ")")

    fi.acquisitionStop()
    print(f"Finished...")

    """
    #### callback
    from mvIMPACT import acquire

    class MyCallback(acquire.ComponentCallback):
      def __init__(self, pUserData=None):
        acquire.ComponentCallback.__init__(self)
        self.pUserData_ = pUserData
        self.executeHitCount_ = 0

      def execute(self, c, pUserData):
        try:
          # here you would actually do what is important for you! Please IGNORE the 'pUserData' parameter
          # coming into this function! Use the one you did provide in you constructor call instead for
          # whatever is necessary (e.g. cast it to an instance of a class you might want to call). Counting
          # the number of times this function got called is a silly example only! Remove this in a real world
          # application!
          self.executeHitCount_ += 1
        except Exception as e:
          print("An exception has been raised by code that is not supposed to raise one: '" + str(e) +
                "'! If this is NOT handled here the application will crash as this Python exception instance will be returned back into the native code that fired the callback!")

    cb = MyCallback(self) # Here we pass the instance of the class we are currently in to the callback.
    """

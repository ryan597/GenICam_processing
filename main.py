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


# Select device, virtual devices available
devMgr = acquire.DeviceManager()
pDev = exampleHelper.getDeviceFromUserInput(devMgr)
if pDev == None:
    exampleHelper.requestENTERFromUser()
    sys.exit(-1)
pDev.open()

print("Please enter the number of buffers to capture followed by [ENTER]: ", end='')
framesToCapture = exampleHelper.getNumberFromUser()
if framesToCapture < 1:
    print("Invalid input! Please capture at least one image")
    sys.exit(-1)

fi = acquire.FunctionInterface(pDev)
statistics = acquire.Statistics(pDev)

while fi.imageRequestSingle() == acquire.DMR_NO_ERROR:
    print("Buffer queued")
pPreviousRequest = None

exampleHelper.manuallyStartAcquisitionIfNeeded(pDev, fi)
for i in range(framesToCapture):
    requestNr = fi.imageRequestWaitFor(10000)
    if fi.isRequestNrValid(requestNr):
        pRequest = fi.getRequest(requestNr)
        if pRequest.isOK:
            if i%100 == 0:  # statistics every 100 frames
                print("Info from " + pDev.serial.read() +
                         ": " + statistics.framesPerSecond.name() + ": " + statistics.framesPerSecond.readS() +
                         ", " + statistics.errorCount.name() + ": " + statistics.errorCount.readS() +
                         ", " + statistics.captureTime_s.name() + ": " + statistics.captureTime_s.readS())
            # For systems with NO mvDisplay library support
            cbuf = (ctypes.c_char * pRequest.imageSize.read()).from_address(int(pRequest.imageData.read()))
            channelType = numpy.uint16 if pRequest.imageChannelBitDepth.read() > 8 else numpy.uint8
            arr = numpy.fromstring(cbuf, dtype = channelType)
            arr.shape = (pRequest.imageHeight.read(), pRequest.imageWidth.read(), pRequest.imageChannelCount.read()+1)
            img = arr[:,:,:3]
            print(img.shape)

            cv2.imwrite(f"data/{i}.png", img)

        if pPreviousRequest != None:
            pPreviousRequest.unlock()
        pPreviousRequest = pRequest
        fi.imageRequestSingle()
    else:
        print("imageRequestWaitFor failed (" + str(requestNr) + ", " + acquire.ImpactAcquireException.getErrorCodeAsString(requestNr) + ")")

exampleHelper.manuallyStopAcquisitionIfNeeded(pDev, fi)
#exampleHelper.requestENTERFromUser()
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

import ctypes
import cv2
import numpy
import argparse

# import all the stuff from mvIMPACT Acquire into the current scope
from mvIMPACT import acquire
# from mvIMPACT.Common import exampleHelper

"""
TO-DO:
    Refactoring - remove helpers for parsed args and built-in funcs
"""


def get_channel_bits(pRequest):
    if pRequest.imageChannelBitDepth.read() > 8:
        channelType = numpy.uint16
    else:
        channelType = numpy.uint8
    return channelType


def print_statistics(pDev, stats):
    print("Info from " + pDev.serial.read() +
          ": FPS: " + stats.framesPerSecond.readS() +
          ", Frame Count: " + stats.frameCount.readS() +
          ", Error Count: " + stats.errorCount.readS() +
          ", Capture Time (s): " + stats.captureTime_s.readS())


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-p", "--path", help="Directory to save images in")
    parser.add_argument("-n", "--num_images", help="Max number of images")
    parser.add_argument("-d", "--device", help="Device number to select")

    args = parser.parse_args()
    path = args.path
    max_frames = int(args.num_images)
    device_number = int(args.device)

    # Select device, virtual devices available
    devMgr = acquire.DeviceManager()
    pDev = devMgr.getDevice(device_number)
    pDev.open()

    fi = acquire.FunctionInterface(pDev)
    stats = acquire.Statistics(pDev)

    while fi.imageRequestSingle() == acquire.DMR_NO_ERROR:
        print("Buffer queued")
    pPreviousRequest = None

    fi.acquisitionStart()
    for i in range(max_frames):
        requestNr = fi.imageRequestWaitFor(10000)
        if fi.isRequestNrValid(requestNr):
            pRequest = fi.getRequest(requestNr)
            if pRequest.isOK:
                if i == 0:
                    channelType = get_channel_bits(pRequest)
                if i % 100 == 0:
                    print_statistics(pDev, stats)

                cbuf = (ctypes.c_char * pRequest.imageSize.read()).\
                    from_address(int(pRequest.imageData.read()))
                arr = numpy.fromstring(cbuf, dtype=channelType)
                arr.shape = (pRequest.imageHeight.read(),
                             pRequest.imageWidth.read(),
                             pRequest.imageChannelCount.read() + 1)
                img = arr[:, :, :3]

                cv2.imwrite(f"data/{path}/{i}.png", img)

            if pPreviousRequest is not None:
                pPreviousRequest.unlock()
            pPreviousRequest = pRequest
            fi.imageRequestSingle()
        else:
            print("imageRequestWaitFor failed (" + str(requestNr) + ", " +
                  acquire.ImpactAcquireException.
                  getErrorCodeAsString(requestNr) + ")")

    fi.acquisitionStop()
    print("Finished...")


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

            self.executeHitCount_ += 1
        except Exception as e:
            print("An exception has been raised by code: '" + str(e))
# Pass the instance of the class we are currently in to the callback.
cb = MyCallback(self)
"""

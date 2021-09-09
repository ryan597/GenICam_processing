"""
Program to interact with GenICam devices. Stores images in a H5 dataset.
"""
import numpy as np
import h5py
import matplotlib.pyplot as plt

from harvesters.core import Harvester
from harvesters.util.pfnc import mono_location_formats, bgr_formats

###############################################################################
#                                   To Do
# Dont write first image to h5 file. init with empty shape (1000, h, w, p)
# convert image to save as rgb func
# save images as grayscale
# save correctly in h5 files s
###############################################################################


class H5ImageDataset():
    # readwrite : 'w' for writing new dataset
    #             'r' for reading images
    #             'a' for appending to existing dataset
    def __init__(self, h5path, component=None, readwrite='w', verbose=True):
        if readwrite == 'w':
            image = self.first_image(component)
            self.h5file = h5py.File(h5path, readwrite)
            dataset = self.h5file.create_dataset('images',
                                                 shape=(1,
                                                        self.height,
                                                        self.width,
                                                        self.pixel),
                                                 data=image,
                                                 dtype=np.int8,
                                                 maxshape=(None,
                                                           self.height,
                                                           self.width,
                                                           self.pixel))
            dataset.attrs.create('format', self.data_format)
            dataset.attrs['height'] = self.height,
            dataset.attrs['width'] = self.width,
            dataset.attrs['pixel'] = self.pixel
            self.h5file.close()
            if verbose:
                print(f"Writing to path: {h5path}\n",
                      f"Using data format: {self.data_format}\n",
                      f"Image shape: {image.shape}")

            self.dataset = h5py.File(h5path, 'a')['images']
        elif readwrite == 'r':
            self.dataset = h5py.File(h5path, 'r')['images']
        elif readwrite == 'a':
            self.dataset = h5py.File(h5path, 'a')['images']

    def first_image(self, component):
        self.set_format(component)
        return self.reshape_image(component)

    def write_image(self, component):
        index = len(self.dataset)
        image = self.reshape_image(component)
        self.dataset.resize(index+1, axis=0)
        self.dataset[index, :, :, :] = image
        print(index)

    def reshape_image(self, component):
        if self.data_format in mono_location_formats:
            image = component.data.reshape(self.height, self.width)
        else:
            image = component.data.reshape(self.height, self.width, self.pixel)
            if self.data_format in bgr_formats:
                # swap each R and B
                image = image[:, :, ::-1]
        return image

    def set_format(self, component):
        self.width = component.width
        self.height = component.height
        self.data_format = component.data_format
        self.pixel = int(component.num_components_per_pixel)

    def read(self, index='return all images'):
        if index=='return all images':
            return self.dataset
        else:
            image = self.dataset[index]
            return image

    def close(self):
        self.h5file.close()


def collect_images(h5path,
                   cti_file,
                   max_images=100
                   ):
    with Harvester() as h:
        h.add_cti_file(cti_file)
        h.update_device_info_list()
        with h.create_image_acquirer(0) as ia:
            ia.start_image_acquisition()
            count = 0
            while count < max_images:
                with ia.fetch_buffer() as buffer:
                    payload = buffer.payload
                    component = payload.components[0]
                    if count == 0:
                        dataset = H5ImageDataset(h5path, component=component)
                    else:
                        dataset.write_image(component)
                    count += 1
    dataset.close()


if __name__ == "__main__":

    H5PATH = './test.h5'
    MAX_IMAGES = 100
    CTI_FILE = '/opt/mvIMPACT_Acquire/lib/x86_64/mvGenTLProducer.cti'
    # '/opt/cvb-13.03.003/drivers/genicam/libGevTL.cti'

    #collect_images(H5PATH, CTI_FILE, MAX_IMAGES)

    dataset = H5ImageDataset(H5PATH, readwrite='r')
    images = dataset.read()
    img1 = images[0]
    print(img1.shape)
    plt.imshow(img1, 'gray')
    plt.show()

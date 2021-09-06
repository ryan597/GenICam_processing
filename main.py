"""
Program to interact with GenICam devices. Stores images in a H5 dataset.
"""
import numpy as np
import h5py

from harvesters.core import Harvester
from harvesters.util.pfnc import mono_location_formats, bgr_formats


class H5ImageDataset():
    # readwrite : 'w' for writing new dataset
    #             'r' for reading images
    #             'a' for appending to existing dataset
    def __init__(self, h5path, component=None, readwrite='w'):
        if readwrite == 'w':
            image = self.first_image(component)
            print(self.data_format)
            h5file = h5py.File(h5path, readwrite)
            dataset = h5file.create_dataset('images', data=image, dtype=np.int,
                                            maxshape=(None,
                                                      self.height,
                                                      self.width,
                                                      self.pixel))
            dataset.attrs['format'] = self.data_format,
            dataset.attrs['height'] = self.height,
            dataset.attrs['width'] = self.width,
            dataset.attrs['pixel'] = self.pixel
            h5file.close()
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

    def close(self):
        self.dataset.close()


if __name__ == "__main__":

    H5PATH = './test.h5'
    MAX_IMAGES = 10

    with Harvester() as h:
        # h.add_cti_file('/opt/cvb-13.03.003/drivers/genicam/libGevTL.cti')
        h.add_cti_file('/opt/mvIMPACT_Acquire/lib/x86_64/mvGenTLProducer.cti')
        h.update_device_info_list()
        with h.create_image_acquirer(0) as ia:
            ia.start_image_acquisition()
            count = 0
            while count < MAX_IMAGES:
                with ia.fetch_buffer() as buffer:
                    payload = buffer.payload
                    component = payload.components[0]
                    if count == 0:
                        dataset = H5ImageDataset(H5PATH, component=component)
                    else:
                        dataset.write_image(component)
                    count += 1
    dataset.close()

    dataset = H5ImageDataset(H5PATH, readwrite='r')
    images = dataset['images']
    img1 = images[0]
    print(img1.shape)

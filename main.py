from harvesters.core import Harvester

# h = Harvester()
#
# h.add_cti_file('/opt/cvb-13.03.003/drivers/genicam/libGevTL.cti')
# h.add_cti_file('/opt/mvIMPACT_Acquire/lib/x86_64/mvGenTLProducer.cti')
#
# h.update_device_info_list()
#
# ia = h.create_image_acquirer(0)
# ia.start_image_acquisition()
#
#
# ia.stop_acquisition()
#
# ia.destroy()
# h.reset()

count = 0

with Harvester() as h:
    h.add_cti_file('/opt/mvIMPACT_Acquire/lib/x86_64/mvGenTLProducer.cti')
    h.update_device_info_list()
    with h.create_image_acquirer(0) as ia:
        ia.start_image_acquisition()
        while count < 10000:
            with ia.fetch_buffer() as buffer:
                print(buffer)
                count += 1

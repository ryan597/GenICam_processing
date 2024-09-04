import gi
from gi.repository import Aravis

gi.require_version("Aravis", "0.8")


camera = Aravis.Camera.new(None)

camera.set_frame_rate(10)
camera.set_pixel_format(Aravis.PIXEL_FORMAT_BAYER_GR8)

payload = camera.get_payload()

[x, y, width, height] = camera.get_region()

print(f"Camera: {camera.get_model_name}")
print(f"ROI: {width}x{height} at {x}, {y}")
print(f"Payload: {payload}")
print(f"Pixel format: {camera.get_pixel_format_as_string()}")


stream = camera.create_stream(None, None)

for i in range(0, 100):
    stream.push_buffer(Aravis.Buffer.new_allocate(payload))

print("Start Acquisition")

camera.start_acquisition()

for i in range(0, 20):
    image = stream.pop_buffer()
    if image:
        print(image)
        stream.push_buffer(image)

    camera.stop_acquisition()


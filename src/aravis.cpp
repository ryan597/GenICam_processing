#include <cstdlib>
#include <cstdio>
#include <cstddef>

#include <omp.h>
#include <arv.h>
#include <CImg.h>

/*
arv_camera_set_frame_rate()
arv_camera_set_exposure_auto()
arv_camera_set_gain_auto()
*/

auto main(int argc, char **argv) -> int
{
    ArvCamera *camera;
    GError *error = NULL;

    // Connect to the first available camera
    camera = arv_camera_new(NULL, &error);
    arv_camera_gv_set_packet_size(camera, 8192, &error);

    if (ARV_IS_CAMERA(camera)) {
        ArvStream *stream;

        printf("Found camera '%s'\n", arv_camera_get_model_name(camera, NULL));

        // Create the stream object without callback
        stream = arv_camera_create_stream(camera, NULL, NULL, &error);
        if (ARV_IS_STREAM(stream)) {
            // Retrive the payload size for buffer creation
            const auto payload = arv_camera_get_payload (camera, &error);
            printf("Payload size '%u'\n", payload);
            if (error == NULL)
            {
                // Insert buffers in the stream buffer pool
                for (int i = 0; i < 20; i++)
                    arv_stream_push_buffer(stream, arv_buffer_new (payload, NULL));
            }

            if (error == NULL)
            {
                // Start the acquisition
                printf("Starting acquisition...\n");
                arv_camera_start_acquisition(camera, &error);
            }
            if (error == NULL)
            {
                // Retrieve 10 buffers
                unsigned char* p_data;  // 8 bit pointer
                size_t buffer_size;

                // Pop first image to get initialise height and width from buffer
                ArvBuffer *buffer;
                buffer = arv_stream_pop_buffer(stream);
                const auto image_width = arv_buffer_get_image_width(buffer);
                const auto image_height = arv_buffer_get_image_height(buffer);
                cimg_library::CImg<unsigned char> image(image_width, image_height);

                #pragma omp parallel for private(buffer, p_data, image) shared(stream, buffer_size, image_width, image_height)
                for (int i = 0; i < 100; i++) {
                    buffer = arv_stream_pop_buffer(stream);
                    printf("Frame %d ", i);

                    if (ARV_IS_BUFFER(buffer)) {
                        // Display some informations about the retrieved buffer
                        p_data = (unsigned char *) arv_buffer_get_data(buffer, &buffer_size);
                        cimg_forXY(image, x, y) {image(x,y) = (p_data[x + y * image_width]);}

                        image.save_bmp("test.bmp");
                        printf("Acquired %dx%d buffer\n",
                               image_width,
                               image_height);
                        // Don't destroy the buffer, but put it back into the buffer pool
                        arv_stream_push_buffer(stream, buffer);
                    }
                }
            }

            if (error == NULL)
                // Stop the acquisition
                arv_camera_stop_acquisition(camera, &error);

            // Destroy the stream object
            g_clear_object(&stream);
        }

        // Destroy the camera instance
        g_clear_object(&camera);
    }

    if (error != NULL) {
        // En error happened, display the correspdonding message
        printf("Error: %s\n", error->message);
        return EXIT_FAILURE;
    }


    return EXIT_SUCCESS;
}

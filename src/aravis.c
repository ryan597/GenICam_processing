#include <stdlib.h>
#include <stdio.h>

#include <arv.h>

/*
arv_camera_set_frame_rate()
arv_camera_set_exposure_auto()
arv_camera_set_gain_auto()
*/

int main (int argc, char **argv)
{
    ArvCamera *camera;
    GError *error = NULL;

    // Connect to the first available camera
    camera = arv_camera_new (NULL, &error);
    arv_camera_gv_set_packet_size(camera, 8192, &error);

    if (ARV_IS_CAMERA (camera)) {
        ArvStream *stream;

        printf ("Found camera '%s'\n", arv_camera_get_model_name (camera, NULL));

        // Create the stream object without callback
        stream = arv_camera_create_stream (camera, NULL, NULL, &error);
        if (ARV_IS_STREAM (stream)) {
            // Retrive the payload size for buffer creation
            guint payload = arv_camera_get_payload (camera, &error);
            printf("Payload size '%lu'\n", payload);
            if (error == NULL) {
                // Insert buffers in the stream buffer pool
                for (int i = 0; i < 20; i++)
                    arv_stream_push_buffer (stream, arv_buffer_new (payload, NULL));
            }

            if (error == NULL)
            {
                // Start the acquisition
                printf("Starting acquisition...\n");
                arv_camera_start_acquisition (camera, &error);
            }
            if (error == NULL) {
                // Retrieve 10 buffers
                for (int i = 0; i < 10; i++) {
                    ArvBuffer *buffer;
                    printf("Frame %d ", i);
                    printf("HEre\n");

                    buffer = arv_stream_pop_buffer (stream);
                    if (ARV_IS_BUFFER (buffer)) {
                        // Display some informations about the retrieved buffer
                        printf ("Acquired %dx%d buffer\n",
                            arv_buffer_get_image_width (buffer),
                            arv_buffer_get_image_height (buffer));
                        // Don't destroy the buffer, but put it back into the buffer pool
                        arv_stream_push_buffer (stream, buffer);
                    }
                }
            }

            if (error == NULL)
                // Stop the acquisition
                arv_camera_stop_acquisition (camera, &error);

            // Destroy the stream object
            g_clear_object (&stream);
        }

        // Destroy the camera instance
        g_clear_object (&camera);
    }

    if (error != NULL) {
        // En error happened, display the correspdonding message
        printf ("Error: %s\n", error->message);
        return EXIT_FAILURE;
    }


    return EXIT_SUCCESS;
}

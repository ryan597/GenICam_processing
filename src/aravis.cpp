#include <cstdlib>
#include <cstdio>
#include <cstddef>
#include <string>

#define cimg_use_tiff USE_TIFF

#include "omp.h"
#include "arv.h"
#include "CImg.h"


auto main(int argc, char **argv) -> int
{
    if (argc == 1)
    {
        printf("Usage ./aravis MaxFrames FilePath FrameRate \nUsing defaults \nMaxFrames: 100 \nFilePath: ../../data/test/ \nFrameRate: 20 \n");
    }

    const int max_frames = (argc > 1) ? std::atoi(argv[1]) : 100;
    const std::string filepath = (argc > 2) ? argv[2] : "../../data/test/";
    const int width = (argc > 3) ? std::atoi(argv[3]) : 2560;
    const int height = (argc > 4) ? std::atoi(argv[4]) : 2048;
    const double framerate = (argc > 5) ? std::atoi(argv[5]) : 20;

    ArvCamera *camera;
    GError *error = NULL;

    // Connect to the first available camera
    camera = arv_camera_new(NULL, &error);
    arv_camera_gv_set_packet_size(camera, 8192, &error);  // Use larger packets for better performance
    arv_camera_set_exposure_time_auto(camera, ARV_AUTO_CONTINUOUS, &error);
    arv_camera_set_gain_auto(camera, ARV_AUTO_CONTINUOUS, &error);
    arv_camera_set_region(camera, 0, 0, width, height, &error);
    arv_camera_set_frame_rate(camera, framerate, &error);  //


    printf("Framerate %f\n", arv_camera_get_frame_rate(camera, &error));
    if (error != NULL)
    {
        printf("Camera error!\n");
        return EXIT_FAILURE;
    }

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
                for (int i = 0; i < 200; i++)
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
                unsigned char* p_data{};  // 8 bit pointer
                size_t buffer_size{};

                ArvBuffer* buffer;
                cimg_library::CImg<unsigned char> image(width, height);

                guint64 completed_buffers{};
                guint64 failed_buffers{};
                guint64 underrun_buffers{};
                guint64* p_completed_buffers = &completed_buffers;
                guint64* p_failed_buffers = &failed_buffers;
                guint64* p_underrun_buffers = &underrun_buffers;

                int count{0};

                #pragma omp parallel for firstprivate(buffer_size, image, p_completed_buffers, p_failed_buffers, p_underrun_buffers) private(buffer, p_data)
                for (int i = 0; i < max_frames; i++) {
                    buffer = arv_stream_pop_buffer(stream);

                    if (ARV_IS_BUFFER(buffer))
                    {
                        p_data = (unsigned char *) arv_buffer_get_data(buffer, &buffer_size);
                        for (int x=0; x < width; x++)
                        {
                            for (int y=0; y < height; y++)
                            {
                                image(x, y) = (p_data[x + y * width]);
                            }
                        }
                        // Stream statistics
                        if (count % 10 == 0)
                        {
                            //arv_stream_get_statistics(stream, p_completed_buffers, p_failed_buffers, p_underrun_buffers);
                            printf("Frames: %d \t|| Completed Buffers: %lu \t || Failed Buffers: %lu \t || Underruns: %lu \n",
                                   count, completed_buffers, failed_buffers, underrun_buffers);
                        }
                        std::string image_path = filepath + "test" + std::to_string(i) + ".tiff";
                        image.save_tiff(image_path.c_str());
                        // Don't destroy the buffer, but put it back into the buffer pool
                        arv_stream_push_buffer(stream, buffer);
                        #pragma omp atomic
                        count++;
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

#include "check_time.cpp"
#include "retrieve_save.cpp"


auto main(int argc, char **argv) -> int
{
    if (argc == 1)
    {
        fprintf(stdout, "Usage ./main MaxFrames FilePath Width Height FrameRate MinToStart PixelFormat CameraID\n");
    }
    // Acquisition Settings
    const guint32 max_frames = (argc > 1) ? std::atoi(argv[1]) : 100;
    const std::string filepath = (argc > 2) ? argv[2] : "../../data/test/";
    const int width = (argc > 3) ? std::atoi(argv[3]) : 1500;  // max 2560
    const int height = (argc > 4) ? std::atoi(argv[4]) : 1500;  // max 2048
    const double framerate = (argc > 5) ? std::atoi(argv[5]) : 10;
    const int minute_to_start = (argc > 6) ? std::atoi(argv[6]) : 00;
    const int pixel_format = (argc > 7) ? std::atoi(argv[7]) : 8;
    const std::string cam_num = (argc > 8) ? argv[8] : "JAI Corporation-U507993";  // Leave blank to connect to first available

    // if using smaller resolution keep the image centered
    int x_offset = (2560 - width) / 2;
    int y_offset = (2048 - height) / 2;

    ArvCamera *camera;
    GError *error = NULL;

    if (cam_num.empty())
    {  // Connect to the first available camera
        camera = arv_camera_new(NULL, &error);
    }
    else
    {
    // connect to the camera with <vendor>-<seriel number>
    // Pi1 & Cam1 = JAI Corporation-U507994
    // Pi2 & Cam2 = JAI Corporation-U507993
        camera = arv_camera_new(cam_num.c_str(), &error);
    }
<<<<<<< Updated upstream

    //GInetAddress *addresshost = g_inet_address_new_from_string("10.1.22.135");
    //GInetAddress *addresscam = g_inet_address_new_from_string("10.1.22.185");
    //auto device = arv_gv_device_new(addresshost, addresscam, &error);
=======
>>>>>>> Stashed changes

    if (error == NULL)
    {
        if (pixel_format == 12)
        {
            arv_camera_set_pixel_format(camera, ARV_PIXEL_FORMAT_BAYER_GR_12, &error);
        }
        else
        {
            arv_camera_set_pixel_format(camera, ARV_PIXEL_FORMAT_BAYER_GR_8, &error);
        }

        arv_camera_gv_auto_packet_size(camera, &error);
        arv_camera_set_exposure_time_auto(camera, ARV_AUTO_CONTINUOUS, &error);
        arv_camera_set_gain_auto(camera, ARV_AUTO_CONTINUOUS, &error);
        arv_camera_set_region(camera, x_offset, y_offset, width, height, &error);
        arv_camera_set_frame_rate(camera, framerate, &error);
<<<<<<< Updated upstream
        arv_camera_gv_set_packet_delay(camera, 1000, &error);  // helps on RPi when framerate is high
=======
        arv_camera_gv_set_packet_delay(camera, 100000, &error);  // helps on RPi when framerate is high
>>>>>>> Stashed changes
        // Print for logging
        fprintf(stdout, "Max Frames: %d\nWidth: %d\nHeight: %d\nPacket Size: %u\nFramerate: %f\nPixelDepth: %d bits\n",
                max_frames, width, height, arv_camera_gv_get_packet_size(camera, &error), arv_camera_get_frame_rate(camera, &error), pixel_format);
    }

    if (ARV_IS_CAMERA(camera))
    {
        fprintf(stdout, "Found camera '%s'\n", arv_camera_get_device_serial_number(camera, NULL));

        // Create the stream object without callback
        ArvStream *stream = arv_camera_create_stream(camera, NULL, NULL, &error);

        if (ARV_IS_STREAM(stream))
        {
            // Retrive the payload size for buffer creation
            const auto payload = arv_camera_get_payload (camera, &error);
            fprintf(stdout, "Payload size '%u'\n", payload);
            if (error == NULL)
            {
                // Insert buffers in the stream buffer pool
                for (int i = 0; i < 100; i++)
                    arv_stream_push_buffer(stream, arv_buffer_new(payload, NULL));
            }

            if (error == NULL)
            {
                // Start the acquisition,
                fprintf(stdout, "Starting acquisition...\n");
                fflush(stdout);
                if (minute_to_start >= 0)
                {  // negative minute bypasses check_time for immediate acquisition
                    check_time(minute_to_start);
                }
<<<<<<< Updated upstream
                else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                }
                arv_camera_start_acquisition(camera, &error);
                // Raspberry Pi 4 with 4 threads
                std::thread t1(retrieve_images, stream, max_frames, width, height);
                std::thread t2(save_images, filepath, max_frames);
                //std::thread t3(save_images, filepath, max_frames);
=======
                
		arv_camera_start_acquisition(camera, &error);
                // Raspberry Pi 4 with 4 threads
                std::thread t1(retrieve_images, stream, max_frames, width, height);
                std::thread t2(save_images, filepath, max_frames);
                std::thread t3(save_images, filepath, max_frames);
>>>>>>> Stashed changes
                //std::thread t4(save_images, filepath, max_frames);

                t1.join();
                t2.join();
<<<<<<< Updated upstream
                //t3.join();
=======
                t3.join();
>>>>>>> Stashed changes
                //t4.join();
            }
            if (error == NULL)
            {
                // Stop the acquisition
                fprintf(stdout, "End of Acquisition\n");
                fflush(stdout);
                arv_camera_stop_acquisition(camera, &error);
            }
            // Destroy the stream object
            g_clear_object(&stream);
        }
        // Destroy the camera instance
        g_clear_object(&camera);
    }

    if (error != NULL)
    {
        // An error happened, display the correspdonding message
        fprintf(stderr, "Error %s: %s\n", cam_num.c_str(), error->message);
        g_clear_object(&camera);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

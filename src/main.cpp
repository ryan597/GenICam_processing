#include "check_time.cpp"
#include "retrieve_save.cpp"


static void stream_callback(void* user_data, ArvStreamCallbackType type, ArvBuffer* buffer)
{
    switch(type){
        case ARV_STREAM_CALLBACK_TYPE_INIT:
            //if (!arv_make_thread_realtime(10))
            //    printf ("Failed to make stream thread realtime\n");
            break;

        case ARV_STREAM_CALLBACK_TYPE_START_BUFFER:
            break;

        case ARV_STREAM_CALLBACK_TYPE_BUFFER_DONE:
            break;

        case ARV_STREAM_CALLBACK_TYPE_EXIT:
            break;
    }
}


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
    const std::string cam_num = (argc > 8) ? argv[8] : "";//"JAI Corporation-U507993";  // Leave blank to connect to first available

    // logging
    auto logfile = fopen((filepath + "log.txt").c_str(), "w");
    
    // if using smaller resolution keep the image centered
    const int x_offset = (argc > 9) ? std::atoi(argv[9]) : 0;
    const int y_offset = (argc > 10) ? std::atoi(argv[10]) : 0;
    
    std::cout << "max frames: " << max_frames
        << "\nfilepath: " << filepath
        << "\nwidth: " << width
        << "\nheight: " << height
        << "\nframerate: " << framerate
        << "\nmin to start: " << minute_to_start
        << "\npixel format: " << pixel_format 
        << "\ncamera name: " << cam_num
        << "\nxoffset: " << x_offset
        << "\nyoffset: " << y_offset << '\n';
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
        arv_camera_gv_set_packet_delay(camera, 100000, &error);  // helps on RPi when framerate is high
        // Print for logging
        fprintf(logfile, "Max Frames: %d\nWidth: %d\nHeight: %d\nPacket Size: %u\nFramerate: %f\nPixelDepth: %d bits\n",
                max_frames, width, height, arv_camera_gv_get_packet_size(camera, &error), arv_camera_get_frame_rate(camera, &error), pixel_format);
    }

    if (ARV_IS_CAMERA(camera))
    {
        fprintf(logfile, "Found camera '%s'\n", arv_camera_get_device_serial_number(camera, NULL));

        // Create the stream object without callback
        ArvStream *stream = arv_camera_create_stream(camera, stream_callback, NULL, &error);

        if (ARV_IS_STREAM(stream))
        {
            g_object_set(stream,
                        "socket-buffer", ARV_GV_STREAM_SOCKET_BUFFER_AUTO,
                        "socket-buffer-size", 0,
                        NULL);

            // Retrive the payload size for buffer creation
            const auto payload = arv_camera_get_payload (camera, &error);
            fprintf(logfile, "Payload size '%u'\n", payload);
            if (error == NULL)
            {
                // Insert buffers in the stream buffer pool
                for (int i = 0; i < 100; i++)
                    arv_stream_push_buffer(stream, arv_buffer_new(payload, NULL));
            }

            if (error == NULL)
            {
                // Start the acquisition,
                fprintf(logfile, "Starting acquisition...\n");
                // negative minute starts at next minute (ie minute_now + 1)
                // if trigger -1 at 16:45:21, then check_time returns at 16:46:00
                check_time(minute_to_start);

                arv_camera_start_acquisition(camera, &error);
                // Raspberry Pi 4 with 4 threads
                std::thread t1(retrieve_images, stream, max_frames, width, height, logfile);
                std::thread t2(save_images, filepath, max_frames);
                std::thread t3(save_images, filepath, max_frames);
                //std::thread t4(save_images, filepath, max_frames);

                t1.join();
                t2.join();
                t3.join();
                //t4.join();
            }
            if (error == NULL)
            {
                // Stop the acquisition
                fprintf(logfile, "End of Acquisition\n");
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
        fprintf(logfile, "Error %s: %s\n", cam_num.c_str(), error->message);
        g_clear_object(&camera);
	fclose(logfile);
        return EXIT_FAILURE;
    }

    fclose(logfile);
    return EXIT_SUCCESS;
}

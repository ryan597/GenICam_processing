#include <cstdlib>
#include <cstdio>
#include <cstddef>
#include <string>
#include <thread>
#include <mutex>
#include <deque>

#define cimg_use_tiff USE_TIFF

#include "arv.h"
#include "CImg.h"
#include "check_time.cpp"

std::deque<cimg_library::CImg<unsigned char>> image_deque;
std::mutex deque_mutex;
int saved_count{};

auto retrieve_images(ArvStream* stream, const int max_frames, const int width, const int height) -> void
{
    ArvBuffer *buffer;
    unsigned char* p_data{};  // 8 bit pointer
    std::size_t buffer_size{};
    cimg_library::CImg<unsigned char> image(width, height);
    int count{};
    unsigned long completed_buffers{};
    unsigned long failed_buffers{};
    unsigned long underrun_buffers{};
    guint64 time0{};
    for (int i = 0; i < max_frames; i++) {
        buffer = arv_stream_pop_buffer(stream);  // pop_buffer blocks until buffer is available
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
            deque_mutex.lock();
            image_deque.push_back(image);
            deque_mutex.unlock();
            guint64 time1 = arv_buffer_get_system_timestamp(buffer);
            double time_between_frames = (time1 - time0) / 1000000000; // Change from nanoseconds to seconds
            fprintf(stdout, "Time between frames: %f\n", time_between_frames);
            time0 = time1;
            arv_stream_push_buffer(stream, buffer);
            count++;
            // Stream statistics
            if (count % 10 == 0)
            {
                arv_stream_get_statistics(stream, &completed_buffers, &failed_buffers, &underrun_buffers);
                fprintf(stdout, "Frames: %d \t|| Completed Buffers: %lu \t || Failed Buffers: %lu \t || Underruns: %lu \n",
                    count, completed_buffers, failed_buffers, underrun_buffers);
                fflush(stdout);
            }
        }
    }
}

auto save_images(std::string filepath, const int max_frames) -> void
{
    while(saved_count < max_frames)
    {
        deque_mutex.lock();
        if (image_deque.size() != 0)
        {
            std::string image_path = filepath + std::to_string(saved_count) + ".tiff";
            auto image = image_deque.front();
            image_deque.pop_front();
            saved_count++;
            deque_mutex.unlock();
            image.save_tiff(image_path.c_str());
        }
        else
        {
             deque_mutex.unlock();
             std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }
}

auto main(int argc, char **argv) -> int
{
    if (argc == 1)
    {
        printf("Usage ./aravis MaxFrames FilePath Width Height FrameRate MinToStart \nUsing defaults \nMaxFrames: 100 \nFilePath: ../../data/test/ \nFrameRate: 20 \n");
    }

    const int max_frames = (argc > 1) ? std::atoi(argv[1]) : 100;
    const std::string filepath = (argc > 2) ? argv[2] : "../../data/test/";
    const int width = (argc > 3) ? std::atoi(argv[3]) : 1500;  // max 2560
    const int height = (argc > 4) ? std::atoi(argv[4]) : 1500;  // max 2048
    const double framerate = (argc > 5) ? std::atoi(argv[5]) : 10;
    const int minute_to_start = (argc > 6) ? std::atoi(argv[6]) : 00;

    ArvCamera *camera;
    GError *error = NULL;

    // Connect to the first available camera
    camera = arv_camera_new(NULL, &error);

    arv_camera_gv_set_packet_size(camera, 1500, &error);
    //arv_camera_gv_auto_packet_size(camera, &error);
    arv_camera_set_exposure_time_auto(camera, ARV_AUTO_CONTINUOUS, &error);
    arv_camera_set_gain_auto(camera, ARV_AUTO_CONTINUOUS, &error);
    arv_camera_set_region(camera, 0, 0, width, height, &error);
    arv_camera_set_frame_rate(camera, framerate, &error);
    arv_camera_set_trigger(camera, "Software", &error);
    printf("Max Frames %d\n", max_frames);
    printf("Width %d\nHeight %d\n", width, height);
    printf("Packet Size %u\n", arv_camera_gv_get_packet_size(camera, &error));
    printf("Framerate %f\n", arv_camera_get_frame_rate(camera, &error));
    if (error != NULL)
    {
        fprintf(stderr, "Camera error!\n");
        return EXIT_FAILURE;
    }

    if (ARV_IS_CAMERA(camera))
    {
        fprintf(stdout, "Found camera '%s'\n", arv_camera_get_model_name(camera, NULL));

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
                for (int i = 0; i < 30; i++)
                    arv_stream_push_buffer(stream, arv_buffer_new(payload, NULL));
            }

            if (error == NULL)
            {
                // Start the acquisition
                fprintf(stdout, "Starting acquisition...\n");
                fflush(stdout);
                check_time(minute_to_start);
                arv_camera_start_acquisition(camera, &error);
                // Raspberry Pi 4 with 4 threads
                std::thread t1(retrieve_images, stream, max_frames, width, height);
                std::thread t2(save_images, filepath, max_frames);
                std::thread t3(save_images, filepath, max_frames);
                std::thread t4(save_images, filepath, max_frames);

                t1.join();
                t2.join();
                t3.join();
                t4.join();
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
        // En error happened, display the correspdonding message
        fprintf(stderr, "Error: %s\n", error->message);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

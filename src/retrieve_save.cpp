#include <cstdlib>
#include <cstdio>
#include <string>
#include <thread>
#include <mutex>
#include <deque>

#define cimg_use_tiff USE_TIFF

#include "arv.h"
#include "CImg.h"

struct image_data
{
    cimg_library::CImg<unsigned char> img;
    guint32 buffer_id;
};

std::deque<image_data> image_deque;
std::mutex deque_mutex;
guint32 buffer_count{};

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
            guint32 id = arv_buffer_get_frame_id(buffer);
            deque_mutex.lock();
            image_deque.push_back(std::move(image_data{image, id}));
            deque_mutex.unlock();
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

auto save_images(std::string filepath, const guint32 max_frames) -> void
{
    while(buffer_count < max_frames)
    {
        deque_mutex.lock();
        if (image_deque.size() != 0)
        {
            auto image = image_deque.front().img;
            buffer_count = image_deque.front().buffer_id;
            std::string image_path = filepath + std::to_string(buffer_count) + ".tiff";
            image_deque.pop_front();
            deque_mutex.unlock();
            image.save_tiff(image_path.c_str());
        }
        else
        {
             deque_mutex.unlock();
             std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
}

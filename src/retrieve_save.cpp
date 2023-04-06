#include <cstdlib>
#include <cstdio>
#include <string>
#include <thread>
#include <mutex>
#include <deque>

#define cimg_use_tiff USE_TIFF
#define cimg_use_openmp USE_OPENMP

//#define use_12bit_pixel

#ifdef use_12bit_pixel
    #define BITPRECISION unsigned short
#else
    #define BITPRECISION unsigned char
#endif

#include "arv.h"
#include "CImg.h"
using namespace cimg_library;
//#include "plugins/bayer.h"

struct image_data
{
    CImg<BITPRECISION> img;
    guint64 buffer_id;
};

std::deque<image_data> image_deque;
std::mutex deque_mutex;
guint32 buffer_count{};

auto retrieve_images(ArvStream* stream, const int max_frames, const int width, const int height) -> void
{
    ArvBuffer *buffer;
    BITPRECISION* p_data{};
    std::size_t buffer_size{};
    CImg<BITPRECISION> image(width, height);
    int count{};
    unsigned long completed_buffers{};
    unsigned long failed_buffers{};
    unsigned long underrun_buffers{};
    for (int i = 0; i < max_frames; i++) {
        buffer = arv_stream_pop_buffer(stream);  // pop_buffer blocks until buffer is available
        if (ARV_IS_BUFFER(buffer))
        {
            p_data = (BITPRECISION *) arv_buffer_get_data(buffer, &buffer_size);
            for (int x=0; x < width; x++)
            {
                for (int y=0; y < height; y++)
                {
                    image(x, y) = (p_data[x + y * width]);
                }
            }
            guint64 id = arv_buffer_get_frame_id(buffer);

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
            auto image_pop = image_deque.front();
            image_deque.pop_front();
            buffer_count = image_pop.buffer_id;
            std::string image_path = filepath + std::to_string(buffer_count) + ".tiff";
            deque_mutex.unlock();

            auto image = image_pop.img;
            image.save_tiff(image_path.c_str());
        }
        else
        {
             deque_mutex.unlock();
             std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
    }
}

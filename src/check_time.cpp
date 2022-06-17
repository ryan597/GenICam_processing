#include <iostream>
#include <chrono>


auto check_current_time(int minute_to_start) -> void
{
    // checks the time until it is minute_to_start:00:000
    bool cond = true;
    while (cond)
    {
        auto timenow = std::chrono::system_clock::now();
        auto since_epoch = timenow.time_since_epoch();
        auto hours = std::chrono::duration_cast<std::chrono::hours>(since_epoch);
        since_epoch -= hours;
        auto minutes = std::chrono::duration_cast<std::chrono::minutes>(since_epoch);
        since_epoch -= minutes;
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(since_epoch);
        since_epoch -= seconds;
        auto milli = std::chrono::duration_cast<std::chrono::milliseconds>(since_epoch);
        since_epoch -= milli;
        auto micro = std::chrono::duration_cast<std::chrono::microseconds>(since_epoch);

        if (minutes.count() == minute_to_start && seconds.count() == 0 && milli.count() == 0)
        {
            cond = false;
            std::cout << minutes.count() << ':' << seconds.count() << ':' << milli.count() << ':' << micro.count() << '\n';
        }
    }
}

/*
auto main(int argc, char **argv) -> int
{
    int minute_to_start = std::atoi(argv[1]);
    check_current_time(minute_to_start);
}
*/

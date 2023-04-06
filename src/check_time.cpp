#include <iostream>
#include <chrono>


auto check_time(int minute_to_start) -> void
{
    // checks the time until it is minute_to_start:00:000
    bool cond = true;
    if (minute_to_start < 0)
    {
        auto timenow = std::chrono::system_clock::now();
        auto since_epoch = timenow.time_since_epoch();
        auto hours = std::chrono::duration_cast<std::chrono::hours>(since_epoch);
        since_epoch -= hours;
        auto minutes = std::chrono::duration_cast<std::chrono::minutes>(since_epoch);
        minute_to_start = minutes.count() + 1;
    }
    while (cond)
    {
        auto timenow = std::chrono::system_clock::now();
        auto since_epoch = timenow.time_since_epoch();
        auto hours = std::chrono::duration_cast<std::chrono::hours>(since_epoch);
        since_epoch -= hours;
        auto minutes = std::chrono::duration_cast<std::chrono::minutes>(since_epoch);

        if (minutes.count() == minute_to_start)
        {
            cond = false;
            std::cout << minutes.count() << '\n';
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

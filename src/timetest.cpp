#include <iostream>
#include <chrono>
#include <ctime>

int main()
{
    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();

    typedef std::chrono::duration<int, std::ratio_multiply<std::chrono::hours::period, std::ratio<8>
    >::type> Days; /* UTC: +8:00 */

    Days days = std::chrono::duration_cast<Days>(duration);
        duration -= days;
    auto hours = std::chrono::duration_cast<std::chrono::hours>(duration);
        duration -= hours;
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration);
        duration -= minutes;
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
        duration -= seconds;
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
        duration -= milliseconds;
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration);
        duration -= microseconds;
    auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);

    std::cout << hours.count() << ":"
              << minutes.count() << ":"
              << seconds.count() << ":"
              << milliseconds.count() << ":"
              << microseconds.count() << ":"
              << nanoseconds.count() << std::endl;
}

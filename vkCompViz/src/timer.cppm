export module timer;
import std;

export class Timer
{
    private:
        std::chrono::steady_clock::time_point start;
        std::chrono::steady_clock::time_point end;
    public:
        Timer()
        {
            start = std::chrono::steady_clock::now();
        }
        float elapsed()
        {
            end = std::chrono::steady_clock::now();
            return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()/1000.0f;
        }
        void waitUntilElapsed(float limit)
        {
            float elapsedTime = elapsed();
            if(elapsedTime < limit)
                std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int>(1000*std::round(limit - elapsedTime))));
        }
};

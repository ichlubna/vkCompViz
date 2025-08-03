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
            return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        }
        void waitUntilElapsed(float limit)
        {
            float elapsedTime = elapsed();
            if(elapsedTime < limit)
                std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(std::round(limit - elapsedTime))));
        }
};

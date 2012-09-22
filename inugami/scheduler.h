#ifndef INUGAMI_ENGINE_H
#define INUGAMI_ENGINE_H

namespace Inugami {

/** @brief A simple scheduler.
 *
 * Use this class to register functions to be called at regular intervals,
 * such as drawing and simulation functions.
 *
 */
class Scheduler
{
    public:
        Scheduler();
        virtual ~Scheduler();

        /** @brief Adds or updates a callback.
         * @param func Function to add or update.
         * @param freq Call frequency, in Hertz.
         * @return @a True if success, @a false if fail.
         */
        bool addCallback(void (*func)(), double freq);

        /** @brief Removes a callback.
         * @param func Function to remove.
         */
        void dropCallback(void (*func)());

        /** @brief Starts the scheduler.
         * This functions runs a loop that calls registered functions at the
         * specified intervals.  The loop continues while @a running is true.
         */
        int go();

        /**
         * Set this to @a false at any time to exit the @a go() cycle.
         */
        bool running;

    protected:
    private:
        struct Callback
        {
            void (*func)();
            double freq, wait, last;
        };

        static const int MAXCALLBACKS = 32;
        Callback callbacks[MAXCALLBACKS];
        int callbackCount;
};

} // namespace Inugami

#endif // INUGAMI_ENGINE_H

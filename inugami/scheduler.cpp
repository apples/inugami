#include "scheduler.h"

#include "math.h"
#include "opengl.h"

namespace Inugami {

Scheduler::Scheduler() :
    running(false), callbackCount(0)
{
    for (int i=0; i<MAXCALLBACKS; ++i)
    {
        callbacks[i].func = nullptr;
        callbacks[i].freq = 0.0;
        callbacks[i].wait = 0.0;
        callbacks[i].last = 0.0;
    }
}

Scheduler::~Scheduler()
{
    //dtor
}

bool Scheduler::addCallback(void (*func)(), double freq)
{
    for (int i=0; i<MAXCALLBACKS; ++i)
    {
        if (i == callbackCount)
        {
            callbacks[i].func = func;
            callbacks[i].freq = freq;
            callbacks[i].wait = 0.0;
            callbacks[i].last = glfwGetTime();
            ++callbackCount;
            return true;
        }
        if (callbacks[i].func == func)
        {
            callbacks[i].freq = freq;
            callbacks[i].wait = 0.0;
            callbacks[i].last = glfwGetTime();
            return true;
        }
    }
    return false;
}

void Scheduler::dropCallback(void (*func)())
{
    for (int i=0; i<MAXCALLBACKS; ++i)
    {
        if (i == callbackCount)
        {
            return;
        }
        if (callbacks[i].func == func)
        {
            for (int j=i; j<callbackCount-1; ++j)
            {
                callbacks[j].func = callbacks[j+1].func;
                callbacks[j].freq = callbacks[j+1].freq;
                callbacks[j].wait = callbacks[j+1].wait;
                callbacks[j].last = callbacks[j+1].last;
            }
            callbacks[callbackCount-1].func = nullptr;
            return;
        }
    }
}

int Scheduler::go()
{
    running = true;

    double currentTime = 0.0;
    double deltaTime = 0.0;

    while (running) {
        for (int i=0; i<callbackCount; ++i)
        {
            currentTime = glfwGetTime();
            deltaTime = currentTime - callbacks[i].last;
            callbacks[i].last = currentTime;

            if (callbacks[i].freq < 0.0)
            {
                callbacks[i].func();
                continue;
            }

            callbacks[i].wait += deltaTime * callbacks[i].freq;
            if (callbacks[i].wait >= 1.0)
            {
                callbacks[i].func();
                callbacks[i].wait = 0.0;
            }

            if (!running) break;
        }
    }

    return 0;
}

} // namespace Inugami

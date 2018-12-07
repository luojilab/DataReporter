//
// Created by bingjian on 2017/10/19.
//

#ifndef ENGINE_HANDLERTHREAD_H
#define ENGINE_HANDLERTHREAD_H

#include <thread>
#include <atomic>
#include "ThreadsafeQueue.h"
#include "TimeQueue.h"

namespace WTF {
    class HandlerThread {
    public:

        HandlerThread();

        ~HandlerThread();

        void start();

        void stop();

        void postMsg(std::function<void(void)> msg);

        void postPeriodTask(const TimeTask &task);

        void cancelPeriodTask(const TimeTask &task);

        void clearTask();

        bool isQueueEmpty();

    private:
        void run();

        void executeTask();

        void executePeriodTask();

    private:
        std::atomic_bool m_IsStop;
        TimeQueue<TimeTask > m_MsgQueue;
        std::mutex mut;
        std::condition_variable cond;
    };
}

#endif //ENGINE_HANDLERTHREAD_H

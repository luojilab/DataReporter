//
// Created by bingjian on 2017/10/19.
//
#include <functional>
#include <thread>
#include <chrono>
#include "HandlerThread.h"
#include "Platform.h"

#if PLATFORM(ANDROID)

#include "AndroidUtil.h"

#endif

namespace WTF {

    HandlerThread::HandlerThread() : m_IsStop(true) {

    }

    HandlerThread::~HandlerThread() {

    }

    void HandlerThread::start() {
        std::unique_lock<std::mutex> lck(mut);
        if (!m_IsStop) {
            return;
        }
        m_IsStop = false;

        std::function<void(void)> fun_run = std::bind(&HandlerThread::run, this);
        std::thread main_thread(fun_run);
        main_thread.detach();
    }

    void HandlerThread::stop() {
        std::unique_lock<std::mutex> lck(mut);
        m_IsStop = true;
        cond.notify_all();
    }

    void HandlerThread::clearTask() {
        m_MsgQueue.clear();
    }

    bool HandlerThread::isQueueEmpty() {
        return (m_MsgQueue.size() == 0);

    }

    void HandlerThread::run() {
#if PLATFORM(ANDROID)
        AndroidUtil::attachCurrentThread();
#endif
        while (!m_IsStop) {
            TimeTask task;
            {
                std::unique_lock<std::mutex> lck(mut);
                if (m_MsgQueue.size() == 0) {
                    cond.wait(lck);
                }


                task = m_MsgQueue.pop_min();
                if (!task.fun()) {
                    continue;
                }

                auto now = std::chrono::steady_clock::now();
                if (now < task.nextExecutionTime()) {
                    cond.wait_until(lck, task.nextExecutionTime());
                    auto wakeNow = std::chrono::steady_clock::now();
                    if (wakeNow < task.nextExecutionTime()) {
                        m_MsgQueue.push(task);
                        continue;
                    }
                }

                if (task.period() != 0) {
                    task.setNextExecutionTime(task.period());
                    m_MsgQueue.push(task);
                }
            }

            std::function<void(void)> fun_run = task.fun();
            if (fun_run) {
                fun_run();
            }

        }

#if PLATFORM(ANDROID)
        AndroidUtil::detachCurrentThread();
#endif
        delete this;
    }

    void HandlerThread::postMsg(std::function<void(void)> msg) {
        std::unique_lock<std::mutex> lck(mut);
        TimeTask timeTask(0, 0, msg);
        m_MsgQueue.push(timeTask);
        cond.notify_all();
    }

    void HandlerThread::postPeriodTask(const TimeTask &task) {
        std::unique_lock<std::mutex> lck(mut);
        m_MsgQueue.push(task);
        cond.notify_all();
    }

    void HandlerThread::cancelPeriodTask(const TimeTask &task) {
        std::unique_lock<std::mutex> lck(mut);
        m_MsgQueue.remove(task);
    }
}

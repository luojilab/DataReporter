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
        if (!m_IsStop) {
            return;
        }
        m_IsStop = false;
        std::function<void(void)> fun_run = std::bind(&HandlerThread::run, this);
        std::thread main_thread(fun_run);
        main_thread.detach();
    }

    void HandlerThread::stop() {
        m_IsStop = true;
        cond.notify_all();
    }

    void HandlerThread::clearTask() {
        std::unique_lock<std::mutex> lck(mut);
        m_MsgQueue.clear();
    }

    bool HandlerThread::isQueueEmpty() {
        return m_MsgQueue.empty();

    }

    void HandlerThread::executeTask() {
        std::shared_ptr<std::function<void(void)>> msg = m_MsgQueue.try_pop();
        if (msg != NULL) {
            std::function<void(void)> *msgPtr = msg.get();
            (*msgPtr)();
        }
    }

    void HandlerThread::executePeriodTask() {
        std::shared_ptr<TimeTask> task = m_PeriodTaskQueue.get_min();
        if (task != NULL) {
            if (task->m_status == TimeTask::CANCELLED) {
                m_PeriodTaskQueue.remove(*task);
                return;
            }
            auto now = std::chrono::steady_clock::now();
            if (now >= task->nextExecutionTime()) {
                std::function<void(void)> fun_run = task->fun();
                m_MsgQueue.push(fun_run);
                if (task->period() != 0) {
                    TimeTask newTimeTask = *task;
                    newTimeTask.setNextExecutionTime(task->period());
                    m_PeriodTaskQueue.reschedule_min(newTimeTask);
                } else {
                    m_PeriodTaskQueue.pop_min();
                }
            }
        }
    }

    void HandlerThread::run() {
#if PLATFORM(ANDROID)
        AndroidUtil::attachCurrentThread();
#endif
        while (!m_IsStop) {
            executePeriodTask();
            executeTask();
            {
                std::unique_lock<std::mutex> lck(mut);
                if (m_MsgQueue.empty()) {
                    if (m_PeriodTaskQueue.size() == 0) {
                        cond.wait(lck);
                    } else {
                        std::shared_ptr<TimeTask> task = m_PeriodTaskQueue.get_min();
                        auto now = std::chrono::steady_clock::now();
                        if (now < task->nextExecutionTime()) {
                            cond.wait_until(lck, task->nextExecutionTime());
                        }
                    }
                }
            }
        }

#if PLATFORM(ANDROID)
        AndroidUtil::detachCurrentThread();
#endif
        delete this;
    }

    void HandlerThread::postMsg(std::function<void(void)> msg) {
        std::unique_lock<std::mutex> lck(mut);
        m_MsgQueue.push(msg);
        cond.notify_all();
    }

    void HandlerThread::postPeriodTask(TimeTask &task) {
        std::unique_lock<std::mutex> lck(mut);
        task.m_status = TimeTask::SCHEDULED;
        m_PeriodTaskQueue.push(task);
        cond.notify_all();
    }

    void HandlerThread::cancelPeriodTask(TimeTask &task) {
        std::unique_lock<std::mutex> lck(mut);
        task.m_status = TimeTask::CANCELLED;
        m_PeriodTaskQueue.remove(task);
    }
}

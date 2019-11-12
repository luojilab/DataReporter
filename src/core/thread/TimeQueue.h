//
// Created by bingjian on 2017/10/23.
//

#ifndef ENGINE_TIMEQUEUE_H
#define ENGINE_TIMEQUEUE_H

#include <queue>
#include <functional>
#include <condition_variable>
#include <set>

namespace WTF {
    class TimeTask {

    public:
        inline TimeTask() : m_period(0), m_fun(nullptr),
                            m_next_execution_time(std::chrono::steady_clock::now()) {

        }

        inline TimeTask(unsigned long next_execution_time, unsigned long period,
                        std::function<void(void)> fun) {
            std::lock_guard<std::mutex> lk(mut);
            m_period = period;
            m_fun = fun;
            auto now = std::chrono::steady_clock::now();
            m_next_execution_time = now + std::chrono::milliseconds(next_execution_time);
        }

        inline TimeTask(const TimeTask &other) {
            this->m_fun = other.m_fun;
            this->m_period = other.m_period;
            this->m_next_execution_time = other.m_next_execution_time;
        }

        inline TimeTask &operator=(const TimeTask &other) {
            this->m_fun = other.m_fun;
            this->m_period = other.m_period;
            this->m_next_execution_time = other.m_next_execution_time;
            return *this;
        }

        inline TimeTask(TimeTask &&other) {
            this->m_fun = other.m_fun;
            other.m_fun = nullptr;
            this->m_period = other.m_period;
            other.m_period = 0;
            this->m_next_execution_time = std::move(other.m_next_execution_time);
        }

        inline TimeTask &operator=(TimeTask &&other) {
            std::swap(m_fun, other.m_fun);
            std::swap(m_period, other.m_period);
            std::swap(m_next_execution_time, other.m_next_execution_time);
            return *this;
        }

        inline bool operator==(const TimeTask &other) const {
            return this->m_next_execution_time == other.m_next_execution_time;
        }

        inline bool operator<(const TimeTask &other) const {
            return this->m_next_execution_time < other.m_next_execution_time;
        }

        inline bool operator>(const TimeTask &other) const {
            return this->m_next_execution_time > other.m_next_execution_time;
        }

        inline void setFun(std::function<void(void)> fun) {
            m_fun = fun;
        }

        inline const unsigned long period() const {
            return m_period;
        }

        inline std::function<void(void)> fun() {
            return m_fun;
        }

        inline void setNextExecutionTime(unsigned long next_execution_time) {
            std::lock_guard<std::mutex> lk(mut);
            m_next_execution_time = std::chrono::steady_clock::now() +
                                    std::chrono::milliseconds(next_execution_time);
        }

        inline std::chrono::steady_clock::time_point nextExecutionTime() {
            std::lock_guard<std::mutex> lk(mut);
            return m_next_execution_time;
        }

    protected:
        std::function<void(void)> m_fun;
        unsigned long m_period;
        std::chrono::steady_clock::time_point m_next_execution_time;
        std::mutex mut;

    };

    template<typename T>
    class TimeQueue {
    public:
        TimeQueue() {

        }

        T pop_min() {
            std::lock_guard<std::mutex> lk(mut);
            if (m_data.empty()) {
                return T();
            }

            T ret = *m_data.begin();
            m_data.erase(m_data.begin());
            return ret;
        }

        T &get_min() {
            std::lock_guard<std::mutex> lk(mut);
            if (m_data.empty()) {
                return T();
            }

            T &ret = m_data.begin()->first;;
            return ret;
        }

        void push(const T &task) {
            std::lock_guard<std::mutex> lk(mut);
            m_data.insert(task);
        }

        void remove(const T &task) {
            std::lock_guard<std::mutex> lk(mut);
            if (!m_data.empty()) {
                m_data.erase(task);
            }
        }

        void clear() {
            std::lock_guard<std::mutex> lk(mut);
            m_data.clear();
        }

        int size() {
            std::lock_guard<std::mutex> lk(mut);
            return m_data.size();
        }


    private:
        std::set<T> m_data;
        std::mutex mut;

    };
}

#endif //ENGINE_TIMEQUEUE_H

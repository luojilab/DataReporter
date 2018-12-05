//
// Created by bingjian on 2017/10/23.
//

#ifndef ENGINE_TIMEQUEUE_H
#define ENGINE_TIMEQUEUE_H

#include <queue>
#include <functional>
#include <condition_variable>
#include <map>

namespace WTF {
    class TimeTask {

    public:
        enum TASK_STATUS {
            EXECUTED,
            SCHEDULED,
            CANCELLED
        };

        inline TimeTask(unsigned long next_execution_time,unsigned long period, std::function<void(void)> fun):m_status(SCHEDULED){
            std::lock_guard <std::mutex> lk(mut);
            m_period = period;
            m_fun = fun;
            auto now = std::chrono::steady_clock::now();
            m_next_execution_time = now + std::chrono::milliseconds(next_execution_time);
        }

        inline TimeTask(const TimeTask &other){
            this->m_fun = other.m_fun;
            this->m_period = other.m_period;
            this->m_next_execution_time = other.m_next_execution_time;
            this->m_status = other.m_status;
        }

        inline TimeTask& operator=(const TimeTask &other){
            this->m_fun = other.m_fun;
            this->m_period = other.m_period;
            this->m_next_execution_time = other.m_next_execution_time;
            this->m_status = other.m_status;
            return *this;
        }

        inline bool operator==(const TimeTask &other) const{
            return this->m_next_execution_time == other.m_next_execution_time;
        }

        inline bool operator<(const TimeTask &other) const{
            return this->m_next_execution_time < other.m_next_execution_time;
        }

        inline bool operator>(const TimeTask &other) const{
            return this->m_next_execution_time > other.m_next_execution_time;
        }

        inline void setFun(std::function<void(void)> fun){
            m_fun = fun;
        }

        inline const unsigned long period()const {
            return m_period;
        }

        inline std::function<void(void)> fun(){
            return m_fun;
        }

        inline void setNextExecutionTime(unsigned long next_execution_time){
            std::lock_guard <std::mutex> lk(mut);
            m_next_execution_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(next_execution_time);
        }

        inline std::chrono::steady_clock::time_point nextExecutionTime(){
            std::lock_guard <std::mutex> lk(mut);
            return m_next_execution_time;
        }

    public:
        TASK_STATUS m_status;

    protected:
        std::function<void(void)> m_fun;
        unsigned long m_period;
        std::chrono::steady_clock::time_point m_next_execution_time;
        std::mutex mut;

    };

    template<typename T>
    class TimeQueue {
    public:
        TimeQueue(){

        }

        void pop_min(){
            std::lock_guard <std::mutex> lk(mut);
            if (m_data.empty()) {
                return;
            }
            m_data.erase(m_data.begin());
        }

        std::shared_ptr <T> get_min(){
            std::shared_ptr <T> ret;
            std::lock_guard <std::mutex> lk(mut);
            if (m_data.empty()) {
                return ret;
            }
            ret = std::make_shared<T>(std::move(m_data.begin()->first));
            return ret;
        }

        void push(const T &task){
            std::lock_guard <std::mutex> lk(mut);
            m_data[task] = 0;
        }

        void remove(const T &task){
            std::lock_guard <std::mutex> lk(mut);
            if (!m_data.empty()) {
                m_data.erase(task);
            }
        }

        void reschedule_min(T &newTask){
            std::lock_guard <std::mutex> lk(mut);
            if (m_data.empty()) {
                return;
            }
            T min = m_data.begin()->first;
            m_data.erase(min);
            m_data[newTask] = 0;
        }

        void reschedule_task(const T &oldTask,const T &newTask){
            std::lock_guard <std::mutex> lk(mut);
            if (m_data.empty()) {
                return;
            }

            if(m_data.find(oldTask) != m_data.end()){
                m_data.erase(oldTask);
                m_data[newTask] = 0;
            }
        }

        void clear(){
            std::lock_guard <std::mutex> lk(mut);
            m_data.clear();
        }

        int size(){
            std::lock_guard <std::mutex> lk(mut);
            return m_data.size();
        }


    private:
        std::map<T,int> m_data;
        std::mutex mut;

    };
}

#endif //ENGINE_TIMEQUEUE_H

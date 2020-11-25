#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

using namespace std;

//线程池最大容量,应尽量设小一点
#define THREADPOOL_MAX_NUM 16
//#define  THREADPOOL_AUTO_GROW

//线程池,可以提交变参函数或拉姆达表达式的匿名函数执行，可以获取执行返回值
//不直接支持类成员函数，支持类静态成员函数或全局函数，operator()函数等
class ThreadPool {
    using Task = function<void()>;  //定义类型
    vector<thread> pool_;           //线程池
    queue<Task> tasks_;             //任务队列
    mutex mtx_;                     //同步
    condition_variable cv_;         //条件阻塞
    atomic<bool> stop_;             //线程池是否停止提交
    atomic<int> idleThreadNum_;     //空闲线程数量

 public:
    inline ThreadPool(size_t size = 4) : stop_(false), idleThreadNum_(0) {
        addThread(size);
    }

    inline ~ThreadPool() {
        stop_ = true;
        cv_.notify_all();  // 唤醒所有线程执行
        for (auto& t : pool_) {
            if (t.joinable()) {
                t.join();  // 等待任务结束， 前提：线程一定会执行完
            }
        }
    }

 public:
    // 提交一个任务
    // 调用get()会等待任务执行完，获取返回值
    // 有两种方法可以实现调用类成员，
    // 一种是使用   bind： .submit(std::bind(&Dog::sayHello, &dog));
    // 一种是用   mem_fn： .submit(std::mem_fn(&Dog::sayHello), this)
    template <typename F, typename... Args>
    auto submit(F&& f, Args&&... args) -> future<decltype(f(args...))> {
        if (stop_) {
            throw runtime_error("submit on ThreadPool is stopped.");
        }

        using RetType = decltype(f(args...));  // typename std::result_of<F(Args...)>::type,
                                               // 函数 f 的返回值类型
        auto task = make_shared<packaged_task<RetType()>>(
            bind(forward<F>(f), forward<Args>(args)...));  // 把函数入口及参数,打包(绑定)
        future<RetType> future = task->get_future();
        {  // 添加任务到队列
            lock_guard<mutex> lock{mtx_};
            tasks_.emplace([task]() { (*task)(); });
        }
#ifdef THREADPOOL_AUTO_GROW
        if (idleThreadNum_ < 1 && pool_.size() < THREADPOOL_MAX_NUM) addThread(1);
#endif
        cv_.notify_one();  // 唤醒一个线程执行

        return future;
    }

    //空闲线程数量
    int getIdleCount() {
        return idleThreadNum_;
    }
    //线程数量
    int getThreadCount() {
        return pool_.size();
    }

 private:
    void addThread(size_t size) {
        //增加线程数量，但不超过预定义数量 THREADPOOL_MAX_NUM
        while (pool_.size() < THREADPOOL_MAX_NUM && size > 0) {
            pool_.emplace_back([this] {  //工作线程函数
                while (!stop_) {
                    Task task;  // 获取一个待执行的 task
                    {
                        // unique_lock 相比 lock_guard 的好处是：可以随时 unlock() 和 lock()
                        unique_lock<mutex> lock(mtx_);
                        cv_.wait(lock, [this] { return stop_ || !tasks_.empty(); });  // wait 直到有 task
                        if (stop_ && tasks_.empty()) return;
                        task = move(tasks_.front());  // 按先进先出从队列取一个 task
                        tasks_.pop();
                    }
                    idleThreadNum_--;
                    task();  //执行任务
                    idleThreadNum_++;
                }
            });
            idleThreadNum_++;
            size--;
        }
    }
};

#endif

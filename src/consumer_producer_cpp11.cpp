#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

using namespace std;

class Task {
 public:
  Task(int taskID) { this->taskID = taskID; }

  void doTask() {
    cout << "handle a task, taskID: " << taskID
         << ", threadID: " << this_thread::get_id() << endl;
  }

 private:
  int taskID;
};

mutex mtx;
queue<Task*> tasks;
condition_variable cv;

void consumer_thread() {
  Task* pTask = NULL;
  while (true) {
    unique_lock<mutex> lock(mtx);
    while (tasks.empty()) {
      //如果获得了互斥锁，但是条件不合适的话，wait会释放锁，不往下执行
      //当发生变化后，条件合适，wait将直接获得锁
      cv.wait(lock);
    }

    pTask = tasks.front();
    tasks.pop();

    if (pTask == NULL) continue;

    pTask->doTask();
    delete pTask;
    pTask = NULL;
  }
}

void producer_thread() {
  int taskID = 0;
  Task* pTask = NULL;

  while (true) {
    pTask = new Task(taskID);

    //使用括号减小lock锁的作用范围
    {
      lock_guard<mutex> lock(mtx);
      tasks.push(pTask);
      cout << "produce a task, taskID: " << taskID
           << ", threadID: " << this_thread::get_id() << endl;
    }

    //释放互斥锁，通知消费者线程
    cv.notify_one();
    taskID++;
    this_thread::sleep_for(chrono::seconds(1));
  }
}

int main() {
  //创建5个消费者线程
  thread consumer1(consumer_thread);
  thread consumer2(consumer_thread);
  thread consumer3(consumer_thread);
  thread consumer4(consumer_thread);
  thread consumer5(consumer_thread);

  //创建一个生产者线程
  thread producer(producer_thread);

  producer.join();
  consumer1.join();
  consumer2.join();
  consumer3.join();
  consumer4.join();
  consumer5.join();

  return 0;
}
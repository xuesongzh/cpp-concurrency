
#ifndef _DATA_POOL_H_
#define _DATA_POOL_H_

#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

using namespace std::chrono;
using std::condition_variable;
using std::cout;
using std::endl;
using std::lock_guard;
using std::mutex;
using std::queue;
using std::unique_lock;
using std::vector;

template <typename T>
class DataPool {
 public:
  DataPool(int count, char *name);
  ~DataPool();

  int putData(T *data);
  T *getData();
  void getAllData(vector<T *> &v);

  int getPoolSize();
  void clearPool();

 private:
  void freeData();

 private:
  queue<T *> dataQueue;

  // 池子大小
  int count;
  // 池子名字
  char *name;

  time_point<steady_clock> t1;
  time_point<steady_clock> t2;

  mutex mtx;
  condition_variable cv;
};

template <typename T>
DataPool<T>::DataPool(int count, char *name) {
  this->count = count;
  this->name = name;
}

template <typename T>
DataPool<T>::~DataPool() {
  clearPool();
}

template <typename T>
int DataPool<T>::putData(T *data) {
  {
    lock_guard<mutex> lock(mtx);
    if (dataQueue.size() >= count) {
      freeData();
    }
    dataQueue.push(data);
    cout << "Add one data into the " << this->name << " Queue, Queue size is "
         << dataQueue.size() << endl;
    t1 = steady_clock::now();
  }
  cv.notify_one();
  return 0;
}

template <typename T>
T *DataPool<T>::getData() {
  T *data = NULL;

  unique_lock<mutex> lock(mtx);
  while (dataQueue.empty()) {
    cv.wait(lock);
  }
  data = dataQueue.front();
  dataQueue.pop();
  t2 = steady_clock::now();
  auto time_diff = t2 - t1;
  auto duration = duration_cast<microseconds>(time_diff);
  cout << this->name << " time consuming :" << duration.count() << "us" << endl;
  cout << "Get one data from the " << this->name << " Queue, Queue size is "
       << dataQueue.size() << endl;
  return data;
}

template <typename T>
void DataPool<T>::freeData() {
  T *data = dataQueue.front();
  dataQueue.pop();
  delete data;
}

template <>
inline void DataPool<MessageFrame_t>::freeData() {
  MessageFrame_t *msgFrame = dataQueue.front();
  dataQueue.pop();
  ASN_STRUCT_FREE(asn_DEF_MessageFrame, msgFrame);
}

template <typename T>
void DataPool<T>::getAllData(vector<T *> &v) {
  lock_guard<mutex> lock(mtx);
  while (!dataQueue.empty()) {
    v.push_back(dataQueue.front());
    dataQueue.pop();
  }
}

template <typename T>
int DataPool<T>::getPoolSize() {
  lock_guard<mutex> lock(mtx);
  return dataQueue.size();
}

template <typename T>
void DataPool<T>::clearPool() {
  lock_guard<mutex> lock(mtx);
  while (!dataQueue.empty()) {
    freeData();
  }
}

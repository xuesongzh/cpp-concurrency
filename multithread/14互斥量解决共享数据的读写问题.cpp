#include <iostream>
#include <list>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
using namespace std;

//用成员函数作为线程初始化函数

class A {
 public:
  //把收到的消息入到一个队列，子线程的启动函数
  void inMsgRecvQueue() {
    for (int i = 0; i < 10000; i++) {
      cout << "inMsgQueue插入一个元素" << i << endl;

      mtx.lock();
      msgRecvQueue.push_back(i);  //假设这个数字i就是收到的玩家的命令
      mtx.unlock();
    }
  }

  ////读共享数据函数的封装函数
  // bool outMsgprocess(int&command)
  //{
  //	mtx.lock();
  //	if (!msgRecvQueue.empty())
  //	{
  //		//消息队列不为空
  //		int command = msgRecvQueue.front();//返回第一个元素
  //		msgRecvQueue.pop_front();//移除第一个元素
  //		mtx.unlock();
  //		return true;
  //	}
  //	mtx.unlock();
  //	return false;
  //	//所有分支都要有unlock()，两个出口必须有两个unlock()
  //
  //}

  //读共享数据函数的封装函数，使用lock_guard()
  bool outMsgprocess(int& command) {
    lock_guard<mutex> myguard(mtx);  // myguard是我们起的名字
    // lock_guard的构造函数里面执行了lock()函数，当myguard局部变量析构的时候，执行unlock()函数。
    if (!msgRecvQueue.empty()) {
      //消息队列不为空
      int command = msgRecvQueue.front();  //返回第一个元素
      msgRecvQueue.pop_front();            //移除第一个元素
      return true;
    }
    return false;
  }

  //把数据从消息队列中取出的子线程
  void outMsgRecvQueue() {
    int command = 0;
    for (int i = 0; i < 10000; i++) {
      bool result = outMsgprocess(command);
      if (result) {
        cout << "取消息函数执行成功" << command << endl;
      } else {
        cout << "消息队列中的消息为空" << i << endl;
      }
    }

    cout << endl;
  }

 private:
  list<int> msgRecvQueue;  //容器用来存放玩家发送过来的命令
  //创建一个互斥量的成员变量
  mutex mtx;
};

int main(void) {
  A myobj;
  //第二个是引用才能保证线程中用的是同一个对象
  thread myOutMsgObj(&A::outMsgRecvQueue, &myobj);
  thread myInMsObj(&A::inMsgRecvQueue, &myobj);
  myOutMsgObj.join();
  myInMsObj.join();

  cout << "main线程" << endl;  //最后执行这一句，整个线程退出
  system("pause");
  return 0;
}

/*
 *保护共享数据：操作时，用代码把共享数据锁住，其他想操作共享数据的线程等待解锁，然后锁住，解锁
 *
 *互斥量(mutex)的基本概念--其实就是一个类对象，理解成一把锁，多个线程尝试用lock()成员函数来加锁
 *只有一个线程可以解锁成功，成功的标志是返回了，如果没有成功，那么线程就会阻塞这里等待锁住，执行完毕
 *用unlock()解锁。
 *保护过多影响效率。保护较小没有作用
 * (1)包含头文件#include<mutex>
 * (2)先lock()，再unlock()要成对使用，不能多也不能少。非成对调用会导致代码不稳定
 * (3)有几个出口就要有几个unlock()，这种错误非常难以排查。
 *    引入lock_guard()，自动帮助我们解锁，类似于智能指针，会自动释放。
 *（4）用了lock_guard()可以取代lock()、unlock()，只需要使用lock_guard()即可。
 *    可以使用{}来约束lock_guard()函数的生命周期。
 *
 */

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

            // mtx1.lock();//先锁一个锁，再锁一个锁
            ////两个锁不一定挨着，可能保存不同的代码
            // mtx2.lock();
            //也可以使用lock_guard()
            // lock_guard<mutex> mylock1(mtx1);

            lock(mtx2, mtx1);           //和顺序无关
            msgRecvQueue.push_back(i);  //假设这个数字i就是收到的玩家的命令
            mtx1.unlock();
            mtx2.unlock();
        }
    }

    //读共享数据函数的封装函数
    bool outMsgprocess(int& command) {
        /*mtx2.lock();
        mtx1.lock();*/
        lock(mtx1, mtx2);
        //相当于每一个互斥量都调用了lock()
        if (!msgRecvQueue.empty()) {
            //消息队列不为空
            int command = msgRecvQueue.front();  //返回第一个元素
            msgRecvQueue.pop_front();            //移除第一个元素
            mtx1.unlock();
            mtx2.unlock();
            return true;
        }
        mtx1.unlock();
        mtx2.unlock();
        return false;
        //所有分支都要有unlock()，两个出口必须有两个unlock()
    }

    //把数据从消息队列中取出的子线程
    void outMsgRecvQueue() {
        int command = 0;
        for (int i = 0; i < 10000; i++) {
            bool result = outMsgprocess(command);
            if (result == true) {
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
    mutex mtx1;
    mutex mtx2;
};

int main(void) {
    A myobj;

    thread myOutMsgObj(&A::outMsgRecvQueue, &myobj);
    thread myInMsObj(&A::inMsgRecvQueue, &myobj);
    myOutMsgObj.join();
    myInMsObj.join();

    cout << "main线程" << endl;  //最后执行这一句，整个线程退出
    system("pause");
    return 0;
}

/*
 *死锁的解决方案：
 *std::lock()函数模板，
 *能力：一次锁住两个或者两个以上的互斥量，不存在在多个线程中，因为锁的顺序问题导致死锁的风险问题。
 *如果互斥量中有一个没锁住，它就在那里等着，等到所有互斥量都上锁，才会继续执行。
 *如果不能全部上锁，就把已经锁住的东西释放掉，也就是说只有全部锁着才上锁。
 */

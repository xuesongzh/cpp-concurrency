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

            mtx1.lock();  //先锁一个锁，再锁一个锁
            //两个锁不一定挨着，可能保存不同的代码
            mtx2.lock();
            //也可以使用lock_guard()
            // lock_guard<mutex> mylock1(mtx1);

            msgRecvQueue.push_back(i);  //假设这个数字i就是收到的玩家的命令
            mtx1.unlock();
            mtx2.unlock();
        }
    }

    //读共享数据函数的封装函数
    bool outMsgprocess(int& command) {
        mtx2.lock();
        mtx1.lock();
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
 *比如我们有两把锁，也就是两个互斥量。当有两个线程A B，使用两把锁都上锁，
 *线程A执行的时候lock a成功，然后再去锁另外一个锁b，
 *出现了上下文切换，线程B，先锁b锁，然后锁a锁,但是a锁已经被使用，
 *线程A和线程B互相等待对方解锁。(上锁的顺序相反)。
 *当两个线程上锁的顺序相同就不会出现问题。
 *
 */

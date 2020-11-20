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
    //通过函数返回一个unique_lock对象，实现所有权转移
    unique_lock<mutex> rtn_unique_lock() {
        unique_lock<mutex> tempLock(mtx);
        return tempLock;  //从函数返回一个局部的unique_lock是可以的
                          //移动构造函数，系统临时生成一个临时对象。
    }

    //把收到的消息入到一个队列，子线程的启动函数
    void inMsgRecvQueue() {
        for (int i = 0; i < 10000; i++) {
            cout << "inMsgQueue插入一个元素" << i << endl;

            // adopt_lock:表示互斥量已经被lock了，互斥量必须在前面lock了，否则会报异常。
            //调用方线程已经拥有了互斥量的所有权，不需要在构造函数中再次lock了。
            mtx.lock();
            unique_lock<mutex> lock1(mtx, adopt_lock);

            // try_to_lock:是我们会尝试用mutex的lock()去锁定mutex，但是如果没有锁定成功，
            //会立即返回，不会阻塞在那里。不能提前lock。
            unique_lock<mutex> lock2(mtx, try_to_lock);
            //如果拿到了锁
            if (lock2.owns_lock()) {
                msgRecvQueue.push_back(i);  //假设这个数字i就是收到的玩家的命令
            } else {
                cout << "没有拿到锁----------------------" << endl;
            }

            // default_lock:不能先lock住，就是没有给mutex加锁，初始化了一个没有加锁的mutex。
            //可以灵活的调用一些unique_lock的成员函数。
            unique_lock<mutex> lock3(mtx, defer_lock);
            //没有加锁的mtx，加锁之后不用自己解锁
            lock3.lock();
            if (lock3.try_lock()) {  //尝试加锁，不成功也不阻塞
                msgRecvQueue.push_back(i);
            } else {
                cout << "没有拿到锁----------------------" << endl;
            }

            // release解除unique_lock和mutex之间的关联
            unique_lock<mutex> myGuard(mtx);
            mutex* ptx = myGuard.release();
            msgRecvQueue.push_back(i);
            ptx->unlock();

            //所有权转移
            unique_lock<mutex> myGuard1(mtx);
            //复制所有权是非法的
            // unique_lock<mutex> myGuard2(myGuard1);
            //移动语义，相当于myGuard2和mtx绑定到一起，myGuard1指向空，myGuard2指向了mtx
            unique_lock<mutex> myGuard2(move(myGuard1));

            mutex* ptx = myGuard2.release();
            msgRecvQueue.push_back(i);
            ptx->unlock();

            // unique_lock<mutex> myGuard1 = rtn_unique_lock();
        }
    }
    //读共享数据函数的封装函数
    bool outMsgprocess(int& command) {
        // lock_guard<mutex> myGuard1(mtx);
        unique_lock<mutex> myGuard1(mtx);

        //睡眠20s
        /*chrono::milliseconds dura(20000);
        this_thread::sleep_for(dura);*/

        if (!msgRecvQueue.empty()) {
            //消息队列不为空
            command = msgRecvQueue.front();  //返回第一个元素
            msgRecvQueue.pop_front();        //移除第一个元素
            return true;
        }

        return false;
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
    mutex mtx;
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
 *一：unique_lock()取代lock_guard()
 *unique_lock是一个类模板，工作中一般推荐使用lock_guard()，其取代了lock(),unlock()函数
 *unique_lock比lock_guard灵活很多，效率上差一点，内存占用多一点。
 *
 *二：参数
 *默认不带参数lock_guard和unique_lock相同
 *
 *unique_lock支持更多的参数
 *
 *三：成员函数
 * lock()
 * unlock()--会自动解锁（可能在中间处理一些非共享代码，然后再锁上）
 * try_lock()--尝试加锁，不成功也不阻塞
 * release()--返回它锁管理的mutex对象指针，并释放管理权
 *	也就是unique_lock和mutex不再有关系,如果原来mutex处于加锁状态，就需要自己接管解锁。
 *	不要混淆release()和unlock()
 *
 *为什么有时候需要unlock()，因为你lock锁住的代码越少，整个程序越快
 *有时候也把锁住的代码多少称为锁的粒度，粒度一般用粗细描述，锁住的代码越少，执行效率越高。
 *粒度越粗，执行的效率越低。所以选择合适的粒度是程序员能力和实力的体现
 *
 *
 *四：unique_lock()所有权的传递
 *通常情况下unique_lock应该和一个mutex相互关联。
 *unique_lock<mutex> myGuard(mtx);
 *myGuard拥有mtx的所有权，并且所有权可以转移给其他unique_lock对象，但是所有权不能复制
 *
 * 1)std::move
 * 2)return std::unique_lock<std::mutex>
 */

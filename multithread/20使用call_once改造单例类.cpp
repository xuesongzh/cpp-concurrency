#include <allocators>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
using namespace std;

//互斥量
std::mutex resource_mutex;
std::once_flag gflag;  //这是系统标记们还没有被置位

//单例类
class MyCAS {
 private:
    //构造函数私有化
    MyCAS() {}
    static void createInstance() {
        m_instance = new MyCAS();
        static ReleaseObject ro;
    }

 private:
    static MyCAS* m_instance;  //静态成员变量指针

 public:
    static MyCAS* getInstance() {
        //两个线程同时执行到这里，其中一个线程等待另外一个线程执行完毕createInstance
        std::call_once(gflag, createInstance);
        return m_instance;
    }

    //类中套一个类实现内存释放（好像没起作用）
    class ReleaseObject {
     public:
        ~ReleaseObject() {
            if (MyCAS::m_instance) {
                cout << "销毁对象" << endl;
                delete MyCAS::m_instance;
                MyCAS::m_instance = NULL;
            }
        }
    };

    void function1() {
        cout << "测试函数" << endl;
    }
};

//需要初始化静态成员变量
MyCAS* MyCAS::m_instance = NULL;

//线程入口函数
void startThread() {
    cout << "线程开始执行了" << endl;
    MyCAS* p_a = MyCAS::getInstance();
    cout << "线程执行完毕了" << endl;
    return;
}

int main(void) {
    ////返回该类对象的指针
    // MyCAS*p_a = MyCAS::getInstance();
    // p_a->function1();

    //两个线程是同一个入口函数，所以这里会有两个流程或者两条通路同时执行startThread函数。
    //当其中一个线程进入getInstance，但是还没有创建对象，切换到第二个线程，这样就可能创建多个对象
    // m_instance == NULL不代表没被创建对象，可能马上就会创建，但是线程切换了
    thread mythread1(startThread);
    thread mythread2(startThread);
    mythread1.join();
    mythread2.join();

    system("pause");
    return 0;
}
/*
 * std::call_once是一个函数模板，是c++11引入的函数
 * 第二个参数是一个函数名字，call_once的功能是保证函数只会被调用一次
 * call_once具有互斥量这种能力，而且效率上，这比互斥来给你占用的资源更少
 * call_once需要和一个标记结合使用，这个标记once_flag，其实是once_flag一种结构
 * call_once就是通过这个标记判断函数是否执行，执行过函数后就把这个参数设置为已经调用状态
 * 后续再调用但是标志已经被设置为“已经调用”状态了，就不会再执行这个函数
 */

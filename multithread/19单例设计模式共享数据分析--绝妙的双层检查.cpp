#include <allocators>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
using namespace std;

//互斥量
std::mutex resource_mutex;

//单例类
class MyCAS {
 private:
  //构造函数私有化
  MyCAS() {}

 private:
  static MyCAS* m_instance;  //静态成员变量指针

 public:
  static MyCAS* getInstance() {
    if (m_instance == NULL) {  //双重检查DCL
      std::unique_lock<std::mutex> MyMutex(resource_mutex);
      //这种写法效率低，因为我们只有在第一次调用的时候才会创建，其他并不会创建，所以两次判断可以解决
      if (m_instance == NULL) {
        m_instance = new MyCAS();
        static ReleaseObject ro;
      }
    }

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

  void function() { cout << "测试函数" << endl; }
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
  //返回该类对象的指针
  MyCAS* p_a = MyCAS::getInstance();
  p_a->function();

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
 * 设计模式大概谈
 * “设计模式”--代码的一些特定的写法，程序灵活，程序维护起来很方便，但是别人阅读起来很痛苦。
 * 用设计模式写出的代码很晦涩难懂，这是别人应付特别大的项目的时候，根据实际开发需求，模块划分经验，总结成设计模式。
 * 但是到中国来，不太一样，拿着一个项目硬套设计模式，一个小小项目，非要套几个设计模式。
 * 设计模式有其独特的优点，要活学活用，不要生搬硬套。
 *
 * 单例设计模式：使用的频率比较高。
 * 在整个项目中，有某个特殊的类。且属于该类的对象只能创建一个。（类的实例只能有一个）
 * 如在：配置文件读写的时候为方便管理，就只有一个类对象，方便管理。
 *
 * 内存释放好像没有起作用
 *
 * //单例设计模式共享数据问题分析解决
 * 最理想的是主线程创建单例对象，但是实际中可能有多个线程同时调用getInstance()函数来创建单例对象，
 * 这个时候就需要使用互斥
 *
 */

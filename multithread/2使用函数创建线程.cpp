#include <iostream>
#include <string>
#include <thread>
using namespace std;

//线程的初始函数--从这个函数开始执行
void MyPrint() {
  cout << "子线程开始执行1" << endl;
  cout << "子线程执行完毕2" << endl;
  cout << "子线程执行完毕3" << endl;
  cout << "子线程执行完毕4" << endl;
  cout << "子线程执行完毕5" << endl;
  cout << "子线程执行完毕6" << endl;
}

int main(void) {
  //实际上是主线程在执行(自动创建的主线程)，主线程main()函数返回，则整个进程执行完毕

  //（1）创建了线程，线程的起点（入口）是MyPrint
  //（2）MyPrint标志的线程已经开始执行，主线程和MyPrint（）并行执行
  thread my_thread(MyPrint);

  //让主线程阻塞等待子线程执行完毕，然后主线程和子线程会合，然后主线程再继续执行
  //去掉这一行主线程和子线程交替执行，会输出混乱，当主线程先结束的时候，子线程可能未执行完毕，会被操作系统杀死
  //一个书写良好的程序，应该是主线程等待子线程执行完毕后，自己才能结束
  my_thread.join();

  if (my_thread.joinable()) {
    cout << "can join" << endl;
  } else {
    cout << "can not join" << endl;
  }

  /*
   * 主线程是不是可以不等待子线程执行完毕，自己先结束，可以detache()
   * 传统的多线程程序主线程要等待子线程执行完毕，然后自己退出，使用detach可以让主线程和子线程分离执行。
   * detach()会导致线程失去我们自己的控制。一旦调用detach()线程就被操作系统接管，就不能再使用join()，否则会有异常
   *
   * 为什么引入detach()
   *（1）当我们创建了很多子线程，让主线程一个一个等待子线程结束，这种编程方法不太好，所以引入detach()
   *（2）一旦detach()之后，与这个主线程关联的thread对象就会失去与主线程的关联。
   *
   * 这个子程序就会驻留在后台运行，被操作系统接管，当子线程运行完成后，由运行时库（操作系统）清理相关的资源。
   * 在linux下就是守护线程，主线程退出会导致子线程的输出转入到后台进行。
   *
   */
  my_thread.detach();

  // joinable()用于判断是否可以使用join或者detach  返回true或者false
  if (my_thread.joinable()) {
    cout << "2can join" << endl;
  } else {
    cout << "2can not join" << endl;
  }

  cout << "这是主线程在执行\n" << endl;

  system("pause");
  return 0;
}

/*
 * 总结
 * （1）必须子线程执行完毕，主线程再执行完，也就说主线程必须阻塞等待子线程执行完毕，然后再结束
 */
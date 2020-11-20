#include <future>
#include <iostream>
#include <string>
#include <thread>
using namespace std;

class A {
 public:
    int mythread(int mypar) {
        cout << "mpar=" << mypar << endl;
        cout << "线程开始执行,线程的id是：" << std::this_thread::get_id() << endl;
        //休息5s
        chrono::milliseconds dura(5000);
        this_thread::sleep_for(dura);

        return 5;
    }
};

int main(void) {
    A a;
    int tempar = 12;
    cout << "main thread id:" << std::this_thread::get_id() << endl;
    // std::future<int> result = std::async(mythread);

    // 第二个对象使用对象引用，才能保证使用的是同一个对象
    std::future<int> result = std::async(&A::mythread, &a, tempar);

    //我们通过向async()函数传递一个参数，该参数类型是std::launcher类型(枚举)，来达到一些特殊的目的。

    // std::launch::deferred--表示线程入口函数调用被延迟到std::future的get()或着wait()调用才执行
    //那么如果get或者wait没有调用，线程函数不会执行，实际上线程根本没有创建。
    //延迟调用，并且没有创建新线程，在主线程中执行了线程入口函数。

    // std::launch::async --异步，在调用async函数时候就创建了新线程
    //如果不写第一个标记，默认就是这个std::launch::async

    // std::launch::any

    std::future<int> result = std::async(std::launch::deferred, &A::mythread, &a, tempar);

    cout << "continue........" << endl;
    int def = 0;
    //卡在这里等待线程执行完毕，通过future对象的get()方法等待程序结束并得到返回结果
    cout << "未来的执行结果是：" << result.get() << endl;
    system("pause");
    return 0;
}

#include <iostream>
#include <memory>
#include <string>
#include <thread>

using namespace std;

void MyPrint2(unique_ptr<int> pn)

{
  cout << "thread is:" << this_thread::get_id() << endl;
}

int main(void) {
  unique_ptr<int> myp(new int(100));
  //智能指针转移使用move
  thread mythread(MyPrint2, std::move(myp));
  mythread.join();
  // mythread.detach();

  system("pause");
  return 0;
}

/*
 *没有编译通过,在gcc编译器中正确
 *复习智能指针，move()*/
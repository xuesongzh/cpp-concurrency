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
  //����ָ��ת��ʹ��move
  thread mythread(MyPrint2, std::move(myp));
  mythread.join();
  // mythread.detach();

  system("pause");
  return 0;
}

/*
 *û�б���ͨ��,��gcc����������ȷ
 *��ϰ����ָ�룬move()*/

#include <future>
#include <iostream>
#include <string>
#include <thread>

using namespace std;

void mythread(std::promise<int>& tem, int calc) {
  calc++;
  calc *= 10;
  chrono::milliseconds dura(5000);
  std::this_thread::sleep_for(dura);
  //��������������5����

  int result = calc;
  //�ѽ�����ݳ���
  tem.set_value(result);
}

void mythread2(std::future<int>& tem) {
  auto result = tem.get();
  cout << "mythread2 result=" << result << endl;
  return;
}

int main(void) {
  std::promise<int> mypromise;  //����һ��promise���󣬱�������Ϊint
  thread t1(mythread, std::ref(mypromise), 120);
  t1.join();

  //�󶨣���ȡ���
  std::future<int> future1 = mypromise.get_future();
  auto result = future1.get();  // get()ֻ�ܵ���һ�Σ����ܵ��ö��
  cout << result << endl;

  //�������߳���ʹ��
  thread t2(mythread2, std::ref(future1));
  t2.join();

  system("pause");
  return 0;
}

/*
 * std::promiseҲ��һ����ģ��
 * ������ĳһ���߳��и�ֵ��Ȼ���������߳��а����ֵ��ȡ������
 * ͨ��promise����һ��ֵ���ڽ���ĳ��ʱ������ͨ����һ��future����󶨵����promise���õ��󶨵�ֵ
 */
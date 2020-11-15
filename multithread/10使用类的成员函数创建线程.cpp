#include <iostream>
#include <string>
#include <thread>

using namespace std;

class A {
 public:
  mutable int m_i;  // m_i��ֵ�ǿ����޸ĵģ���ʹ��const��
  //���캯��--����ת�����캯������һ������ת��Ϊһ����A����
  A(int a) : m_i(a) {
    cout << "A�Ĺ��캯��ִ��" << endl;
    cout << this << "thread id" << this_thread::get_id() << endl;
  }
  //�������캯��
  A(const A& m) : m_i(m.m_i) {
    cout << "A�Ŀ������캯��ִ��" << endl;
    cout << this << "thread id" << this_thread::get_id() << endl;
  }
  ~A() {
    cout << "A����������ִ��" << std::endl;
    cout << this << "thread id" << this_thread::get_id() << endl;
  }

  void thread_work(int num) { cout << "���߳��еĳ�Ա����" << endl; }

  void operator()(int num) { cout << "ʹ������󴴽��̣߳���" << endl; }
};

int main(void) {
  A obj(1100);
  //���ݵ��̵߳ĺ�����Ҫȡ��ַ�����Ҽ����������򣬶�����ʽ������ֵ
  //����ÿ������캯��
  // thread mythread(&A::thread_work, obj, 14);

  //�����Ĵ���һ�����ã��򿽱����캯������ִ��
  thread mythread(&A::thread_work, ref(obj), 14);

  // thread mythread(obj, 14);
  // thread mythread(ref(obj), 14);

  mythread.join();
  system("pause");
  return 0;
}

//ʹ����ĳ�Ա����ִ��һ���߳�

#include <future>
#include <iostream>
#include <string>
#include <thread>

using namespace std;

//��װ��һ������Ϊԭ�����͵�int���ҿ����������ͨ����һ��
std::atomic<int> g_mycout = 0;
void mythread() {
  for (int i = 0; i < 1000000; i++) {
    // g_mycout++;//�Ѿ���һ��atomic�����ˣ��������ᱻ���
    // g_mycout += 1;  //����
    g_mycout = g_mycout + 1;  //���ֲ��������ԣ�����������в��������ǿ��Ե�
  }
}

int main(void) {
  thread t1(mythread);
  thread t2(mythread);

  t1.join();
  t2.join();
  cout << "g_mycout=" << g_mycout << endl;
  system("pause");
  return 0;
}

/*
 *g_mycout = g_mycout + 1;//���ֲ��������ԣ�����������в��������ǿ��Ե�
 *һ��atomicԭ�Ӳ��������++��--��+=��&=��|=��^=��֧�ֵģ������Ŀ��ܲ�֧�֡�
 */
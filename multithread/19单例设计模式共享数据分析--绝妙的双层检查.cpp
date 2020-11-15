#include <allocators>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
using namespace std;

//������
std::mutex resource_mutex;

//������
class MyCAS {
 private:
  //���캯��˽�л�
  MyCAS() {}

 private:
  static MyCAS* m_instance;  //��̬��Ա����ָ��

 public:
  static MyCAS* getInstance() {
    if (m_instance == NULL) {  //˫�ؼ��DCL
      std::unique_lock<std::mutex> MyMutex(resource_mutex);
      //����д��Ч�ʵͣ���Ϊ����ֻ���ڵ�һ�ε��õ�ʱ��Żᴴ�������������ᴴ�������������жϿ��Խ��
      if (m_instance == NULL) {
        m_instance = new MyCAS();
        static ReleaseObject ro;
      }
    }

    return m_instance;
  }
  //������һ����ʵ���ڴ��ͷţ�����û�����ã�
  class ReleaseObject {
   public:
    ~ReleaseObject() {
      if (MyCAS::m_instance) {
        cout << "���ٶ���" << endl;
        delete MyCAS::m_instance;
        MyCAS::m_instance = NULL;
      }
    }
  };

  void function() { cout << "���Ժ���" << endl; }
};
//��Ҫ��ʼ����̬��Ա����
MyCAS* MyCAS::m_instance = NULL;

//�߳���ں���
void startThread() {
  cout << "�߳̿�ʼִ����" << endl;
  MyCAS* p_a = MyCAS::getInstance();
  cout << "�߳�ִ�������" << endl;
  return;
}

int main(void) {
  //���ظ�������ָ��
  MyCAS* p_a = MyCAS::getInstance();
  p_a->function();

  //�����߳���ͬһ����ں�����������������������̻�������ͨ·ͬʱִ��startThread������
  //������һ���߳̽���getInstance�����ǻ�û�д��������л����ڶ����̣߳������Ϳ��ܴ����������
  // m_instance == NULL������û���������󣬿������Ͼͻᴴ���������߳��л���
  thread mythread1(startThread);
  thread mythread2(startThread);
  mythread1.join();
  mythread2.join();

  system("pause");
  return 0;
}
/*
 * ���ģʽ���̸
 * �����ģʽ��--�����һЩ�ض���д��������������ά�������ܷ��㣬���Ǳ����Ķ�������ʹ�ࡣ
 * �����ģʽд���Ĵ���ܻ�ɬ�Ѷ������Ǳ���Ӧ���ر�����Ŀ��ʱ�򣬸���ʵ�ʿ�������ģ�黮�־��飬�ܽ�����ģʽ��
 * ���ǵ��й�������̫һ��������һ����ĿӲ�����ģʽ��һ��СС��Ŀ����Ҫ�׼������ģʽ��
 * ���ģʽ������ص��ŵ㣬Ҫ��ѧ���ã���Ҫ����Ӳ�ס�
 *
 * �������ģʽ��ʹ�õ�Ƶ�ʱȽϸߡ�
 * ��������Ŀ�У���ĳ��������ࡣ�����ڸ���Ķ���ֻ�ܴ���һ���������ʵ��ֻ����һ����
 * ���ڣ������ļ���д��ʱ��Ϊ���������ֻ��һ������󣬷������
 *
 * �ڴ��ͷź���û��������
 *
 * //�������ģʽ������������������
 * ������������̴߳����������󣬵���ʵ���п����ж���߳�ͬʱ����getInstance()������������������
 * ���ʱ�����Ҫʹ�û���
 *
 */

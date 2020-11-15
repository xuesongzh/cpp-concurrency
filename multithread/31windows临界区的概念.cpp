#include <windows.h>

#include <iostream>
#include <list>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
using namespace std;

//�ó�Ա������Ϊ�̳߳�ʼ������
#define __WINDOWS

//���������Զ��ͷ�windows�µ��ٽ�ȥ��������lock_guard()
class CwinLock  //�����౻��ΪRAII�ࣨresource acquisition is initialation��
{
 public:
  CwinLock(CRITICAL_SECTION *pCritem)  //���캯��
  {
    m_pCritical = pCritem;
    EnterCriticalSection(m_pCritical);
  }
  ~CwinLock()  //��������
  {
    LeaveCriticalSection(m_pCritical);
  }

 private:
  CRITICAL_SECTION *m_pCritical;
};

class A {
 public:
  //���յ�����Ϣ�뵽һ�����У����̵߳���������
  void inMsgRecvQueue() {
    for (int i = 0; i < 10000; i++) {
      cout << "inMsgQueue����һ��Ԫ��" << i << endl;
#ifdef __WINDOWS
      //�����ٽ���
      // EnterCriticalSection(&my_winsec);
      // msgRecvQueue.push_back(i);//ִ�в���
      // LeaveCriticalSection(&my_winsec);//�뿪�ٽ���

      CwinLock wlock(&my_winsec);  //���ö�β������
      msgRecvQueue.push_back(i);   //ִ�в���

#else
      std::lock_guard<mutex> my_lockguard(my_mutex);
      // my_mutex.lock();
      msgRecvQueue.push_back(i);  //�����������i�����յ�����ҵ�����
                                  // my_mutex.unlock();
#endif
    }
  }

  //���������ݺ����ķ�װ������ʹ��lock_guard()
  bool outMsgprocess(int &command) {
#ifdef __WINDOWS
    EnterCriticalSection(&my_winsec);

    if (!msgRecvQueue.empty()) {
      //��Ϣ���в�Ϊ��
      command = msgRecvQueue.front();  //���ص�һ��Ԫ��
      msgRecvQueue.pop_front();        //�Ƴ���һ��Ԫ��
      LeaveCriticalSection(&my_winsec);
      return true;
    }
    LeaveCriticalSection(&my_winsec);
    return false;
#else

    my_mutex.lock();
    if (!msgRecvQueue.empty()) {
      //��Ϣ���в�Ϊ��
      command = msgRecvQueue.front();  //���ص�һ��Ԫ��
      msgRecvQueue.pop_front();        //�Ƴ���һ��Ԫ��
      my_mutex.unlock();
      return true;
    }
    my_mutex.unlock();
    return false;
    //���з�֧��Ҫ��unlock()���������ڱ���������unlock()
#endif
  }
  //�����ݴ���Ϣ������ȡ�������߳�
  void outMsgRecvQueue() {
    int command = 0;
    for (int i = 0; i < 10000; i++) {
      bool result = outMsgprocess(command);
      if (result == true) {
        cout << "ȡ��Ϣ����ִ�гɹ�" << command << endl;
      } else {
        cout << "��Ϣ�����е���ϢΪ��" << i << endl;
      }
    }

    cout << endl;
  }

  //����һ��widows�ٽ�������
#ifdef __WINDOWS
  A() {
    //��ʼ���ٽ���,��֮ǰ�����ʼ��
    InitializeCriticalSection(&my_winsec);
  }
#endif
 private:
  list<int> msgRecvQueue;  //�������������ҷ��͹���������
  //����һ���������ĳ�Ա����
  mutex my_mutex;
  //����һ���ݹ黥����--û��ʹ��
  recursive_mutex my_mutex2;

  //����һ��widows�ٽ�������
#ifdef __WINDOWS
  CRITICAL_SECTION my_winsec;
#endif
};

int main(void) {
  A myobj;
  thread myOutMsgObj(&A::outMsgRecvQueue,
                     &myobj);  //�ڶ��������ò��ܱ�֤�߳����õ���ͳһ������
  thread myInMsObj(&A::inMsgRecvQueue, &myobj);
  myOutMsgObj.join();
  myInMsObj.join();

  cout << "main�߳�" << endl;  //���ִ����һ�䣬�����߳��˳�
  system("pause");
  return 0;
}

/*
 * windows�µ��ٽ����ǳ�������c++11�е�mutex;�ٽ���ʹ��ǰ�����ʼ��
 * ��Ҫ����#include<windows.h>
 * (1)�����ٽ������ͼ���һ����(2)ִ�в���(3)�뿪�ٽ������ͽ���һ��unlock()��
 *
 * ��ν����ٽ���ʵ��
 *��ͬһ���߳��У���ν����ٽ�������ͬ���ٽ�������������Ҫ����뿪�ٽ�����Ҫ��ԣ�ʹ��windows�ٽ����ǿ��Ե�
 *���뼸���ٽ�����Ҫ�뿪�����ٽ�����
 *c++11�л�����mutex�ǲ����ԼӶ��������������--��ͬ��mutex��������ͬһ���߳��е��ö�Ρ�
 *
 *
 *�Զ���������
 *std::lock_guard()��������ʹ���Զ�����������
 *
 *
 *�ݹ�Ķ�ռ������ recursive mutex--����ͬһ���̶߳��lock(),�÷���mutex��ͬ
 *���ʹ��������lock()--->Ӧ�ÿ��Ǵ����Ƿ����Ż��Ŀռ�
 *
 *����ʱ�Ļ����� std::timed_mutex--��ʱ���ܵĻ�����,�ȴ�����ʱ�䲻����
 *	���˽ӿ�  try_lock_for()   try_lock_until()--�ȴ�δ����һ������ڵ�
 std::chrono::milliseconds timeout(100);
 *if(my_mutex2.try_lock_for(timeout))
 *���� if (my_mutex2.try_lock_until(chrono::stead_clock::now()+timeout))
 *{
 *	û���õ���ͷ
 *}
 *else
 *{
 *	����˯��100ms�����´��ܲ����õ���
 *}
 *
 *std::recursive_timed_mutex����ʱ���ܵĵݹ黥����
 */
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
//�����౻��ΪRAII�ࣨresource acquisition is initialation��
class CwinLock {
 public:
    //���캯��
    CwinLock(CRITICAL_SECTION *pCritem) {
        m_pCritical = pCritem;
        EnterCriticalSection(m_pCritical);
    }
    //��������
    ~CwinLock() {
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
            // EnterCriticalSection(&winsec);
            // msgRecvQueue.push_back(i);//ִ�в���
            // LeaveCriticalSection(&winsec);//�뿪�ٽ���

            CwinLock wlock(&winsec);    //���ö�β������
            msgRecvQueue.push_back(i);  //ִ�в���
#else
            std::lock_guard<mutex> lock(mtx1);
            msgRecvQueue.push_back(i);  //�����������i�����յ�����ҵ�����
#endif
        }
    }

    //���������ݺ����ķ�װ������ʹ��lock_guard()
    bool outMsgprocess(int &command) {
#ifdef __WINDOWS
        EnterCriticalSection(&winsec);

        if (!msgRecvQueue.empty()) {
            //��Ϣ���в�Ϊ��
            command = msgRecvQueue.front();  //���ص�һ��Ԫ��
            msgRecvQueue.pop_front();        //�Ƴ���һ��Ԫ��
            LeaveCriticalSection(&winsec);
            return true;
        }
        LeaveCriticalSection(&winsec);
        return false;
#else

        mtx1.lock();
        if (!msgRecvQueue.empty()) {
            //��Ϣ���в�Ϊ��
            command = msgRecvQueue.front();  //���ص�һ��Ԫ��
            msgRecvQueue.pop_front();        //�Ƴ���һ��Ԫ��
            mtx1.unlock();
            return true;
        }
        mtx1.unlock();
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
        InitializeCriticalSection(&winsec);
    }
#endif
 private:
    list<int> msgRecvQueue;  //�������������ҷ��͹���������
    //����һ���������ĳ�Ա����
    mutex mtx1;
    //����һ���ݹ黥����--û��ʹ��
    recursive_mutex mtx2;

    //����һ��widows�ٽ�������
#ifdef __WINDOWS
    CRITICAL_SECTION winsec;
#endif
};

int main(void) {
    A myobj;
    //�ڶ��������ò��ܱ�֤�߳����õ���ͳһ������
    thread myOutMsgObj(&A::outMsgRecvQueue, &myobj);
    thread myInMsObj(&A::inMsgRecvQueue, &myobj);
    myOutMsgObj.join();
    myInMsObj.join();

    cout << "main�߳�" << endl;  //���ִ����һ�䣬�����߳��˳�
    system("pause");
    return 0;
}

/*
 * һ��windows�µ��ٽ����ǳ�������c++11�е�mutex;�ٽ���ʹ��ǰ�����ʼ��
 * ��Ҫ����#include<windows.h>
 * (1)�����ٽ������ͼ���һ��lock()��(2)ִ�в���(3)�뿪�ٽ������ͽ���һ��unlock()��
 *
 * ������ν����ٽ���ʵ��
 * ��ͬһ���߳��У���ν����ٽ�������ͬ���ٽ�������������Ҫ����뿪�ٽ�����Ҫ��ԣ�ʹ��windows�ٽ����ǿ��Ե�
 * ���뼸���ٽ�����Ҫ�뿪�����ٽ�����
 *
 * c++11�л�����mutex�ǲ����ԼӶ��������������--��ͬ��mutex��������ͬһ���߳��е��ö�Ρ�
 *
 * �����Զ�����������RAII
 * std::lock_guard()��������ʹ���Զ�����������
 *
 * �ġ�������mutex
 * recursive_mutex--�ݹ�Ķ�ռ��������ͬһ���߳̿��Զ��lock()���÷���mutex��ͬ
 * ���ʹ��������lock()--Ӧ�ÿ��Ǵ����Ƿ����Ż��Ŀռ�
 *
 * timed_mutex--��ʱ���ܵĻ�����,�ȴ�����ʱ�䲻����
 * try_lock_for()   try_lock_until()--�ȴ�δ����һ��ʱ��ڵ�
 *
 * std::chrono::milliseconds timeout(100);
 * if (mtx2.try_lock_for(timeout))
 * if (mtx2.try_lock_until(chrono::stead_clock::now() + timeout)) {
 *   // �õ���
 * } else {
 *  // ����˯��100ms�����´��ܲ����õ���
 * }
 *
 * recursive_timed_mutex����ʱ���ܵĵݹ黥����
 */
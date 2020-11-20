#include <iostream>
#include <list>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
using namespace std;

//�ó�Ա������Ϊ�̳߳�ʼ������

class A {
 public:
    //���յ�����Ϣ�뵽һ�����У����̵߳���������
    void inMsgRecvQueue() {
        for (int i = 0; i < 10000; i++) {
            cout << "inMsgQueue����һ��Ԫ��" << i << endl;

            mtx1.lock();  //����һ����������һ����
            //��������һ�����ţ����ܱ��治ͬ�Ĵ���
            mtx2.lock();
            //Ҳ����ʹ��lock_guard()
            // lock_guard<mutex> mylock1(mtx1);

            msgRecvQueue.push_back(i);  //�����������i�����յ�����ҵ�����
            mtx1.unlock();
            mtx2.unlock();
        }
    }

    //���������ݺ����ķ�װ����
    bool outMsgprocess(int& command) {
        mtx2.lock();
        mtx1.lock();
        if (!msgRecvQueue.empty()) {
            //��Ϣ���в�Ϊ��
            int command = msgRecvQueue.front();  //���ص�һ��Ԫ��
            msgRecvQueue.pop_front();            //�Ƴ���һ��Ԫ��
            mtx1.unlock();
            mtx2.unlock();
            return true;
        }
        mtx1.unlock();
        mtx2.unlock();
        return false;
        //���з�֧��Ҫ��unlock()���������ڱ���������unlock()
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

 private:
    list<int> msgRecvQueue;  //�������������ҷ��͹���������
    //����һ���������ĳ�Ա����
    mutex mtx1;
    mutex mtx2;
};

int main(void) {
    A myobj;

    thread myOutMsgObj(&A::outMsgRecvQueue, &myobj);
    thread myInMsObj(&A::inMsgRecvQueue, &myobj);
    myOutMsgObj.join();
    myInMsObj.join();

    cout << "main�߳�" << endl;  //���ִ����һ�䣬�����߳��˳�
    system("pause");
    return 0;
}

/*
 *������������������Ҳ�������������������������߳�A B��ʹ����������������
 *�߳�Aִ�е�ʱ��lock a�ɹ���Ȼ����ȥ������һ����b��
 *�������������л����߳�B������b����Ȼ����a��,����a���Ѿ���ʹ�ã�
 *�߳�A���߳�B����ȴ��Է�������(������˳���෴)��
 *�������߳�������˳����ͬ�Ͳ���������⡣
 *
 */

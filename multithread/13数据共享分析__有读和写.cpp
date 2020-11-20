#include <iostream>
#include <list>
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
            msgRecvQueue.push_back(i);  //�����������i�����յ�����ҵ�����
        }
    }

    //�����ݴ���Ϣ������ȡ�������߳�
    void outMsgRecvQueue() {
        for (int i = 0; i < 10000; i++) {
            if (!msgRecvQueue.empty()) {
                //��Ϣ���в�Ϊ��
                int command = msgRecvQueue.front();  //���ص�һ��Ԫ��
                msgRecvQueue.pop_front();            //�Ƴ���һ��Ԫ��
            } else {
                cout << "��Ϣ�����е���ϢΪ��" << i << endl;
            }
        }

        cout << endl;
    }

 private:
    list<int> msgRecvQueue;  //�������������ҷ��͹���������
};

int main(void) {
    A myobj;
    thread myOutMsgObj(&A::outMsgRecvQueue,
                       &myobj);  //�ڶ��������ò��ܱ�֤�߳����õ���ͬһ������
    thread myInMsObj(&A::inMsgRecvQueue, &myobj);
    myOutMsgObj.join();
    myInMsObj.join();

    cout << "main�߳�" << endl;  //���ִ����һ�䣬�����߳��˳�
    system("pause");
    return 0;
}
/*
 *�е��̶߳����ݣ��е��߳�д���ݣ���ô��Ҫ����Ĵ���,
 *��򵥵Ĵ�����Ƕ���ʱ����д��д��ʱ���ܶ���ֻ����һ���߳�д
 *Ҳ����ֻ����һ���߳�ռ�����ݡ���������д���ݺͶ����ݶ��Ƿֺܶಽ��
 *�磺�ƶ�ָ��ȡ����������л��ᵼ�ºܶ��������鷢����
 *
 *
 *���������
 *���ݹ����ʵ������Ʊ��ʣ��Ʊ������Ҫ�����еĿͻ��˽��й������Ҳ���֮��Ҫԭ���Ե�
 *
 *�������ݵı�����������
 *����Ҫ��һ��������Ϸ�����������������̣߳�һ���߳��ռ���������������д��һ��������
 *�ڶ����̴߳Ӷ�����ȡ����ҷ������������ִ����ҵĶ�����
 *���������ֱ�ʾ��ҷ��͹��������ʹ��list���档��Ƶ���İ�˳���ȡ����Ч�ʸ�--˫���б�����
 *
 *���ж�����д�������̣߳�������п��ƣ���ֻ֤��һ���߳̽��ж�����д���ɡ�
 *���룺����������--���̹߳������ݵı�������
 *
 */

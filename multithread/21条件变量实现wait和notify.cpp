#include <condition_variable>
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
    //���յ�����Ϣ�뵽һ�����е����߳�
    void inMsgRecvQueue() {
        for (int i = 0; i < 10000; i++) {
            cout << "inMsgQueue����һ��Ԫ��" << i << endl;
            unique_lock<mutex> lock(mtx);
            msgRecvQueue.push_back(i);
            // cv.notify_one();//���ǳ��԰�wait�̻߳��ѣ�ִ�к�wait�����ѣ����������
            cv.notify_all();
        }
    }

    //���������ݺ����ķ�װ����
    // bool outMsgprocess(int&command)
    //{
    //	//���˫�ؼ��---˼�����ܷ��Ϊ�����ݲ�Ϊ�յ�ʱ��֪ͨ����
    //	if (!msgRecvQueue.empty())
    //	{
    //		unique_lock<mutex> myGuard1(mtx);
    //		if (!msgRecvQueue.empty())
    //		{
    //			//��Ϣ���в�Ϊ��
    //			command = msgRecvQueue.front();//���ص�һ��Ԫ��
    //			msgRecvQueue.pop_front();//�Ƴ���һ��Ԫ��
    //			return true;
    //		}
    //	}
    //
    //	return false;
    //	//���з�֧��Ҫ��unlock()���������ڱ���������unlock()
    //}

    //�����ݴ���Ϣ������ȡ�������߳�
    void outMsgRecvQueue() {
        int command = 0;
        while (true) {
            unique_lock<mutex> lock(mtx);
            cv.wait(lock, [this] {
                if (!msgRecvQueue.empty()) {
                    return true;
                }
                return false;
            });  // lambda��������ٻ���
            command = msgRecvQueue.front();
            msgRecvQueue.pop_front();
            lock.unlock();  //��Ϊunique_lock������ԣ�������ʱ����
                            //������һЩ����Ķ�����������ҳ鿨�Ȳ���
            //�����������ڴ���һ��������Ҫһ��ʱ�䣬�����ǿ���wait()�ȴ�����ʱ���
            // notify_one()������û�����á�Ҳ����˵notify_one()��һ�����Ի���wait�߳�
        }

        /*
         * wait()�����ȴ�һ������
         * �ڶ�������lambda���ʽ����ֵ��false����ôwait�������������������������У�˯�ߣ���
         * ������������������notify_one()Ϊֹ��û�еڶ��������ȼ���false��
         *
         * ����ڶ�������ֵ��true����ôwaitֱ�ӷ���
         *
         * �������߳�ʹ��notify_one()����wait
         * 1���ٴγ��Ի�ȡ���������������ȡ�����߳���������ȴ���ȡ�������ȡ������wait�ͼ���
         * 2�����wait�еڶ���������lambda�������ж�������ʽ��
         *   ������ʽΪfalse���ͽ������½���˯�ߣ��ȴ��ٴα�notefy_one()����
         *	 ������ʽΪtrue����ô����ֱ������������ʱ�������������š�
         *	 ����ֻҪ���ߵ����ﻥ����һ�������ŵġ�
         *
         *	���waitû�еڶ������������������ߡ�
         *
         */
    }

 private:
    list<int> msgRecvQueue;      //�������������ҷ��͹���������
    std::mutex mtx;              //����һ���������ĳ�Ա����
    std::condition_variable cv;  //����һ����������
};

int main(void) {
    A myobj;
    //������һ���̱߳�����
    thread myInMsObj(&A::inMsgRecvQueue, &myobj);
    thread myOutMsgObj(&A::outMsgRecvQueue, &myobj);
    thread myOutMsgObj2(&A::outMsgRecvQueue, &myobj);

    myInMsObj.join();
    myOutMsgObj.join();
    myOutMsgObj2.join();

    cout << "main�߳�" << endl;  //���ִ����һ�䣬�����߳��˳�
    system("pause");
    return 0;
}
/*
 *�������� std::condition_variable
 *��������--�����߳��У���һ����������ص��࣬˵���˾��ǵȴ��������㣬��Ҫ�ͻ��������ʹ��
 *
 *wait()--��Ҫ����һ������������˯�߽���
 *notify_one()--����wait()���̣����ǲ��ܱ�֤һ���ܻ���
 *
 *����˼����
 *�������߳��õ����Ĵ�����һ����Ҳ��˵���ݴ���������ô��
 *��������̻߳���Լ�����ݵĴ���
 *ʵ�ʵĻ����У������ҵĲ���һֱ�ò���ִ���ǲ����Եġ�
 *
 *notify_one()ֻ��֪ͨһ���̣߳�Ҫ��֪ͨ����߳̾���Ҫʹ��notify_all()
 *
 */

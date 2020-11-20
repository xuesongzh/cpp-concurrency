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
    //ͨ����������һ��unique_lock����ʵ������Ȩת��
    unique_lock<mutex> rtn_unique_lock() {
        unique_lock<mutex> tempLock(mtx);
        return tempLock;  //�Ӻ�������һ���ֲ���unique_lock�ǿ��Ե�
                          //�ƶ����캯����ϵͳ��ʱ����һ����ʱ����
    }

    //���յ�����Ϣ�뵽һ�����У����̵߳���������
    void inMsgRecvQueue() {
        for (int i = 0; i < 10000; i++) {
            cout << "inMsgQueue����һ��Ԫ��" << i << endl;

            // adopt_lock:��ʾ�������Ѿ���lock�ˣ�������������ǰ��lock�ˣ�����ᱨ�쳣��
            //���÷��߳��Ѿ�ӵ���˻�����������Ȩ������Ҫ�ڹ��캯�����ٴ�lock�ˡ�
            mtx.lock();
            unique_lock<mutex> lock1(mtx, adopt_lock);

            // try_to_lock:�����ǻ᳢����mutex��lock()ȥ����mutex���������û�������ɹ���
            //���������أ��������������������ǰlock��
            unique_lock<mutex> lock2(mtx, try_to_lock);
            //����õ�����
            if (lock2.owns_lock()) {
                msgRecvQueue.push_back(i);  //�����������i�����յ�����ҵ�����
            } else {
                cout << "û���õ���----------------------" << endl;
            }

            // default_lock:������lockס������û�и�mutex��������ʼ����һ��û�м�����mutex��
            //�������ĵ���һЩunique_lock�ĳ�Ա������
            unique_lock<mutex> lock3(mtx, defer_lock);
            //û�м�����mtx������֮�����Լ�����
            lock3.lock();
            if (lock3.try_lock()) {  //���Լ��������ɹ�Ҳ������
                msgRecvQueue.push_back(i);
            } else {
                cout << "û���õ���----------------------" << endl;
            }

            // release���unique_lock��mutex֮��Ĺ���
            unique_lock<mutex> myGuard(mtx);
            mutex* ptx = myGuard.release();
            msgRecvQueue.push_back(i);
            ptx->unlock();

            //����Ȩת��
            unique_lock<mutex> myGuard1(mtx);
            //��������Ȩ�ǷǷ���
            // unique_lock<mutex> myGuard2(myGuard1);
            //�ƶ����壬�൱��myGuard2��mtx�󶨵�һ��myGuard1ָ��գ�myGuard2ָ����mtx
            unique_lock<mutex> myGuard2(move(myGuard1));

            mutex* ptx = myGuard2.release();
            msgRecvQueue.push_back(i);
            ptx->unlock();

            // unique_lock<mutex> myGuard1 = rtn_unique_lock();
        }
    }
    //���������ݺ����ķ�װ����
    bool outMsgprocess(int& command) {
        // lock_guard<mutex> myGuard1(mtx);
        unique_lock<mutex> myGuard1(mtx);

        //˯��20s
        /*chrono::milliseconds dura(20000);
        this_thread::sleep_for(dura);*/

        if (!msgRecvQueue.empty()) {
            //��Ϣ���в�Ϊ��
            command = msgRecvQueue.front();  //���ص�һ��Ԫ��
            msgRecvQueue.pop_front();        //�Ƴ���һ��Ԫ��
            return true;
        }

        return false;
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
    mutex mtx;
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
 *һ��unique_lock()ȡ��lock_guard()
 *unique_lock��һ����ģ�壬������һ���Ƽ�ʹ��lock_guard()����ȡ����lock(),unlock()����
 *unique_lock��lock_guard���ܶ࣬Ч���ϲ�һ�㣬�ڴ�ռ�ö�һ�㡣
 *
 *��������
 *Ĭ�ϲ�������lock_guard��unique_lock��ͬ
 *
 *unique_lock֧�ָ���Ĳ���
 *
 *������Ա����
 * lock()
 * unlock()--���Զ��������������м䴦��һЩ�ǹ�����룬Ȼ�������ϣ�
 * try_lock()--���Լ��������ɹ�Ҳ������
 * release()--�������������mutex����ָ�룬���ͷŹ���Ȩ
 *	Ҳ����unique_lock��mutex�����й�ϵ,���ԭ��mutex���ڼ���״̬������Ҫ�Լ��ӹܽ�����
 *	��Ҫ����release()��unlock()
 *
 *Ϊʲô��ʱ����Ҫunlock()����Ϊ��lock��ס�Ĵ���Խ�٣���������Խ��
 *��ʱ��Ҳ����ס�Ĵ�����ٳ�Ϊ�������ȣ�����һ���ô�ϸ��������ס�Ĵ���Խ�٣�ִ��Ч��Խ�ߡ�
 *����Խ�֣�ִ�е�Ч��Խ�͡�����ѡ����ʵ������ǳ���Ա������ʵ��������
 *
 *
 *�ģ�unique_lock()����Ȩ�Ĵ���
 *ͨ�������unique_lockӦ�ú�һ��mutex�໥������
 *unique_lock<mutex> myGuard(mtx);
 *myGuardӵ��mtx������Ȩ����������Ȩ����ת�Ƹ�����unique_lock���󣬵�������Ȩ���ܸ���
 *
 * 1)std::move
 * 2)return std::unique_lock<std::mutex>
 */

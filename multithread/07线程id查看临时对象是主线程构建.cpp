#include <iostream>
#include <string>
#include <thread>

using namespace std;
/*
 * �߳�id��id��һ�����飬ÿһ�����̺߳����̶߳���Ӧ��һ��id���֣���ͬ���߳�
 * �����߳�id�ǲ�ͬ�ģ��߳�id���Ի�ȡ�ģ�ʹ��c++��׼���еĺ�����ȡstd::this_thread::get_id()
 *
 */
class A {
 public:
    int m_i;
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
};

void MyPrint(const A& pMyBuf) {
    cout << "���̵߳�MyPrint�Ĳ�����ַ��:" << &pMyBuf << endl;
    cout << "thread id" << this_thread::get_id() << endl;
}

int main(void) {
    int myVar = 10;
    int mySecondVar = 20;
    cout << "������������ִ��" << endl;
    cout << "���߳�id�ǣ�" << this_thread::get_id() << endl;

    // thread mythread(MyPrint, mySecondVar);
    thread mythread(MyPrint, A(mySecondVar));

    //��һ�����ζ���MySecondVar���ݸ�A���Ͷ��������ת�����캯��
    mythread.join();
    // mythread.detach();

    system("pause");
    return 0;
}

/*
 *���Կɵã����ʹ����ʽ����ת������ô���캯�������߳���ִ��
 *���ʹ��ǿ������ת������һ����ʱ������ô�������߳��й�������(���������Ϳ�������)��
 *ֻҪ�����߳��й��죨�������죩�����ʱ������ô���߳̾Ϳ���ʹ��
 */
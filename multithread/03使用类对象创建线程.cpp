#include <iostream>
#include <string>
#include <thread>

using namespace std;
class testClass {
 public:
    int &m_i;
    //���캯��
    testClass(int &i) : m_i(i) {
        cout << "��testClass�Ĺ��캯����ִ��" << endl;
    }

    //�������캯��
    testClass(const testClass &ts) : m_i(ts.m_i) {
        cout << "�������캯����ִ��" << endl;
    }

    ~testClass() {
        cout << "����������ִ��" << endl;
    }

    void operator()()  //����()����������һ���ɵ��ö���
    {
        cout << "�ҵ��߳�opertator��ʼִ����\n" << endl;
        cout << "m_i1=" << m_i << endl;
        cout << "m_i2=" << m_i << endl;
        cout << "m_i3=" << m_i << endl;
        cout << "m_i4=" << m_i << endl;
        cout << "�ҵ��߳�opertator����ִ����\n" << endl;
    }
};

int main(void) {
    //ʹ���ഴ���߳�
    int myi = 6;
    testClass mytestclass(myi);
    thread myThread(mytestclass);  //ʹ������󴴽�һ���̣߳�����ÿ������캯��

    // myThread.join();
    myThread.detach();
    cout << "�������̵߳�ִ��\n" << endl;

    system("pause");
    return 0;
}
/*
 * ���̴߳�ӡ������̵߳�һ��int���͵Ŀռ䣨�ֲ��������������߳�ִ����ϣ����̼߳����������̵߳��ڴ�ռ䣬�ͻ����
 * �������ﲻ��ʹ�����á�
 *
 *  ���⣺�����߳�ִ����ϣ���ô���߳��еĶ�����������ʹ�������߳��еĶ���û�б������𣿣�
 *	ԭ���ǣ������Ѿ��������ˣ����Ǹ�����һ���¶���mytestclass�����߳��С�
 *  ���߳��еĶ��󲻴����ˣ����Ǹ��ƵĶ������ɴ��ڡ�ֻҪ������û�����ã�ָ��ȾͲ��������⡣
 *  ����ÿ������캯����ʹ��join()���Կ��ƣ��϶������һ�ι��죬һ�ο������죬����������
 *  ����ʹ��detach()��������������ܿ��ƣ�������һ��������Ρ�
 *
 */
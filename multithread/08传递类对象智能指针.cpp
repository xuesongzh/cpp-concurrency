#include <iostream>
#include <memory>
#include <string>
#include <thread>

using namespace std;

class A {
 public:
    mutable int m_i;  // m_i��ֵ�ǿ����޸ĵģ���ʹ��const��
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
    pMyBuf.m_i = 3333;  //�����޸����ֵ������Ӱ�쵽���߳��е�ֵ
    cout << "���̵߳�MyPrint�Ĳ�����ַ��:" << &pMyBuf << endl;
    cout << "thread is:" << this_thread::get_id() << endl;
}

//����һ������ָ��
void MyPrint2(unique_ptr<int> pint) {
    cout << "���߳�thread id is:" << this_thread::get_id() << endl;
}

int main(void) {
    A myobj(10);
    // thread mythread(MyPrint,myobj);//���������Ϊ�̵߳Ĳ���
    thread mythread(MyPrint, ref(myobj));  //���������Ϊ�̵߳Ĳ���

    mythread.join();
    // mythread.detach();

    system("pause");
    return 0;
}

/*
 *�������������ָ����Ϊ�̲߳�������
 *std::ref()����
 *��Ȼ��������Ϊ������������Ȼ���ÿ������캯���������̴߳�����������͵Ĳ��������ܽ�����
 *ʹ��ʲô���ͽ��ܣ����ý��ܻ���ֵ���ܣ������������ǰ��տ�������ʽ���ܡ�
 *��������Ҫ����һ��Ӱ�����̵߳����ã������ǿ�������ôʹ��ref��reference��,�Ͳ�����ÿ������캯��
 */
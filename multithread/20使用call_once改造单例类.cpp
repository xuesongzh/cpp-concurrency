#include <allocators>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
using namespace std;

//������
std::mutex resource_mutex;
std::once_flag gflag;  //����ϵͳ����ǻ�û�б���λ

//������
class MyCAS {
 private:
    //���캯��˽�л�
    MyCAS() {}
    static void createInstance() {
        m_instance = new MyCAS();
        static ReleaseObject ro;
    }

 private:
    static MyCAS* m_instance;  //��̬��Ա����ָ��

 public:
    static MyCAS* getInstance() {
        //�����߳�ͬʱִ�е��������һ���̵߳ȴ�����һ���߳�ִ�����createInstance
        std::call_once(gflag, createInstance);
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

    void function1() {
        cout << "���Ժ���" << endl;
    }
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
    ////���ظ�������ָ��
    // MyCAS*p_a = MyCAS::getInstance();
    // p_a->function1();

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
 * std::call_once��һ������ģ�壬��c++11����ĺ���
 * �ڶ���������һ���������֣�call_once�Ĺ����Ǳ�֤����ֻ�ᱻ����һ��
 * call_once���л�������������������Ч���ϣ���Ȼ���������ռ�õ���Դ����
 * call_once��Ҫ��һ����ǽ��ʹ�ã�������once_flag����ʵ��once_flagһ�ֽṹ
 * call_once����ͨ���������жϺ����Ƿ�ִ�У�ִ�й�������Ͱ������������Ϊ�Ѿ�����״̬
 * �����ٵ��õ��Ǳ�־�Ѿ�������Ϊ���Ѿ����á�״̬�ˣ��Ͳ�����ִ���������
 */

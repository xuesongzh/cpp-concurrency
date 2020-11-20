#include <future>
#include <iostream>
#include <string>
#include <thread>
using namespace std;

class A {
 public:
    int mythread(int mypar) {
        cout << "mpar=" << mypar << endl;
        cout << "�߳̿�ʼִ��,�̵߳�id�ǣ�" << std::this_thread::get_id() << endl;
        //��Ϣ5s
        chrono::milliseconds dura(5000);
        this_thread::sleep_for(dura);

        return 5;
    }
};

int main(void) {
    A a;
    int tempar = 12;
    cout << "main thread id:" << std::this_thread::get_id() << endl;
    // std::future<int> result = std::async(mythread);

    // �ڶ�������ʹ�ö������ã����ܱ�֤ʹ�õ���ͬһ������
    std::future<int> result = std::async(&A::mythread, &a, tempar);

    //����ͨ����async()��������һ���������ò���������std::launcher����(ö��)�����ﵽһЩ�����Ŀ�ġ�

    // std::launch::deferred--��ʾ�߳���ں������ñ��ӳٵ�std::future��get()����wait()���ò�ִ��
    //��ô���get����waitû�е��ã��̺߳�������ִ�У�ʵ�����̸߳���û�д�����
    //�ӳٵ��ã�����û�д������̣߳������߳���ִ�����߳���ں�����

    // std::launch::async --�첽���ڵ���async����ʱ��ʹ��������߳�
    //�����д��һ����ǣ�Ĭ�Ͼ������std::launch::async

    // std::launch::any

    std::future<int> result = std::async(std::launch::deferred, &A::mythread, &a, tempar);

    cout << "continue........" << endl;
    int def = 0;
    //��������ȴ��߳�ִ����ϣ�ͨ��future�����get()�����ȴ�����������õ����ؽ��
    cout << "δ����ִ�н���ǣ�" << result.get() << endl;
    system("pause");
    return 0;
}

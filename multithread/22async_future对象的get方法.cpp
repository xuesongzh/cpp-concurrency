#include <future>
#include <iostream>
#include <string>
#include <thread>
using namespace std;

int mythread() {
    cout << "�߳̿�ʼִ��,�̵߳�id�ǣ�" << std::this_thread::get_id() << endl;
    //��Ϣ5s
    chrono::milliseconds dura(5000);
    this_thread::sleep_for(dura);

    return 5;
}
int main(void) {
    cout << "main thread id:" << std::this_thread::get_id() << endl;
    std::future<int> result = std::async(mythread);
    //�Զ�����һ���̣߳��������߳���ں���
    cout << "continue........" << endl;
    int def = 0;
    //��������ȴ��߳�ִ����ϣ�ͨ��future�����get()�����ȴ�����������õ����ؽ��
    // get()ֻ�ܵ���һ��
    cout << "δ����ִ�н���ǣ�" << result.get() << endl;
    system("pause");
    return 0;
}

/*
 * std::async--����ģ�壬��������һ���첽������������󣬷���һ��std::future����
 * ʲô������һ���첽����--�Զ�����һ���̲߳�ִ�ж�Ӧ���߳���ں���������һ��future����
 * �����������Ͱ����߳���ں������صĽ���������̷߳��صĽ����Ҳ����ʹ��get()��������ȡ
 *
 * future--δ��---����Ҳ��Ϊfuture�ṩ��һ�ַ����첽��������Ļ���
 * ���ӵ����㣬��Ҫ�ȴ�δ����������--�����߳�ִ����ϾͿ����õ������
 * future��wait()����ֻ�ȴ��߳̽������������ؽ����
 */

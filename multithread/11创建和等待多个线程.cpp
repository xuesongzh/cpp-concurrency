#include <iostream>
#include <string>
#include <thread>
#include <vector>

using namespace std;

//�߳���ں���
void MyPrint(int num) {
    cout << "�������̵߳���ں��������߳̿�ʼִ��" << num << endl;

    cout << "�߳̽���ִ����" << num << endl;
    return;
}

int main(void) {
    vector<thread> mythread;
    //����10���߳�,���ͳһʹ��MyPrint
    for (int i = 0; i < 10; i++) {
        //�����̣߳���ִ��
        mythread.push_back(thread(MyPrint, i));
    }
    //�ȴ�10�����̷߳���
    for (auto iter = mythread.begin(); iter != mythread.end(); iter++) {
        iter->join();
    }

    cout << "main�߳�" << endl;  //���ִ����һ�䣬�����߳��˳�
    system("pause");
    return 0;
}
/*
 * ��1������̵߳�ִ��˳�����޹��ɵģ��Ͳ���ϵͳ���ڲ����û����йء�
 * ��2�����̵߳ȴ������е����߳�ִ����ϣ����̲߳��˳����Ƽ�����join��д��������д���ȶ��ĳ���
 * ��3�������˶���̲߳��ҷ���һ����̬���������С����㴴���͹������߳�
 */

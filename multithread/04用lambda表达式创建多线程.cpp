#include <iostream>
#include <string>
#include <thread>

using namespace std;

int main(void) {
    //ʹ��lambda���ʽ�����߳�
    auto myLambdaThread = [] {
        cout << "�ӽ��̿�ʼִ��" << endl;
        cout << "�ӽ��̽���ִ��" << endl;
    };

    thread mythread(myLambdaThread);
    mythread.join();

    cout << "����������ִ��" << endl;
    system("pause");
    return 0;
}

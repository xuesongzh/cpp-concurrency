#include <iostream>
#include <string>
#include <thread>
#include <vector>

using namespace std;

//��������
vector<int> g_var = {1, 2, 3};

//�߳���ں���
void MyPrint(int num) {
    cout << "id =" << this_thread::get_id() << "��ֵ" << g_var[0] << endl;
    return;
}

int main(void) {
    vector<thread> mythread;
    for (int i = 0; i < 10; i++) {
        mythread.push_back(thread(MyPrint, i));
    }

    for (auto iter = mythread.begin(); iter != mythread.end(); iter++) {
        iter->join();
    }

    cout << "main�߳�" << endl;  //���ִ����һ�䣬�����߳��˳�
    system("pause");
    return 0;
}
/*
 *���������ֻ���ģ�ÿ���̶߳�ȥ�����ݣ��ǰ�ȫ�ȶ��ģ�����Ҫʲô�ر���ֶ�
 */

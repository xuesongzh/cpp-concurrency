#include <iostream>
#include <string>
#include <thread>

using namespace std;

//�̺߳���
// void MyPrint1(const int &i, char *pMyBuf)
void MyPrint2(const int i, const string& pMyBuf) {
    cout << i << endl;
    cout << pMyBuf << endl;
    return;
}

int main(void) {
    //������ʱ������Ϊ����
    int myvar = 10;
    char mybuf[] = "���ǲ����ַ���";
    //��һ���������̵߳�ִ�к�����ʣ�µ��Ƕ�������б�
    // thread myThread(MyPrint1, myvar, mybuf);
    thread myThread(MyPrint2, myvar, string(mybuf));
    //������ַ�����mybuf����ת��Ϊ��ʱ��string�����ٽ��в�������
    //�������ʹ������,������mybuf��������ʲôʱ��ת��Ϊstring�ģ�
    //ȷʵ���ڵ�mian()����ִ�й�����ϣ��Ż���ַ�����mybufת��Ϊstring

    // myThread.join();

    //�����̺߳����̷߳����ʱ�򣬴��ݵĲ���int &i�����ã��Ḵ�ƣ����᲻�����
    //���ᣬд�Ĳ�����Ȼ�����ã����ǻḴ��һ�ݣ����ǵڶ���ָ��������������
    //ʹ��ָ��Ĭ��ִ�е���ǳ����
    myThread.detach();
    cout << "�������̵߳�ִ��" << endl;

    system("pause");
    return 0;
}

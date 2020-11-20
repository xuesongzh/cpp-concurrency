#include <future>
#include <iostream>
#include <string>
#include <thread>
using namespace std;

vector<std::packaged_task<int(int)>> myTasks;  // t����

int main(void) {
    cout << "���߳�id=" << std::this_thread::get_id() << endl;
    std::packaged_task<int(int)> mpt([](int mypar) {
        cout << "mpar=" << mypar << endl;
        cout << "�߳̿�ʼִ��,�̵߳�id�ǣ�" << std::this_thread::get_id() << endl;
        //��Ϣ5s
        chrono::milliseconds dura(5000);
        this_thread::sleep_for(dura);

        return 5;
    });

    std::thread t1(std::ref(mpt), 1);  // 1�Ǵ��ݵĲ���,����һ���̣߳�����ʼִ��
    t1.join();
    // packaged_task���̺߳�����װ��һ��

    //ͨ��future��ȡδ������ִ�е�ֵ
    // std::future��������������߳���ں����Ľ����result����mythread�ķ��ؽ�
    // get_future()��packaged_task()����Ľӿ�
    std::future<int> result = mpt.get_future();
    cout << result.get() << endl;

    myTasks.push_back(std::move(mpt));
    //����
    std::packaged_task<int(int)> mypt2;
    auto iter = myTasks.begin();
    mypt2 = std::move(*iter);  //�ƶ�����
    myTasks.erase(iter);       //ɾ����һ��Ԫ�أ��������Ѿ�ʧЧ�������ڼ���ʹ��

    mypt2(123);
    std::future<int> result2 = mypt2.get_future();
    cout << result2.get() << endl;

    system("pause");
    return 0;
}

/*
 *packaged_task:������񣬰������װ������
 *�Ǹ���ģ�壬����ģ������Ǹ��ֿɵ��ö���ͨ��packaged_task���԰Ѹ��ֿɵ��ö����װ������������Ϊ�߳���ں��������á�
 *packaged_task��װ�����Ŀɵ��ö��󣬻�����ֱ�ӵ��ã�������Ƕ�����packaged_taskҲ��һ���ɵ��ö���
 */

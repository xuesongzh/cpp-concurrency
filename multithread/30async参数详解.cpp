#include <future>
#include <iostream>
#include <string>
#include <thread>

using namespace std;

int mythread() {
    cout << "���߳�id=" << std::this_thread::get_id() << endl;
    return 1;
}

int main(void) {
    cout << "main thread id =" << std::this_thread::get_id() << endl;
    //����һ���첽����
    std::future<int> result = std::async(mythread);
    std::future_status status = result.wait_for(std::chrono::seconds(0));  //�Ƿ�����ִ����
    if (status == std::future_status::ready) {
        cout << "�߳�ִ�����" << endl;
    } else if (status == std::future_status::deferred) {
        cout << "�̱߳��ӳٵ���" << endl;
    } else if (status == std::future_status::timeout) {
        cout << "�̳߳�ʱ�䣬��û��ִ�н���" << endl;
    }
    cout << result.get() << endl;
    system("pause");
    return 0;
}

/*
 *std::async������ϸ̸
  --��������һ���첽����

  1.std::async(std::launch::deferred)--�ӳٵ��ã����������̣߳��ӳٵ�future�������get(),wait()�����Ż�ִ���̺߳�����ͬ����
  2.std::async(std::launch::async);--ǿ�ƴ���һ���첽�����̣߳�,�����߳���ִ�У��첽��
  3.deferred | async
  �ɲ���ϵͳ�Լ�����ѡ�񣬿����Ǵ������̣߳�����ִ�У�Ҳ����û�д������̣߳��ӳٵ�����get,wait����ִ���̺߳�����
  ��������--��ʵû��ָ��������Ĭ�ϲ����ǣ�deferred|async��

  ���о�����ʲô��˼������ô�����Ƿ񴴽����̣߳�

  std::thread()���ϵͳ��Դ�ѷ�����ô�����㴴���߳�ʧ�ܣ������������������thread()�����ķ���ֵ��̫�ý��գ�Ҫʹ��һ��ȫ�ֱ������ֶν��ա�
  std::async()�����첽���񣨿��ܴ����̣߳�Ҳ���ܲ������̣߳����̵߳ķ���ֵ����ʹ��future���������ա�
  ��async()�����߳�һ�㲻�ᱨ�쳣�����ϵͳ��Դ���ţ����Ӷ�������ĵ���async()�Ͳ��ᴴ���̣߳�
  ���Ǻ���˭����get()�����������߳���ں����������ڵ��õ��߳��С�
  ���ǿ�ƴ������̣߳���Ҫʹ��std::async(std::launch::async);

  ���ݾ��飬һ���������߳���Ŀ��100-200���߳��л����˷Ѻܶ�cpu��Դ��
  std::async()��ȷ����������
  ���Ӳ�����async()���ã�����ȷ��ϵͳ�ܷ񴴽������̡߳����ϵͳ����ѡ�����Ƴ�����
  ��ζ��mythread�߳���ں�����������ִ�У��������û�е���get()��ѹ���Ͳ�ִ�С�
  ����д��deferred|async�Ƿ���Ƴٴ����̡߳�Ҫʹ��std::wait_for()
*/
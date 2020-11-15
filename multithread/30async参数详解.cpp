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
  //����һ���첽�߳�
  // std::future<int> result = std::async(mythread);

  std::future<int> result = std::async(mythread);
  std::future_status status =
      result.wait_for(std::chrono::seconds(0));  //�Ƿ�����ִ����
  if (status == std::future_status::ready)       //--timeout --
  {
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
 *--��������һ���첽����
 *����--�ӳٵ��ò���
 *std::async(std::launch::deferred)--�ӳٵ��ã����Ҳ��������߳�--�ӳٵ�future�������get(),wait()�����Żᴴ��
        std::async(std::launch::async);--ǿ�ƴ���һ���첽�����̣߳�,�����߳���ִ��
        std::async(std::launch::any);
        std::async(std::launch::sync);
        deferred|asyncʹ��������ɲ���ϵͳ�Լ�����ѡ�񣬣�1�������̣߳�����ִ�У����ߣ��ӳٵ�get,wait����ִ�У�����������Ϊ�е�һ��
        ��������--��ʵû��ָ��������Ĭ�ϲ����ǣ�deferred|async������һ�ν�����
���о�����ʲô��˼������ô�����Ƿ񴴽����̣߳���
|
|
|
async()--����һ��д���һ���첽����async()��ʱ�򲻴���һ�����̣߳�˭����get()�����������Ǹ�������������ִ��
|
v
std::thread()���ϵͳ��Դ�ѷ�����ô�����㴴���߳�ʧ�ܡ����������������
thread()�����ķ���ֵ��̫�ý��գ�Ҫʹ��һ��ȫ�������ֶν��ܡ�
std::async()�����첽���񣨿��ܴ����̣߳�Ҳ���ܲ������̣߳����̵߳ķ���ֵ����ʹ��future����������
��async()�����߳�һ�㲻�ᱨ�쳣�����ϵͳ��Դ���ţ����Ӷ�������ĵ���async()�Ͳ��ᴴ���̣߳�
���Ǻ���˭����get()�����������߳���ں����������ڵ��õ��߳��С�
���ǿ�ƴ������̣߳���Ҫʹ��std::async(std::launch::async);


���ݾ��飬һ���������߳���Ŀ��100-200���߳��л����˷Ѻܶ�cpu��Դ��
std::async()��ȷ����������
        ���Ӳ�����async()���ã�����ȷ��ϵͳ�ܷ񴴽������̡߳����ϵͳ����ѡ�����Ƴ�����
        ��ζ��mythread�߳���ں�����������ִ�У��������û�е���get()��ѹ���Ͳ�ִ�С�
        ����д��deferred|async�Ƿ���Ƴٴ����̡߳�Ҫʹ��std::wait_for()
*/
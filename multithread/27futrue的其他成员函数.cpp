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

int mythread2() {
  cout << "�߳̿�ʼִ��,�̵߳�id�ǣ�" << std::this_thread::get_id() << endl;
  //��Ϣ5s
  chrono::milliseconds dura(5000);
  this_thread::sleep_for(dura);

  return 5;
}
int main(void) {
  cout << "main thread id:" << std::this_thread::get_id() << endl;
  std::future<int> result = std::async(mythread);
  // std::future<int> result = std::async(std::launch::deferred,mythread);

  //�Զ�����һ���̣߳��������߳���ں���
  cout << "continue........" << endl;

  //��������ȴ��߳�ִ����ϣ�ͨ��future�����get()�����ȴ�����������õ����ؽ��
  cout << "δ����ִ�н���ǣ�" << result.get() << endl;

  // future_status��ö������
  // wait_for()�ȴ�һ��ʱ��
  std::future_status status =
      result.wait_for(std::chrono::seconds(1));  //�ȴ�1s
  if (status == std::future_status::timeout) {  //��ʱ��ʾ�̻߳�û��ִ�����
    cout << "��ʱ���̻߳�û��ִ�����" << endl;
  } else if (status == std::future_status::ready) {
    cout << "�߳�ִ�����" << endl;
  } else if (status == std::future_status::deferred) {  //�ӳ�
    //���async()�ĵ�һ������������Ϊdeferred ,����������
    cout << "�̱߳��ӳ�ִ��" << endl;
    //ʹ��get()�Ż�ִ�У���ֻ��getһ��
    cout << result.get() << endl;
  }

  //�����ڶ����߳�
  std::future<int> result2 = std::async(mythread2);
  std::shared_future<int> result3(std::move(result2));
  //�ж�future���������Ƿ���ֵ
  bool ifCanGet = ressult3.valid();
  cout << ifCanGet << endl;  //���Ϊ1
  //�ȼ�������д��
  std::shared_future<int> ressult3(result2.share());
  //�������get���
  auto mythreadResult = ressult3.get();

  system("pause");
  return 0;
}

/*
 *wait_for()������һ��ö������
 *get()����ʹ�����λ�����쳣����Ϊget()�����������һ���ƶ����塣
 *shared_future()Ҳ��һ����ģ�壬future����ֻ��get()һ�Σ�shared_future()��get()�����Ǹ������ݡ�
 */

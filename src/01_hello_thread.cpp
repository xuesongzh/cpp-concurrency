#include <iostream>
#include <thread>

using namespace std;

void hello() {
  cout << "Hello World from new thread." << endl;
}

int main() {
  thread t(hello);
  t.join();//当前线程会一直阻塞，直到目标线程执行完成

  return 0;
}
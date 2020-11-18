#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

int g_i = 0;
mutex g_i_mutex;

void safe_increment() {
  lock_guard<mutex> lock(g_i_mutex);
  ++g_i;

  cout << this_thread::get_id() << ": " << g_i << '\n';
}

int main() {
  cout << "main: " << g_i << '\n';

  thread t1(safe_increment);
  thread t2(safe_increment);

  t1.join();
  t2.join();

  std::cout << "main: " << g_i << '\n';
}
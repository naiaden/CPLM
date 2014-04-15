#include <iostream>
#include <thread>
#include <vector>

#include "utils.hpp"

const int number_of_threads = 10;

void call_from_thread(int thread_id)
{
  std::cout << "Printed by thread " << thread_id << std::endl;
}

int main(int args, char** argv)
{
  std::thread t[number_of_threads];

  for(int i : range(10))
  {
    t[i] = std::thread(call_from_thread, i);
  }

  std::cout << "Printed from the main thread" << std::endl;

  for(auto& thread : t)
  {
    thread.join();
  }

  return 0;
}

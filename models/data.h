#include <iostream>
#include <string>
#pragma once

using namespace std;

class Data {
public:
  string machine;
  string token;
  double cpu_usage;
  double memory_usage;
  double disk_usage;
  int process_count;

  Data();
  Data(string machine, string token, double cpu_usage, double memory_usage,
       double disk_usage, int process_count);
  void createRegistry();
};

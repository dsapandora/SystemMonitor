#include <iostream>
#include <string>
#pragma once

using namespace std;

class Client {
public:
  string machine;
  string token;
  Client();
  Client(string machine, string token);
  void createClient();
  static Client *searchClient(string machine, string token);
};

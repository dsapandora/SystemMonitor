#include "../helpers/read_configuration_file.h"
#include "cpu_usage.h"
#include "memory_usage.h"
#include "processes_count.h"
#include <Poco/AutoPtr.h>
#include <Poco/FileChannel.h>
#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Logger.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/NetException.h>
#include <Poco/URI.h>
#include <chrono>
#include <iostream>
#include <thread>

using namespace Poco::Net;
using namespace std;
using Poco::Logger;
using Poco::FileChannel;
using Poco::AutoPtr;

Poco::JSON::Object::Ptr doRequest(string url, Poco::JSON::Object obj) {
  Poco::URI uri(url);
  std::string path(uri.getPathAndQuery());
  if (path.empty())
    path = "/";
  HTTPClientSession session(uri.getHost(), uri.getPort());
  HTTPRequest request(HTTPRequest::HTTP_POST, path, HTTPMessage::HTTP_1_1);
  request.setContentType("text/json");
  HTTPResponse response;
  request.setContentType("application/json");
  std::stringstream ss;
  obj.stringify(ss);
  request.setContentLength(ss.str().size());
  std::ostream &o = session.sendRequest(request);

  obj.stringify(o);

  // this line is where you get your response
  std::istream &s = session.receiveResponse(response);

  Poco::JSON::Parser parser;
  Poco::JSON::Object::Ptr ret =
      parser.parse(s).extract<Poco::JSON::Object::Ptr>();
  return ret;
}
inline bool exists(string name) {
  if (FILE *file = fopen(name.c_str(), "r")) {
    fclose(file);
    return true;
  } else {
    return false;
  }
}

bool registry() {
  string url, port;
  Poco::JSON::Object obj;
  cout << "Indicate the server url: (localhost)" << endl;
  cin >> url;
  cout << "Indicate the server port: (9090)" << endl;
  cin >> port;
  Poco::JSON::Object::Ptr ret =
      doRequest("http://" + url + ":" + port + "/register", obj);
  cout << ret->getValue<string>("token") << endl;
  cout << "Copy the following content in the client_config.xml" << endl;
  cout << "<config><client key=\"" + ret->getValue<string>("token") +
              "\"/><server url =\"http://" + ret->getValue<string>("hostname") +
              "/data\"/></config>"
       << endl;
  return false;
}

int main() {
  AutoPtr<FileChannel> pChannel(new FileChannel);
  pChannel->setProperty("path", "client.log");
  pChannel->setProperty("rotation", "2 M");
  pChannel->setProperty("archive", "timestamp");
  Logger::root().setChannel(pChannel);

  Logger &logger = Logger::get("TestLogger"); // inherits root channel
  if (!exists("client_config.xml")) {
    try {
      registry();
    } catch (Poco::Net::NetException e) {
      logger.error("Error registrando la conexion, " + e.displayText());
    }
    return 0;
  }
  while (true) {
    std::this_thread::sleep_for(std::chrono::minutes(5));
    try {
      Poco::JSON::Object obj;
      obj.set("token", client_key());
      obj.set("cpu", cpu());
      obj.set("memory", memory());
      obj.set("disk", 0);
      obj.set("process_count", GetTotalNumProcesses());
      doRequest(server_url(), obj);
      logger.information("memory usage: " + to_string(memory()) +
                         ", CPU USAGE: " + to_string(cpu()) +
                         ", TOTAL_PROCESS:" +
                         to_string(GetTotalNumProcesses()));
    } catch (Poco::Exception e) {
      logger.error("Error registrando la conexion, " + e.displayText());
      return 1;
    }
  }
  return 0;
}

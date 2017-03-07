#include "models/client.h"
#include "models/data.h"
#include <Poco/Dynamic/Var.h>
#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/UUID.h>
#include <Poco/UUIDGenerator.h>
#include <Poco/Util/ServerApplication.h>
#include <iostream>
#include <string>
#include <vector>

using Poco::UUID;
using Poco::UUIDGenerator;
using namespace Poco::JSON;
using namespace Poco::Net;
using namespace Poco::Util;
using namespace Poco::Dynamic;
using namespace std;

string GetValue(Object::Ptr aoJsonObject, const char *aszKey) {
  Poco::Dynamic::Var loVariable;
  string lsReturn;
  string lsKey(aszKey);

  // Get the member Variable
  //
  loVariable = aoJsonObject->get(lsKey);

  // Get the Value from the Variable
  //
  lsReturn = loVariable.convert<std::string>();

  return lsReturn;
}

class MyRequestHandler : public HTTPRequestHandler {
public:
  virtual void handleRequest(HTTPServerRequest &req, HTTPServerResponse &resp) {
    if (req.getURI() == "/data") {
      ostream &out = handle_send_data_request(req, resp);
      out.flush();
    } else if (req.getURI() == "/register") {
      ostream &out = handle_register_request(req, resp);
      out.flush();
    } else {
      ostream &out = handle_wrong_path_request(req, resp);
      out.flush();
    }
    cout << endl
         << "Response sent for count=" << count
         << " and message=" << req.getURI() << endl;
  }

private:
  static int count;
  virtual ostream &handle_wrong_path_request(HTTPServerRequest &req,
                                             HTTPServerResponse &resp) {
    ostream &out = resp.send();
    resp.setStatus(HTTPResponse::HTTP_NOT_FOUND);
    resp.setContentType("text/json");
    out << "{ \"error\" : \" Route Not Found \" } ";
    return out;
  }
  bool authorized(Object::Ptr loJsonObject, string machine) {
    // is Better to send it in the header but for now it need to be included in
    // the json
    try {
      string token = loJsonObject->getValue<string>("token");
      Client *c = Client::searchClient(machine, token);
      return c != NULL;
    } catch (Poco::Exception &error) {
      cout << "Error : " << error.displayText() << endl;
      return false;
    }
  }
  virtual ostream &handle_send_data_request(HTTPServerRequest &req,
                                            HTTPServerResponse &resp) {
    string response;
    Parser parser;
    string machine = req.clientAddress().host().toString();
    istream &in = req.stream();
    ostream &out = resp.send();
    resp.setStatus(HTTPResponse::HTTP_OK);
    resp.setContentType("text/json");
    // Read Json content
    getline(in, response);

    /*Parse json string*/
    Var loParsedJson = parser.parse(response);
    Var loParsedJsonResult = parser.result();
    // Get the JSON Object
    Object::Ptr loJsonObject = loParsedJsonResult.extract<Object::Ptr>();
    if (authorized(loJsonObject, machine)) {
      try {
        Data *registry =
            new Data(machine, loJsonObject->getValue<string>("token"),
                     loJsonObject->getValue<double>("cpu"),
                     loJsonObject->getValue<double>("memory"),
                     loJsonObject->getValue<double>("disk"),
                     loJsonObject->getValue<int>("process_count"));
        registry->createRegistry();
        out << "{ \"hostname\" : \"" << machine
            << "\", \"request\":" << response << " } ";
      } catch (Poco::Exception &error) {
        cout << "Error : " << error.displayText() << endl;
        out << "{ \"hostname\" : \"" << machine
            << "\", \"error\": \"Cannot create register, remeber to send, cpu, "
               "memory, disk, and process_count\" } ";
      }
    } else {
      out << "{ \"hostname\" : \"" << machine
          << "\",}\"error\": \"Not Authorized \"}";
    }
    return out;
  }

  virtual ostream &handle_register_request(HTTPServerRequest &req,
                                           HTTPServerResponse &resp) {
    string address = req.clientAddress().host().toString();
    UUIDGenerator &generator = UUIDGenerator::defaultGenerator();
    UUID uuid(generator.createFromName(UUID::uri(), address));
    ostream &out = resp.send();
    resp.setStatus(HTTPResponse::HTTP_OK);
    resp.setContentType("text/json");
    // Read Json content
    Client *c = new Client(address, uuid.toString());
    c->createClient();
    out << "{ \"hostname\" : \"" << req.getHost() << "\", \"token\":\""
        << c->token << "\" } ";
    return out;
  }
};

int MyRequestHandler::count = 0;

class MyRequestHandlerFactory : public HTTPRequestHandlerFactory {
public:
  virtual HTTPRequestHandler *createRequestHandler(const HTTPServerRequest &) {
    return new MyRequestHandler;
  }
};

class MonitorServerApp : public ServerApplication {
protected:
  int main(const vector<string> &) {
    HTTPServer s(new MyRequestHandlerFactory, ServerSocket(9090),
                 new HTTPServerParams);

    s.start();
    cout << endl << "Server started" << endl;

    waitForTerminationRequest(); // wait for CTRL-C or kill

    cout << endl << "Shutting down..." << endl;
    s.stop();

    return Application::EXIT_OK;
  }
};

int main(int argc, char **argv) {
  MonitorServerApp app;
  return app.run(argc, argv);
}

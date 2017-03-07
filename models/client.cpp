#include "client.h"
#include <Poco/Data/RecordSet.h>
#include <Poco/Data/SQLite/Connector.h>
#include <Poco/Data/Session.h>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;
using Poco::Data::RecordSet;

Client::Client() {
  this->machine = "";
  this->token = "";
}

Client::Client(string machine, string token) {
  this->machine = machine;
  this->token = token;
}

Client *Client::searchClient(string machine, string token) {
  try {
    Poco::Data::SQLite::Connector::registerConnector();
    // create a session
    Session session("SQLite", "system_monitor.db");
    Statement select(session);
    Client *c;
    select << "SELECT machine, token FROM Client";
    select.execute();
    RecordSet rs(select);
    rs.moveFirst();
    if (rs["machine"] == machine && rs["token"] == token) {
      c = new Client(rs["machine"], rs["token"]);
      cout << c->machine << " " << c->token << std::endl;
      return c;
    } else {
      return NULL;
    }
  } catch (Poco::Exception &error) {
    cout << "Error : " << error.displayText() << endl;
    return NULL;
  }
}

void Client::createClient() {
  // register SQLite connector
  try {
    Poco::Data::SQLite::Connector::registerConnector();
    // create a session
    Session session("SQLite", "system_monitor.db");
    // (re)create table
    session << "CREATE TABLE IF NOT EXISTS Client(machine VARCHAR(50), token "
               "VARCHAR(50))",
        now;
    Client *found = Client::searchClient(this->machine, this->token);
    if (found == NULL) {
      Statement insert(session);
      insert << "INSERT INTO Client VALUES(?, ?)", use(this->machine),
          use(this->token);
      insert.execute();
    }
  } catch (Poco::Exception &error) {
    cout << "Error : " << error.displayText() << endl;
  }
}

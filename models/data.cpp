#include "data.h"
#include "../helpers/send_email.h"
#include <Poco/Data/RecordSet.h>
#include <Poco/Data/SQLite/Connector.h>
#include <Poco/Data/Session.h>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;
using Poco::Data::RecordSet;

Data::Data() {
  this->machine = "";
  this->token = "";
  this->cpu_usage = 0;
  this->memory_usage = 0;
  this->disk_usage = 0;
  this->process_count = 0;
}

Data::Data(string machine, string token, double cpu_usage, double memory_usage,
           double disk_usage, int process_count) {
  this->machine = machine;
  this->token = token;
  this->cpu_usage = cpu_usage;
  this->memory_usage = memory_usage;
  this->disk_usage = disk_usage;
  this->process_count = process_count;
}

void Data::createRegistry() {
  // register SQLite connector
  try {
    Poco::Data::SQLite::Connector::registerConnector();
    // create a session
    Poco::Data::Session session("SQLite", "system_monitor.db");
    // (re)create table
    session << "CREATE TABLE IF NOT EXISTS Data(machine VARCHAR(50), token "
               "VARCHAR(50), cpu_usage decimal, memory_usage decimal, "
               "disk_usage decimal, process_count integer)",
        now;
    Statement insert(session);
    insert << "INSERT INTO DATA VALUES(?, ?,?,?,?,?)", use(this->machine),
        use(this->token), use(this->cpu_usage), use(this->memory_usage),
        use(this->disk_usage), use(this->process_count);
    insert.execute();
    alarm(this);
  } catch (Poco::Exception &error) {
    cout << "Error : " << error.displayText() << endl;
  }
}

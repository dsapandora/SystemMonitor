#include "../models/data.h"
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/MailMessage.h>
#include <Poco/Net/MailRecipient.h>
#include <Poco/Net/NetException.h>
#include <Poco/Net/SMTPClientSession.h>
#include <Poco/Net/SSLManager.h>
#include <Poco/Net/SecureSMTPClientSession.h>
#include <Poco/Net/SecureStreamSocket.h>
#include <Poco/Net/StringPartSource.h>
#include <Poco/URI.h>
#include <Poco/Util/AbstractConfiguration.h>
#include <Poco/Util/XMLConfiguration.h>
#include <algorithm>
#include <iostream>
#pragma once

using namespace Poco::Net;
using namespace std;
using namespace Poco::Util;
using Poco::Net::SSLManager;
using Poco::Net::SecureStreamSocket;
using Poco::Net::SocketAddress;
using Poco::Net::SecureSMTPClientSession;
using Poco::Net::SMTPClientSession;
using Poco::SharedPtr;

typedef struct  {
  string sSmtpServer;
  int nSmtpPort;
  string username;
  string password;
  string authentication;
  bool enable_starttls_auto;
  string sender;
} email_configuration;

email_configuration *get_email_configuration() {
  email_configuration *mail_configuration = new email_configuration();
  AbstractConfiguration *cfg = new XMLConfiguration("config.xml");
  mail_configuration->sSmtpServer = cfg->getString("email.smpt_server");
  mail_configuration->nSmtpPort = cfg->getInt("email.smpt_port");
  mail_configuration->username = cfg->getString("email.smpt_username");
  mail_configuration->password = cfg->getString("email.smpt_password");
  mail_configuration->authentication = cfg->getString("email.autentication");
  mail_configuration->enable_starttls_auto = cfg->getBool("email.smtp_use_ssl");
  return mail_configuration;
}

void doRequest(string recipient, string subject, string body) {
  email_configuration *configuration = get_email_configuration();
  string sSmtpServer = configuration->sSmtpServer;
  int nSmtpPort = configuration->nSmtpPort;
  string username = configuration->username;
  string password = configuration->password;
  string authentication = configuration->authentication;
  bool enable_starttls_auto = configuration->enable_starttls_auto;
  MailMessage message;
  message.setSender(configuration->sender);
  message.addRecipient(
      MailRecipient(MailRecipient::PRIMARY_RECIPIENT, recipient));
  message.setSubject(subject);
  std::string content;
  content += body;
  content += ",\r\n\r\n";
  message.addContent(new StringPartSource(content));
  message.setContentType("text/plain; charset=UTF-8");
  SMTPClientSession s(sSmtpServer);
  s.login(SMTPClientSession::AUTH_LOGIN, username, password);
  s.sendMessage(message);
  s.close();
}

string remove(char to_rem, string s) {
  size_t pos = s.find(to_rem);
  s.erase(pos, count(s.begin(), s.end(), to_rem));
  return s;
}

void alarm(Data *registry) {
  string token = registry->token;
  AbstractConfiguration *cfg = new XMLConfiguration("config.xml");
  string memory_limit_string = cfg->getString("client[@key='" + token +
                                              "']."
                                              "alert[@type='memory'][@limit]");
  double memory_limit = stod(remove('%', memory_limit_string));
  string cpu_limit_string = cfg->getString("client[@key='" + token +
                                           "']."
                                           "alert[@type='cpu'][@limit]");
  double cpu_limit = stod(remove('%', cpu_limit_string));
  int processes_limit = cfg->getInt("client[@key='" + token +
                                    "']."
                                    "alert[@type='processes'][@limit]");
  string message = "";
  if (registry->memory_usage >= memory_limit) {
    message += "\nMemory usage over limit " + to_string(registry->memory_usage);
  }
  if (registry->cpu_usage >= cpu_limit) {
    message += "\nCPU usage over limit " + to_string(registry->cpu_usage);
  }
  if (registry->process_count >= processes_limit) {
    message += "\nProcess over limit " + to_string(registry->process_count);
  }
  if (message != "") {
    string email = cfg->getString("client[@key='" + token + "'][@mail]");
    string subject = "Error in server " + registry->machine;
  }
}

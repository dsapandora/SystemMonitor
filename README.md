# SystemMonitor
Server/Client Application to monitor system statistics
1. *Database SQLITE*: Is installed in the server, an store the the client information about cpu, memory usage and running process, etc.

2. *Sengrid*: I don't want to install a Mail server to no tify my user so I will delegate the mail server to sengrid, and I will integrate it using SMTP.

3. *POCO*: Framework to create application as fast a posible. Poco allow to connect to different database just setting a connections stream.


```
curl -X POST -d '{"machine":"pepito", "cpu_usage":"45","memory_usage":"100", "disk_usage":"100"}' -H "Content-Type: application/json" 'http://localhost:9090'

```

The development will cover 4 points:

- [ X] Rest API Server with postgres database.

- [ X] Autentication Token for client installation.

- [X ] Schedule task for the client, client installation.

- [X ] Deploy Application


Compile the server
```
export POCO_HOME=/home/dsapandora/Downloads/poco-1.7.8-all
g++ -std=c++0x application_server.cpp models/*.cpp -L$POCO_HOME/lib/Linux/x86_64 -lPocoFoundation -lPocoNet -lPocoUtil -lPocoJSON -lPocoDataSQLite -lPocoData -lPocoXML
```

compile Client
The client collect every 5 minutes the system information.
The client can be installed in unix and windows.

```
export POCO_HOME=/home/dsapandora/Downloads/poco-1.7.8-all
g++ -std=c++11 client.cpp -L$POCO_HOME/lib/Linux/x86_64 -lPocoFoundation -lPocoNet -lPocoUtil -lPocoJSON -lPocoDataSQLite -lPocoData -lPocoXML

```

Almost all the application work using json instead of xml, and it was just for my personal taste.

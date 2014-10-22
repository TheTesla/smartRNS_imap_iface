#ifndef DNSPGDBIFACE_H_INCLUDED
#define DNSPGDBIFACE_H_INCLUDED

#include <iostream>
#include <postgresql/libpq-fe.h>
#include <string>
#include <algorithm>
#include <fstream>

using namespace std;

int isuri(string uri);
int adduri(PGconn *conn, string name, string content, string type =  "TXT", uint32_t ttl = 3600);
int deluris(PGconn *conn, string name);
int delall(PGconn *conn);




#endif // DNSPGDBIFACE_H_INCLUDED

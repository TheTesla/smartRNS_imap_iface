#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <vmime/vmime.hpp>
#include <vmime/platforms/posix/posixHandler.hpp>
#include <unistd.h>

#include <postgresql/libpq-fe.h>
#include <string>
#include <algorithm>
#include <fstream>
#include "DNSpgDBiface.h"
#include "imapiface.h"

using namespace std;


int writeDB(PGconn* conn, string cmdstr, string uristr, string contstr, string typestr, string ttlstr)
{
    if("add"==cmdstr){
        if(""==typestr){
            return adduri(conn, uristr, contstr);
        }else if(""==ttlstr){
            return adduri(conn, uristr, contstr, typestr);
        }else{
            return adduri(conn, uristr, contstr, typestr, atoi(ttlstr.c_str()));
        }
    }else if("del"==cmdstr){
        //cout << "DELETED: " << uristr << " (" << deluris(conn, uristr) << ")"<< endl;
        return deluris(conn, uristr);
    }
    return -3;
}

void writeDBv(PGconn* conn, string cmdstr, string uristr, string contstr, string typestr, string ttlstr)
{
    if("add"==cmdstr){
        cout << "ADD: " << uristr << " " << contstr << " " << typestr << " " << ttlstr << endl;
        writeDB(conn, cmdstr, uristr, contstr, typestr, ttlstr);
    }else if("del"==cmdstr){
        cout << "DELETED: " << uristr << " (" << writeDB(conn, cmdstr, uristr, contstr, typestr, ttlstr) << ")"<< endl;
    }
}

string getarg(stringstream &ss)
{
    string str, line;
    str = "";
    do{
    line = "";
        if(!getline(ss, line)) break;
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        str += line;
    } while('#' != line.back());
    if('#' == str.back()) str.pop_back();
    cout << str << endl;
    return str;
}

int main()
{
    PGconn          *conn;

    string host, dbname, user, passwd, mode;
    string cmdstr, uristr, contstr, ttlstr, typestr;
    fstream cf;
    string password;
    stringstream cmdss;
    vmime::utility::outputStreamAdapter cmd(cmdss);
    vmime::ref <vmime::net::message> msg;
    vmime::string urlstr;
    vmime::platform::setHandler<vmime::platforms::posix::posixHandler>();
    vmime::ref <vmime::net::store> st;

    cf.open("db.conf", std::fstream::in);
    cf >> host >> dbname >> user >> passwd;
    cf.close();

    cout << "Password: ";
    cin >> password;

    conn = PQconnectdb(("dbname="+dbname+" host="+host+" user="+user+" password="+passwd).c_str());
    if (PQstatus(conn) == CONNECTION_BAD) {
        cout << "-3 (We were unable to connect to the database)" << endl;
        return 0;
    }

    urlstr = "imaps://stefan:"+password+"@smartrns.net";
    std::cout << "Available protocols: " << findAvailableProtocols(vmime::net::service::TYPE_STORE) << std::endl;

    vmime::utility::url url(urlstr);

    st = g_session->getStore(url);
    // Enable TLS support if available
    st->setProperty("connection.tls", true);

    // Set the object responsible for verifying certificates, in the
    // case a secured connection is used (TLS/SSL)
	st->setCertificateVerifier(vmime::create <NonInteractiveCertificateVerifier>());

    st->connect();
    vmime::ref <vmime::net::connectionInfos> ci = st->getConnectionInfos();
    std::cout << std::endl;
    std::cout << "Connected to '" << ci->getHost() << "' (port " << ci->getPort() << ")" << std::endl;
    std::cout << "Connection is " << (st->isSecuredConnection() ? "" : "NOT ") << "secured." << std::endl;

    // Open the default folder in this store
    vmime::ref <vmime::net::folder> f = st->getRootFolder();
    f = f->getFolder(string("INBOX"));
    f = f->getFolder(string("smartrns"));
    f = f->getFolder(string("access"));
    f = f->getFolder(string("test"));

    int cnt = 0;
    int num = 1;
    delall(conn);
    while(1){

        f->open(vmime::net::folder::MODE_READ_WRITE);
        cnt = f->getMessageCount();
        std::cout << std::endl;
        std::cout << "Total number of messages: " << cnt << std::endl;

        for(;num<=cnt;num++){
            cmd.flush();
            cmdss.clear();
            cmdss.str("");
            cmdstr = uristr = contstr = typestr = ttlstr = "";
            msg = f->getMessage(num);
            msg->extract(cmd);

            cmdstr = getarg(cmdss);
            uristr = getarg(cmdss);
            contstr = getarg(cmdss);
            typestr = getarg(cmdss);
            ttlstr = getarg(cmdss);

            writeDBv(conn, cmdstr, uristr, contstr, typestr, ttlstr);
        }
        f->close(true);
        usleep(1000000);
    }
    PQfinish(conn);
    return 0;
}

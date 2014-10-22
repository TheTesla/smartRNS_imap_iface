
#include "DNSpgDBiface.h"

int isuri(string uri)
{
    if(std::string::npos != uri.find_first_not_of("1234567890abcdefghijklmnopqrstuvwxyz_-.")) return 0;
    return 1;
}


int adduri(PGconn *conn, string name, string content, string type, uint32_t ttl)
{

    PGresult *res;
    char *contesc;
    char *typesc;
    string id;
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);
    if(!isuri(name)) return -2;
    contesc = PQescapeLiteral(conn, content.c_str(), content.length());
    typesc = PQescapeLiteral(conn, type.c_str(), content.length());
    res = PQexec(conn, ("SELECT id FROM domains WHERE '" + name + "' LIKE CONCAT('%', name);").c_str());
    if(0 == PQntuples(res)) return -1;
    id = PQgetvalue(res, 0, 0);
    res = PQexec(conn, ("INSERT INTO records (domain_id, name, type, content, ttl) VALUES ('" + id + "', '" + name + "', " + typesc + ", " + contesc + ", '" + to_string(ttl) + "');").c_str());
    PQclear(res);
    PQfreemem(contesc);
    PQfreemem(typesc);
    return 1;

}

int deluris(PGconn *conn, string name)
{
    PGresult *res;
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);
    if(!isuri(name)) return -2;
    res = PQexec(conn, ("DELETE FROM records WHERE name = '" + name + "';").c_str());
    return atoi(PQcmdTuples(res));
}

int delall(PGconn *conn)
{
    PGresult *res;
    res = PQexec(conn, "DELETE FROM records;");
    return atoi(PQcmdTuples(res));
}


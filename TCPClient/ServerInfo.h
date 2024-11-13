#ifndef SERVERINFO_H
#define SERVERINFO_H

#include <QString>

class ServerInfo
{
public:
    static ServerInfo &getInstance()
    {
        static ServerInfo instance;
        return instance;
    }

    QString getServerAddress() const { return serverAddress; }
    quint16 getServerPort() const { return serverPort; }

    void setServerInfo(const QString &address, quint16 port)
    {
        serverAddress = address;
        serverPort = port;
    }

private:
    ServerInfo() {}
    ServerInfo(const ServerInfo&) = delete;
    ServerInfo& operator=(const ServerInfo&) = delete;

    QString serverAddress;
    quint16 serverPort;
};

#endif // SERVERINFO_H

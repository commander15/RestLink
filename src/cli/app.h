#ifndef APP_H
#define APP_H

#include <QCoreApplication>
#include <QTextStream>
#include <QCommandLineParser>

namespace RestLink {
class Api;
class Request;
class Body;
class Response;
}

class App : public QCoreApplication
{
    Q_OBJECT

public:
    App(int &argc, char *argv[]);

    void initParser();

    void setApi(RestLink::Api *api);

private slots:
    void run();

private:
    RestLink::Request makeRequest(const QString &option);
    RestLink::Body makeBody();
    void monitorResponse(RestLink::Response *response);

    QTextStream m_out;

    QCommandLineParser m_parser;

    int m_pendingRequestCount;
    RestLink::Api *m_api;
};

#endif // APP_H

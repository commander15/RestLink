#ifndef RESTLINK_SERVER_P_H
#define RESTLINK_SERVER_P_H

#include "server.h"

namespace RestLink {

class RESTLINK_EXPORT ServerPrivate
{
public:
    ServerPrivate(AbstractServerWorker *worker, Server *q);
    virtual ~ServerPrivate();

    Server *q_ptr;

    AbstractServerWorker *worker;

    friend class Server;
};

class DefaultServer : public Server
{
    Q_OBJECT

public:
    DefaultServer(const QString &name, const QStringList &schemes, AbstractServerWorker *worker, QObject *parent = nullptr)
        : Server(worker, parent)
        , m_name(name)
        , m_schemes(schemes)
    {}

    QString handlerName() const override
    { return m_name; }

    QStringList supportedSchemes() const override
    { return m_schemes; }

private:
    const QString m_name;
    const QStringList m_schemes;
};

} // namespace RestLink

#endif // RESTLINK_SERVER_P_H

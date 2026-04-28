#ifndef RESTLINK_SERVER_P_H
#define RESTLINK_SERVER_P_H

#include "server.h"

namespace RestLink {

class ServerPrivate
{
public:
    ServerPrivate(AbstractServerWorker *worker, Server *q);
    virtual ~ServerPrivate();

    Server *q_ptr;

    AbstractServerWorker *worker;

    friend class Server;
};

class DefaultServer final : public Server
{
    Q_OBJECT

public:
    DefaultServer(const QByteArray &id, const QString &name, const QStringList &schemes, AbstractServerWorker *worker, QObject *parent = nullptr)
        : Server(worker, parent)
        , m_id(id)
        , m_name(name)
        , m_schemes(schemes)
    {}

    QByteArray handlerId() const override
    { return m_id; }

    QString handlerName() const override
    { return m_name; }

    QStringList supportedSchemes() const override
    { return m_schemes; }

private:
    const QByteArray m_id;
    const QString m_name;
    const QStringList m_schemes;
};

} // namespace RestLink

#endif // RESTLINK_SERVER_P_H

#ifndef RESTLINK_PLUGIN_H
#define RESTLINK_PLUGIN_H

#include <RestLink/global.h>

#include <QtCore/qobject.h>
#include <QtCore/qjsonobject.h>

#define RESTLINK_PLUGIN_IID "com.restlink.plugin"

namespace RestLink {

class RequestHandler;

class RESTLINK_EXPORT Plugin : public QObject
{
    Q_OBJECT

public:
    explicit Plugin(QObject *parent = nullptr);
    virtual ~Plugin();

    QString name() const;

    QJsonObject metaData() const;
    void setMetaData(const QJsonObject &metaData);

    virtual RequestHandler *createHandler() = 0;

private:
    QJsonObject m_metaData;
};

} // namespace RestLink

Q_DECLARE_INTERFACE(RestLink::Plugin, RESTLINK_PLUGIN_IID)

#endif // RESTLINK_PLUGIN_H

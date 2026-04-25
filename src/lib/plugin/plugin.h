#ifndef RESTLINK_PLUGIN_H
#define RESTLINK_PLUGIN_H

#include <RestLink/global.h>

#include <QtCore/qobject.h>
#include <QtCore/qjsonobject.h>

#define RESTLINK_PLUGIN_IID "com.restlink.plugin"

namespace RestLink {

class AbstractRequestHandler;

class RESTLINK_EXPORT Plugin : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString uuid READ uuid CONSTANT FINAL)
    Q_PROPERTY(QString name READ name CONSTANT FINAL)
    Q_PROPERTY(QString version READ version CONSTANT FINAL)
    Q_PROPERTY(QString description READ description CONSTANT FINAL)
    Q_PROPERTY(QStringList supportedSchemes READ supportedSchemes CONSTANT FINAL)

public:
    explicit Plugin(QObject *parent = nullptr);

    QString uuid() const;
    QString name() const;
    QString description() const;
    virtual QString version() const;
    virtual QStringList supportedSchemes() const = 0;

    QJsonObject metaData() const;

protected:
    virtual ~Plugin();

    virtual AbstractRequestHandler *createHandler(QObject *parent) = 0;

private:
    void setMetaData(const QJsonObject &metaData);

    QJsonObject m_metaData;

    friend class PluginManagerPrivate;
    friend class PluginLoadHelper;
};

} // namespace RestLink

Q_DECLARE_INTERFACE(RestLink::Plugin, RESTLINK_PLUGIN_IID)

#endif // RESTLINK_PLUGIN_H

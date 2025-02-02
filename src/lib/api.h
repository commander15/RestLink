#ifndef RESTLINK_API_H
#define RESTLINK_API_H

#include <RestLink/global.h>
#include <RestLink/apibase.h>

#include <QtCore/qversionnumber.h>
#include <QtCore/qurl.h>
#include <QtCore/qlocale.h>

namespace RestLink {

class ApiPrivate;
class RESTLINK_EXPORT Api : public ApiBase
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged FINAL)
    Q_PROPERTY(QVersionNumber version READ version WRITE setVersion NOTIFY versionChanged FINAL)
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged FINAL)
    Q_PROPERTY(QLocale locale READ locale WRITE setLocale NOTIFY localeChanged FINAL)
    Q_PROPERTY(QString userAgent READ userAgent WRITE setUserAgent NOTIFY userAgentChanged FINAL)

public:
    explicit Api(QObject *parent = nullptr);
    ~Api();

    QString name() const;
    Q_SLOT void setName(const QString &name);
    Q_SIGNAL void nameChanged(const QString &name);

    QVersionNumber version() const;
    Q_SLOT void setVersion(const QVersionNumber &version);
    Q_SIGNAL void versionChanged(const QVersionNumber &version);

    QUrl url() const override;
    Q_SLOT void setUrl(const QUrl &url);
    Q_SIGNAL void urlChanged(const QUrl &url);

    QLocale locale() const override;
    Q_SLOT void setLocale(const QLocale &locale);
    Q_SIGNAL void localeChanged(const QLocale &locale);

    QString userAgent() const override;
    Q_SLOT void setUserAgent(const QString &agent);
    Q_SIGNAL void userAgentChanged(const QString &agent);

    Q_SLOT void configure(const QUrl &url);
    bool configure(const QJsonObject &config);
    Q_SIGNAL void configurationCompleted();
    Q_SIGNAL void configurationFailed();

protected:
    Response *send(Operation operation, const Request &request, const Body &body) override;

    friend class ApiPrivate;
};

}

#endif // RESTLINK_API_H

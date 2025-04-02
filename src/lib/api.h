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
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString version READ versionString WRITE setVersionString NOTIFY versionChanged)
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(QLocale locale READ locale WRITE setLocale NOTIFY localeChanged)
    Q_PROPERTY(QString bearerToken READ bearerToken WRITE setBearerToken NOTIFY bearerTokenChanged FINAL)
    Q_PROPERTY(QString userAgent READ userAgent WRITE setUserAgent NOTIFY userAgentChanged)

public:
    explicit Api(QObject *parent = nullptr);
    ~Api();

    QString name() const;
    Q_SLOT void setName(const QString &name);
    Q_SIGNAL void nameChanged(const QString &name);

    QVersionNumber version() const;
    Q_SLOT void setVersion(const QVersionNumber &version);
    Q_SIGNAL void versionChanged();

    QUrl url() const override;
    Q_SLOT void setUrl(const QUrl &url);
    Q_SIGNAL void urlChanged(const QUrl &url);

    QLocale locale() const override;
    Q_SLOT void setLocale(const QLocale &locale);
    Q_SIGNAL void localeChanged(const QLocale &locale);

    QString bearerToken() const;
    Q_SLOT void setBearerToken(const QString &token);
    Q_SIGNAL void bearerTokenChanged(const QString &token);

    QString userAgent() const override;
    Q_SLOT void setUserAgent(const QString &agent);
    Q_SIGNAL void userAgentChanged(const QString &agent);

    Q_SLOT void configure(const QUrl &url);
    bool configure(const QJsonObject &config);
    Q_SIGNAL void configurationCompleted();
    Q_SIGNAL void configurationFailed();

    Response *send(Operation operation, const Request &request, const Body &body) override;

private:
    inline QString versionString() const
    { return version().toString(); }

    inline void setVersionString(const QString &version)
    { setVersion(QVersionNumber::fromString(version)); }

    friend class ApiPrivate;
};

}

#endif // RESTLINK_API_H

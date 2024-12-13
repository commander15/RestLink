#ifndef RESTLINK_APIREQUEST_H
#define RESTLINK_APIREQUEST_H

#include <RestLink/global.h>

#include <QtCore/qshareddata.h>
#include <QtCore/qvariant.h>

namespace RestLink {
class ApiRequestParameter;

class ApiRequestPrivate;
class RESTLINK_EXPORT ApiRequest
{
public:
    ApiRequest();
    ApiRequest(const ApiRequest &other);
    ApiRequest(ApiRequest &&other);
    ~ApiRequest();

    ApiRequest &operator=(const ApiRequest &other);
    ApiRequest &operator=(ApiRequest &&other);

    QString endpoint() const;
    void setEndpoint(const QString &endpoint);

    bool hasParameter(const QString &name) const;
    ApiRequestParameter parameter(const QString &name) const;
    QStringList parameterList() const;
    QList<ApiRequestParameter> parameters() const;
    void addParameter(const ApiRequestParameter &parameter);
    void removeParameter(const QString &name);
    void setParameters(const QList<ApiRequestParameter> &parameters);

    QString contentType() const;
    void setContentType(const QString &type);

    bool isCacheable() const;
    void setCacheable(bool cache = true);

    QString urlPath() const;

    void loadFromJsonObject(const QJsonObject &object, QByteArray *data = nullptr);

    void swap(ApiRequest &other);

    bool operator==(const ApiRequest &other) const;
    inline bool operator!=(const ApiRequest &other) const
    {return !operator==(other);}

    static ApiRequest mergeRequests(const ApiRequest &r1, const ApiRequest &r2);

private:
    QSharedDataPointer<ApiRequestPrivate> d;

    friend class ApiBase;
};

class ApiRequestParameterPrivate;
class RESTLINK_EXPORT ApiRequestParameter
{
public:
    enum ParameterScope {
        UrlPathScope,
        UrlQueryScope,
        HeaderScope
    };

    enum ParameterFlag {
        NoFlag = 0x0,
        AuthenticationFlag = 0x1,
        SecretFlag = 0x2
    };
    Q_DECLARE_FLAGS(ParameterFlags, ParameterFlag)

    ApiRequestParameter();
    ApiRequestParameter(const QString &name, const QVariant &value, ParameterScope scope = UrlQueryScope);
    ApiRequestParameter(const ApiRequestParameter &other);
    ApiRequestParameter(ApiRequestParameter &&other);
    ~ApiRequestParameter();

    ApiRequestParameter &operator=(const ApiRequestParameter &other);
    ApiRequestParameter &operator=(ApiRequestParameter &&other);

    QString name() const;
    void setName(const QString &name);

    QVariant value() const;
    void setValue(const QVariant &value);

    ParameterScope scope() const;
    void setScope(ParameterScope scope);

    bool hasFlag(ParameterFlag flag) const;
    ParameterFlags flags() const;
    void setFlag(ParameterFlag flag, bool on = true);
    void setFlags(const ParameterFlags &flags);

    bool isEnabled() const;
    void setEnabled(bool enabled = true);

    void loadFromJsonObject(const QJsonObject &object);

    bool isValid() const;

    void swap(ApiRequestParameter &other);

    bool operator==(const ApiRequestParameter &other) const;
    inline bool operator!=(const ApiRequestParameter &other) const
    {return !operator==(other);}

    static ApiRequestParameter mergeParameters(const ApiRequestParameter &p1, const ApiRequestParameter &p2);

private:
    QSharedDataPointer<ApiRequestParameterPrivate> d;
};

}

#endif // RESTLINK_APIREQUEST_H

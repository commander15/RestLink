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
    enum DataType {
        RawData,
        FileData
    };

    enum RequestVerb {
        GetRequest,
        PostRequest,
        PutRequest,
        PatchRequest,
        DeleteRequest,
        UnknownRequest
    };

    ApiRequest();
    ApiRequest(const ApiRequest &other);
    ApiRequest(ApiRequest &&other);
    ~ApiRequest();

    ApiRequest &operator=(const ApiRequest &other);
    ApiRequest &operator=(ApiRequest &&other);

    QString endpoint(bool resolveParameters = false) const;
    void setEndpoint(const QString &endpoint);

    bool hasParameter(const QString &name) const;
    ApiRequestParameter parameter(const QString &name) const;
    QStringList parameterList() const;
    QList<ApiRequestParameter> parameters() const;
    void addParameter(const ApiRequestParameter &parameter);
    void removeParameter(const QString &name);
    void setParameters(const QList<ApiRequestParameter> &parameters);

    QByteArray data() const;
    DataType dataType() const;
    void setRawData(const QByteArray &data);
    void setFileName(const QString &fileName);
    void setData(const QByteArray &data, DataType type);

    RequestVerb verb() const;
    void setVerb(RequestVerb verb);

    void loadFromJsonObject(const QJsonObject &object);

    void swap(ApiRequest &other);

    bool operator==(const ApiRequest &other) const;
    inline bool operator!=(const ApiRequest &other) const
    {return !operator==(other);}

private:
    QSharedDataPointer<ApiRequestPrivate> d;

    friend class ApiPrivate;
};

class ApiRequestParameterPrivate;
class RESTLINK_EXPORT ApiRequestParameter
{
public:
    enum ParameterScope {
        UrlPathScope,
        UrlQueryScope,
        HeaderScope,
        UnknownScope
    };

    enum ParameterFlag {
        NoFlag = 0x0,
        AuthenticationFlag = 0x1,
        SecretFlag = 0x128
    };
    Q_DECLARE_FLAGS(ParameterFlags, ParameterFlag)

    ApiRequestParameter();
    ApiRequestParameter(const ApiRequestParameter &other);
    ApiRequestParameter(ApiRequestParameter &&other);
    ~ApiRequestParameter();

    ApiRequestParameter &operator =(const ApiRequestParameter &other);
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

private:
    QSharedDataPointer<ApiRequestParameterPrivate> d;
};

}

#endif // RESTLINK_APIREQUEST_H

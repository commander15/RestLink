#ifndef REQUESTINTERFACE_H
#define REQUESTINTERFACE_H

#include <RestLink/global.h>

#include <QtCore/qlist.h>

namespace RestLink {

class PathParameter;
class QueryParameter;
class Header;

class RESTLINK_EXPORT RequestInterface
{
public:
    bool hasPathParameter(const QString &name) const;
    PathParameter pathParameter(const QString &name) const;
    QVariant pathParameterValue(const QString &name) const;
    QStringList pathParameterNames() const;
    void setPathParameter(const QString &name, const QVariant &value);
    void setPathParameter(const PathParameter &parameter);
    void unsetPathParameter(const QString &name);

    QList<PathParameter> pathParameters() const;
    void setPathParameters(const QList<PathParameter> &parameters);

    bool hasQueryParameter(const QString &name) const;
    QueryParameter queryParameter(const QString &name) const;
    QList<QVariant> queryParameterValues(const QString &name) const;
    QStringList queryParameterNames() const;
    void addQueryParameter(const QString &name, const QVariant &value);
    void addQueryParameter(const QueryParameter &param);
    void removeQueryParameter(const QString &name);
    void removeQueryParameter(const QString &name, const QVariant &value);

    QList<QueryParameter> queryParameters() const;
    void setQueryParameters(const QList<QueryParameter> &parameters);

    bool hasHeader(const QString &name) const;
    Header header(const QString &name) const;
    QList<QVariant> headerValues(const QString &name) const;
    QStringList headerNames() const;
    void setHeader(const QString &name, const QVariant &value);
    void setHeader(const Header &header);
    void unsetHeader(const QString &name);

    QList<Header> headers() const;
    void setHeaders(const QList<Header> &headers);

protected:
    virtual const QList<PathParameter> *constPathParameters() const = 0;
    virtual QList<PathParameter> *mutablePathParameters() = 0;

    virtual const QList<QueryParameter> *constQueryParameters() const = 0;
    virtual QList<QueryParameter> *mutableQueryParameters() = 0;

    virtual const QList<Header> *constHeaders() const = 0;
    virtual QList<Header> *mutableHeaders() = 0;

private:
    QList<PathParameter>::const_iterator findPathParameter(const QString &name) const;
    QList<PathParameter>::iterator findPathParameter(const QString &name);

    QList<QueryParameter>::const_iterator findQueryParameter(const QString &name) const;
    QList<QueryParameter>::iterator findQueryParameter(const QString &name);

    QList<Header>::const_iterator findHeader(const QString &name) const;
    QList<Header>::iterator findHeader(const QString &name);
};

}

#endif // REQUESTINTERFACE_H

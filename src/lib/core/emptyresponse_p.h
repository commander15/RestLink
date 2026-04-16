#ifndef EMPTYRESPONSE_P_H
#define EMPTYRESPONSE_P_H

#include <RestLink/response.h>
#include <RestLink/private/response_p.h>

#include <QtNetwork/qnetworkrequest.h>

namespace RestLink {

class EmptyResponse final : public Response
{
    Q_OBJECT

public:
    explicit EmptyResponse(AbstractRequestHandler::Method method, QObject *parent = nullptr)
        : RestLink::Response(new ResponsePrivate(this), parent)
        , m_method(method)
    {}

public:
    AbstractRequestHandler::Method method() const override { return m_method; }
    bool isFinished() const override { return true; }
    int httpStatusCode() const override { return 500; }
    QString header(const QString &) const override { return QString(); }
    QStringList headerList() const override { return QStringList(); }
    QNetworkRequest networkRequest() const override { return netRequest; }
    QNetworkReply *networkReply() const override { return nullptr; }

    QNetworkRequest netRequest;

public slots:
    void abort() override {}

private:
    const AbstractRequestHandler::Method m_method;
};

} // namespace RestLink

#endif // EMPTYRESPONSE_P_H

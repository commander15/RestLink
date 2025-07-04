#include "request.h"

#include "utils.h"

#include <RestLink/request.h>
#include <RestLink/body.h>

namespace RestLink {
namespace Qml {

Request::Request(QObject *parent)
    : QObject(parent)
    , m_method(Get)
    , m_body(new Body(this))
    , m_autoRun(true)
    , m_response(nullptr)
    , m_api(nullptr)
{
}

QQmlListProperty<RequestParameter> Request::parameters()
{
    return QQmlListProperty<RequestParameter>(this, &m_parameters);
}

Body *Request::body() const
{
    return m_body;
}

bool Request::isRunning() const
{
    return (m_response ? m_response->isRunning() : false);
}

bool Request::isFinished() const
{
    return (m_response ? m_response->isFinished() : false);
}

Response *Request::response() const
{
    return m_response;
}

void Request::classBegin()
{
}

void Request::componentComplete()
{
    if (m_autoRun)
        run();
}

void Request::run()
{
    if (!m_api)
        return;

    if (m_response) {
        m_response->deleteLater();
        m_response = nullptr;
    }

    AbstractRequestHandler::Method method = static_cast<AbstractRequestHandler::Method>(m_method);

    RestLink::Request request(m_endpoint);
    Utils::fillParameters(&request, m_parameters);

    m_response = m_api->send(method, request, m_body->body());
    if (!m_response)
        return;

    connect(m_response, &Response::finished, this, &Request::finished);
    emit started();
}

Body::Body(QObject *parent)
    : QObject(parent)
    , m_contentType(Data)
    , m_dataType(RestLink::Body::RawData)
{
}

QVariant Body::data() const
{
    if (m_contentType == Data)
        return m_content;
    else
        return QString();
}

void Body::setData(const QVariant &data)
{
    if (m_content == data && m_contentType == Data)
        return;

    m_content = data;
    m_contentType = Data;
    emit contentChanged();
}

bool Body::hasJson() const
{
    return m_dataType == RestLink::Body::JsonData;
}

void Body::setJson(bool on)
{
    setDataType(RestLink::Body::JsonData);
}

QString Body::fileName() const
{
    if (m_contentType == File)
        return m_content.toString();
    else
        return QString();
}

void Body::setFileName(const QString &fileName)
{
    if (m_content == fileName && m_contentType == File)
        return;

    m_content = fileName;
    m_contentType = File;
    emit contentChanged();
}

RestLink::Body Body::body() const
{
    if (m_contentType == File)
        return RestLink::Body(RestLink::File(m_content.toString()));

    switch (m_dataType) {
    case RestLink::Body::JsonData:
        return RestLink::Body(QJsonDocument::fromJson(m_content.toByteArray()));
    }

    return RestLink::Body(m_content.toByteArray());
}

void Body::setDataType(int type)
{
    if (m_dataType != type) {
        m_dataType = type;
        emit dataTypeChanged();
    }
}

} // namespace Qml
} // namespace RestLink

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
    , m_parametersProperty(this, &m_parameters)
{
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
    switch (m_contentType) {
    case File:
        return RestLink::Body(RestLink::File(m_content.toString()));

    default:
        return RestLink::Body(m_content.toByteArray());
    }
}

} // namespace Qml
} // namespace RestLink

#include "request.h"

#include "utils.h"

#include <RestLink/request.h>
#include <RestLink/body.h>

namespace RestLink {
namespace Qml {

Request::Request(QObject *parent)
    : QObject(parent)
    , m_method(Get)
    , m_response(nullptr)
    , m_api(nullptr)
    , m_parametersProperty(this, &m_parameters)
{
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

    RequestHandler::Method method = static_cast<RequestHandler::Method>(m_method);

    RestLink::Request request(m_endpoint);
    Utils::fillParameters(&request, m_parameters);

    Body body(m_body);

    m_response = m_api->send(method, request, body);
    if (!m_response)
        return;

    connect(m_response, &Response::finished, this, &Request::finished);
    emit started();
}

} // namespace Qml
} // namespace RestLink

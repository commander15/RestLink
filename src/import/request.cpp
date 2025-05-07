#include "request.h"

#include "requestparameter.h"

#include <RestLink/request.h>
#include <RestLink/pathparameter.h>
#include <RestLink/queryparameter.h>
#include <RestLink/header.h>
#include <RestLink/body.h>
#include <RestLink/response.h>

namespace RestLink {
namespace Qml {

Request::Request(QObject *parent)
    : QObject(parent)
    , m_operation(Api::GetOperation)
    , m_response(nullptr)
    , m_api(nullptr)
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

void Request::fillParameters(RequestInterface *interface, QObject *request)
{
    const ParameterList parameters = Request::parameters(request);
    for (const Parameter &parameter : parameters) {
        switch (parameter.type()) {
        case Parameter::PathParameterType:
            interface->setPathParameter(parameter.toPathParameter());
            break;

        case Parameter::QueryParameterType:
            interface->addQueryParameter(parameter.toQueryParameter());
            break;

        case Parameter::HeaderType:
            interface->setHeader(parameter.toHeader());

        default:
            break;
        }
    }
}

ParameterList<> Request::parameters(QObject *request)
{
    ParameterList parameters;

    const QList<QObject *> children = request->children();
    for (QObject *child : children) {
        if (child->inherits("RestLink::Qml::RequestParameter")) {
            RequestParameter *p = static_cast<RequestParameter *>(child);
            if (p->isEnabled())
                parameters.addParameter(p->parameter());
        }
    }

    return parameters;
}

void Request::run()
{
    if (!m_api) {
        return;
    }

    if (m_response) {
        m_response->deleteLater();
        m_response = nullptr;
    }

    Api::Operation operation = static_cast<Api::Operation>(m_operation);

    RestLink::Request request(m_endpoint);
    fillParameters(&request, this);

    Body body(m_body);

    m_response = m_api->send(operation, request, body);
    if (!m_response)
        return;

    connect(m_response, &Response::finished, this, [this] {
        emit finished(m_response);
    });
}







} // namespace Qml
} // namespace RestLink

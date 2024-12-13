#include "restlinkapirequest.h"

#include <RestLink/api.h>

#include <QtNetwork/qnetworkreply.h>

#include <QtCore/qfile.h>
#include <QtCore/qbuffer.h>

RestLinkApiRequest::RestLinkApiRequest(QObject *parent) :
    QObject(parent),
    m_operation(RestLink::Api::GetOperation),
    m_reply(nullptr),
    m_api(nullptr)
{
}

RestLinkApiRequest::~RestLinkApiRequest()
{
}

QString RestLinkApiRequest::endpoint() const
{
    return m_endpoint;
}

void RestLinkApiRequest::setEndpoint(const QString &newEndpoint)
{
    if (m_endpoint == newEndpoint)
        return;

    m_endpoint = newEndpoint;
    emit endpointChanged();
}

QVariant RestLinkApiRequest::body() const
{
    return m_body;
}

void RestLinkApiRequest::setBody(const QVariant &data)
{
    if (m_body != data) {
        m_body = data;
        emit bodyChanged();
    }
}

int RestLinkApiRequest::operation() const
{
    return m_operation;
}

void RestLinkApiRequest::setOperation(int operation)
{
    if (m_operation != operation) {
        m_operation = static_cast<RestLink::Api::Operation>(operation);
        emit operationChanged();
    }
}

bool RestLinkApiRequest::isRunning() const
{
    if (m_reply)
        return !m_reply->isFinished();
    else
        return false;
}

void RestLinkApiRequest::run()
{
    if (!m_api || !m_api->isReady())
        return;

    if (m_reply)
        m_reply->deleteLater();

    RestLink::ApiRequest request;
    request.setEndpoint(m_endpoint);
    request.setParameters(apiRequestParameters(this));

    QFile *file = nullptr;

    if (m_body.typeId() == QMetaType::QUrl) {
        const QUrl url = m_body.toUrl();
        if (url.isLocalFile()) {
            file = new QFile(url.toLocalFile());
            if (!file->open(QIODevice::ReadOnly)) {
                delete file;
                file = nullptr;
            }
        }
    }

    switch (m_operation) {
    case RestLink::Api::GetOperation:
        m_reply = m_api->get(request);
        break;

    case RestLink::Api::PostOperation:
        if (file)
            m_reply = m_api->post(request, file);
        else
            m_reply = m_api->post(request, m_body.toByteArray());
        break;

    case RestLink::Api::PutOperation:
        if (file)
            m_reply = m_api->put(request, file);
        else
            m_reply = m_api->put(request, m_body.toByteArray());
        break;

    case RestLink::Api::PatchOperation:
        if (file)
            m_reply = m_api->patch(request, file);
        else
            m_reply = m_api->patch(request, m_body.toByteArray());
        break;

    case RestLink::Api::DeleteOperation:
        m_reply = m_api->deleteResource(request);
        break;
    }

    if (file)
        file->setParent(m_reply);

    emit responseChanged();

    connect(m_reply, &RestLink::ApiReply::finished, this, &RestLinkApiRequest::finished);
}

RestLink::ApiReply *RestLinkApiRequest::response() const
{
    return m_reply;
}

RestLinkApi *RestLinkApiRequest::api() const
{
    return m_api;
}

void RestLinkApiRequest::setApi(RestLinkApi *api)
{
    if (m_api != api) {
        m_api = api;
        emit apiChanged();

        if (api && api->isReady() && !m_reply)
            run();
    }
}

QList<RestLink::ApiRequestParameter> RestLinkApiRequest::apiRequestParameters(const QObject *request)
{
    QList<RestLink::ApiRequestParameter> parameters;
    for (QObject *child : request->children()) {
        if (child->inherits("RestLinkApiRequestParameter")) {
            RestLinkApiRequestParameter *param = static_cast<RestLinkApiRequestParameter *>(child);
            parameters.append(param->requestParameter());
        }
    }
    return parameters;
}

RestLinkApiRequestParameter::RestLinkApiRequestParameter(QObject *parent) :
    QObject(parent)
{
    connect(this, &RestLinkApiRequestParameter::nameChanged, this, &RestLinkApiRequestParameter::parameterChanged);
    connect(this, &RestLinkApiRequestParameter::valueChanged, this, &RestLinkApiRequestParameter::parameterChanged);
    connect(this, &RestLinkApiRequestParameter::scopeChanged, this, &RestLinkApiRequestParameter::parameterChanged);
    connect(this, &RestLinkApiRequestParameter::enabledChanged, this, &RestLinkApiRequestParameter::parameterChanged);
}

RestLinkApiRequestParameter::~RestLinkApiRequestParameter()
{
}

QString RestLinkApiRequestParameter::name() const
{
    return m_param.name();
}

void RestLinkApiRequestParameter::setName(const QString &name)
{
    if (m_param.name() != name) {
        m_param.setName(name);
        emit nameChanged();
    }
}

QVariant RestLinkApiRequestParameter::value() const
{
    return m_param.value();
}

void RestLinkApiRequestParameter::setValue(const QVariant &value)
{
    if (m_param.value() != value) {
        m_param.setValue(value);
        emit valueChanged();
    }
}

int RestLinkApiRequestParameter::scope() const
{
    return m_param.scope();
}

void RestLinkApiRequestParameter::setScope(int scope)
{
    if (m_param.scope() != scope) {
        m_param.setScope(static_cast<RestLink::ApiRequestParameter::ParameterScope>(scope));
        emit scopeChanged();
    }
}

bool RestLinkApiRequestParameter::isEnabled() const
{
    return m_param.isEnabled();
}

void RestLinkApiRequestParameter::setEnabled(bool enabled)
{
    if (m_param.isEnabled() != enabled) {
        m_param.setEnabled(enabled);
        emit enabledChanged();
    }
}

RestLink::ApiRequestParameter RestLinkApiRequestParameter::requestParameter() const
{
    return m_param;
}

#include "restlinkapirequest.h"

#include <RestLink/api.h>

#include <QtNetwork/qnetworkreply.h>

RestLinkApiRequest::RestLinkApiRequest(QObject *parent) :
    QObject(parent),
    m_type(RestLink::ApiRequest::GetRequest),
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

QString RestLinkApiRequest::data() const
{
    return m_data;
}

void RestLinkApiRequest::setData(const QString &data)
{
    if (m_data != data) {
        m_data = data;
        emit dataChanged(data);
    }
}

int RestLinkApiRequest::type() const
{
    return m_type;
}

void RestLinkApiRequest::setType(int type)
{
    if (m_type != type) {
        m_type = type;
        emit typeChanged(type);
    }
}

QString RestLinkApiRequest::response() const
{
    return m_reply->downloadedData();
}

bool RestLinkApiRequest::isRunning() const
{
    if (m_reply)
        return !m_reply->networkReply()->isFinished();
    else
        return false;
}

void RestLinkApiRequest::run()
{
    if (!m_api || !m_api->isReady()) {
        return;
    } else if (m_reply) {
        m_reply->deleteLater();
    }

    RestLink::ApiRequest request;
    request.setEndpoint(m_endpoint);
    request.setRawData(m_data.toUtf8());
    request.setVerb(static_cast<RestLink::ApiRequest::RequestVerb>(m_type));
    request.setParameters(apiRequestParameters(this));

    m_reply = m_api->run(request);
    connect(m_reply, &RestLink::ApiReply::finished, this, &RestLinkApiRequest::finished);

    emit apiReplyChanged();
}

RestLink::ApiReply *RestLinkApiRequest::apiReply() const
{
    return m_reply;
}

RestLinkApi *RestLinkApiRequest::api() const
{
    return m_api;
}

void RestLinkApiRequest::setApi(RestLinkApi *api)
{
    m_api = api;

    connect(api, &RestLinkApi::readyChanged, this, [api, this] {
        if (api->isReady() && !m_reply)
            run();
    });
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
        m_param.setScope(static_cast<RestLink::ApiRequestParameter::ApiRequestParameterScope>(scope));
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

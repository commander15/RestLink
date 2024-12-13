#include "api.h"
#include "api_p.h"

#include <RestLink/debug.h>
#include <RestLink/apirequest.h>
#include <RestLink/apireply.h>

#include <QtNetwork/qnetworkrequest.h>
#include <QtNetwork/qnetworkreply.h>
#include <QtNetwork/qsslsocket.h>

#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>

namespace RestLink {

Api::Api(QObject *parent) :
    ApiBase(new ApiPrivate(this), parent)
{
}

Api::~Api()
{
}

QString Api::apiName() const
{
    RESTLINK_D(const Api);
    return d->name;
}

void Api::setApiName(const QString &name)
{
    RESTLINK_D(Api);
    if (d->name != name) {
        d->name = name;
        emit apiNameChanged(name);
    }
}

int Api::apiVersion() const
{
    RESTLINK_D(const Api);
    return d->version;
}

void Api::setApiVersion(int version)
{
    RESTLINK_D(Api);
    if (d->version != version) {
        d->version = version;
        emit apiVersionChanged(version);
    }
}

QUrl Api::apiUrl() const
{
    RESTLINK_D(const Api);
    return d->url;
}

void Api::setApiUrl(const QUrl &url)
{
    RESTLINK_D(Api);

    if (d->url == url)
        return;

    d->url = url;
    emit apiUrlChanged(url);
}

ApiRequestParameter Api::apiParameter(int index) const
{
    RESTLINK_D(const Api);
    if (index < d->parameters.size())
        return d->parameters.at(index);
    else {
        restlinkWarning() << Q_FUNC_INFO << ": index is out of bound";
        return ApiRequestParameter();
    }
}

int Api::apiParameterCount() const
{
    RESTLINK_D(const Api);
    return d->parameters.size();
}

QList<ApiRequestParameter> Api::apiParameters() const
{
    RESTLINK_D(const Api);
    return d->parameters.toList();
}

int Api::addParameter(const ApiRequestParameter &parameter)
{
    RESTLINK_D(Api);

    int index = d->parameters.indexOf(parameter);

    if (index == -1) {
        d->parameters.append(parameter);
        index = d->parameters.size() - 1;
    } else
        d->parameters.replace(index, parameter);
    emit apiParametersChanged();

    return index;
}

void Api::addParameters(const QList<ApiRequestParameter> &parameters)
{
    RESTLINK_D(Api);
    if (!parameters.isEmpty()) {
        d->parameters.append(parameters.toVector());
        emit apiParametersChanged();
    }
}

void Api::setApiParameters(const QList<ApiRequestParameter> &parameters)
{
    RESTLINK_D(Api);
    const QList<ApiRequestParameter> currentParameters = d->parameters.toList();
    if (currentParameters == parameters)
        return;

    d->parameters = parameters.toVector();
    emit apiParametersChanged();
}

QString Api::userAgent() const
{
    RESTLINK_D(const Api);
    return d->userAgent;
}

void Api::setUserAgent(const QString &agent)
{
    RESTLINK_D(Api);
    if (d->userAgent != agent) {
        d->userAgent = agent;
        emit userAgentChanged(agent);
    }
}

void Api::configureApi(const QUrl &url)
{
    RESTLINK_D(Api);

    QNetworkRequest request(url);
    request.setPriority(QNetworkRequest::HighPriority);
    request.setTransferTimeout(3000);

    ApiReply *reply = new ApiReply(this);
    reply->setNetworkReply(d->netMan()->get(request));
    connect(reply, &ApiReply::finished, this, [reply, this] {
        if (configureApi(reply->readJsonObject())) {
#ifdef RESTLINK_DEBUG
            restlinkInfo() << "API configured from " << reply->networkReply()->url().toString();
#endif
            emit configurationCompleted();
        } else {
            emit configurationFailed();
        }

        reply->deleteLater();
    });
}

bool Api::configureApi(const QJsonObject &config)
{
    if (!config.contains("name") || !config.contains("version") || !config.contains("url"))
        return false;

    RESTLINK_D(Api);

    setApiName(config.value("name").toString());
    setApiVersion(config.value("version").toInt());
    setApiUrl(config.value("url").toString());

    const QJsonArray params = config.value("parameters").toArray();
    for (const QJsonValue &param : params) {
        ApiRequestParameter parameter;
        parameter.loadFromJsonObject(param.toObject());
        addParameter(parameter);
    }

    const QJsonArray requestArray = config.value("requests").toArray();
    for (const QJsonValue &requestValue : requestArray) {
        const QJsonObject requestObject = requestValue.toObject();
        ApiPrivate::RemoteRequest remote;
        remote.request.loadFromJsonObject(requestObject, &remote.data);
        d->remoteRequests.append(remote);
    }

#ifdef RESTLINK_D
    restlinkInfo() << "API '" << d->name << "' version " << d->version << " configured with "
                   << d->parameters.size() << " parameter(s) and "
                   << d->remoteRequests.size() << " remote request(s)";
#endif
    return true;
}

ApiReply *Api::createApiReply(const ApiRequest &request, QNetworkReply *netReply)
{
    ApiReply *apiReply = new ApiReply(this);
    apiReply->setApiRequest(request);
    apiReply->setNetworkReply(netReply);

#ifdef RESTLINK_DEBUG
    restlinkInfo() << d_ptr->httpVerbFromOperation(apiReply->operation())
                   << ' ' << apiReply->url().toString();
#endif

    return apiReply;
}

QNetworkRequest Api::createNetworkRequest(const ApiRequest &request, const void *data, DataType dataType)
{
    RESTLINK_D(Api);

    if (d->hasRemoteRequest(request)) {
        const ApiRequest remote = d->remoteRequest(request);
        return ApiBase::createNetworkRequest(ApiRequest::mergeRequests(request, remote), data, dataType);
    } else {
        return ApiBase::createNetworkRequest(request, data, dataType);
    }
}

void Api::processSslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
#ifdef RESTLINK_DEBUG
        reply->ignoreSslErrors(errors);
#endif

    for (const QSslError &error : errors)
        restlinkWarning() << "SSL error: " << error.errorString();
}

ApiPrivate::ApiPrivate(Api *qq) :
    ApiBasePrivate(qq),
    version(0),
    userAgent(QStringLiteral("libRestLink/") + QStringLiteral(RESTLINK_VERSION_STR))
{
    if (!QSslSocket::supportsSsl())
        restlinkWarning() << "SSL support not found";
}

void ApiPrivate::registerNetworkManager(QNetworkAccessManager *manager)
{
    QObject::connect(manager, &QNetworkAccessManager::sslErrors, q, &Api::processSslErrors);
}

bool ApiPrivate::hasRemoteRequest(const ApiRequest &request) const
{
    auto it = std::find_if(remoteRequests.cbegin(), remoteRequests.cend(), [&request](const RemoteRequest &r) {
        return r.request.endpoint() == request.endpoint();
    });

    return (it != remoteRequests.end());
}

ApiRequest ApiPrivate::remoteRequest(const ApiRequest &request) const
{
    auto it = std::find_if(remoteRequests.cbegin(), remoteRequests.cend(), [&request](const RemoteRequest &r) {
        return r.request.endpoint() == request.endpoint();
    });

    if (it != remoteRequests.cend())
        return it->request;
    else
        return ApiRequest();
}

QByteArray ApiPrivate::remoteRequestData(const ApiRequest &request) const
{
    auto it = std::find_if(remoteRequests.cbegin(), remoteRequests.cend(), [&request](const RemoteRequest &r) {
        return r.request.endpoint() == request.endpoint();
    });

    if (it != remoteRequests.cend())
        return it->data;
    else
        return QByteArray();
}

}

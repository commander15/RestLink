#include "api.h"
#include "api_p.h"

#include <RestLink/debug.h>
#include <RestLink/apiconfigurationdownload.h>
#include <RestLink/apirequest.h>
#include <RestLink/private/apirequest_p.h>

#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtNetwork/qnetworkrequest.h>
#include <QtNetwork/qnetworkreply.h>
#include <QtNetwork/qsslsocket.h>

#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>
#include <QtCore/qurl.h>
#include <QtCore/qurlquery.h>
#include <QtCore/qfile.h>

namespace RestLink {

Api::Api(QObject *parent) :
    QObject(parent),
    d(new ApiPrivate(this))
{
    d->netMan = new QNetworkAccessManager(this);
    d->netMan->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
    d->registerNetworkManager(d->netMan);
}

Api::Api(QNetworkAccessManager *netMan, QObject *parent) :
    QObject(parent),
    d(new ApiPrivate(this))
{
    d->netMan = netMan;
    d->registerNetworkManager(netMan);
}

Api::~Api()
{
}

QString Api::apiName() const
{
    return d->name;
}

void Api::setApiName(const QString &name)
{
    if (d->name != name) {
        d->name = name;
        emit apiNameChanged(name);
    }
}

int Api::apiVersion() const
{
    return d->version;
}

void Api::setApiVersion(int version)
{
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

ApiReply *Api::run(const ApiRequest &request)
{
    d->logRequest(request);

    const QByteArray verb = d->httpVerbFromRequestVerb(request.verb());
    QNetworkRequest netReq = createNetworkRequest(request);
    QNetworkReply *netReply = createNetworkReply(verb, netReq, request.data(), request.dataType());

    ApiReply *reply = new ApiReply(this);
    reply->setApiRequest(request);
    reply->setNetworkReply(netReply);
    return reply;
}

ApiRequestParameter Api::apiParameter(int index) const
{
    if (index < d->parameters.size())
        return d->parameters.at(index);
    else {
        restlinkWarning() << Q_FUNC_INFO << ": index is out of bound";
        return ApiRequestParameter();
    }
}

int Api::apiParameterCount() const
{
    return d->parameters.size();
}

QList<ApiRequestParameter> Api::apiParameters() const
{
    RESTLINK_D(const Api);
    return d->parameters.toList();
}

int Api::addParameter(const ApiRequestParameter &parameter)
{
    d->parameters.append(parameter);
    emit apiParametersChanged();
    return d->parameters.size() - 1;
}

void Api::addParameters(const QList<ApiRequestParameter> &parameters)
{
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

bool Api::canConfigureApi(const QJsonObject &config) const
{
    return config.contains("name")
            && config.contains("version")
            && config.contains("url");
}

void Api::configureApi(const QUrl &url)
{
    ApiConfigurationDownload *download = downloadApiConfiguration(url);
    download->enableAutoconfiguration();
    connect(download, &ApiReply::finished, download, &QObject::deleteLater);
}

void Api::configureApi(const QJsonObject &config)
{
    if (!canConfigureApi(config))
        return;

    RESTLINK_D(Api);

    setApiName(config.value("name").toString());
    setApiVersion(config.value("version").toInt());
    setApiUrl(config.value("url").toString());

    d->parameters.clear();
    emit apiParametersChanged();

    const QJsonArray params = config.value("parameters").toArray();
    for (const QJsonValue &param : params) {
        ApiRequestParameter parameter;
        parameter.loadFromJsonObject(param.toObject());
        addParameter(parameter);
    }

    d->requests.clear();

    const QJsonArray requests = config.value("requests").toArray();
    for (const QJsonValue &req : requests) {
        ApiRequest request;
        request.loadFromJsonObject(req.toObject());
        d->requests.append(request);
    }

#ifdef RESTLINK_D
    restlinkInfo() << "API '" << d->name << "' version " << d->version << " configured with "
                   << d->parameters.size() << " parameter(s) and "
                   << d->requests.size() << " remote request(s)";
#endif

    emit apiConfigured();
}

ApiConfigurationDownload *Api::downloadApiConfiguration(const QUrl &url)
{
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, d->userAgent);
    request.setRawHeader("Accept", "application/json");

    ApiConfigurationDownload *download = new ApiConfigurationDownload(this);
    download->setNetworkReply(d->netMan->get(request));
    return download;
}

QNetworkAccessManager *Api::networkAccessManager() const
{
    return d->netMan;
}

void Api::processSslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
#ifdef RESTLINK_DEBUG
        reply->ignoreSslErrors(errors);
#endif

    for (const QSslError &error : errors)
        restlinkWarning() << "SSL error: " << error.errorString();
}

QNetworkRequest Api::createNetworkRequest(const ApiRequest &request)
{
    QNetworkRequest netReq(d->requestUrl(request, true));
    netReq.setHeader(QNetworkRequest::UserAgentHeader, d->userAgent);
    netReq.setRawHeader("Connection", "keep-alive");

    const QList<ApiRequestParameter> parameters = d->requestParameters(request, ApiPrivate::HeaderContext);
    for (const ApiRequestParameter &parameter : parameters)
        netReq.setRawHeader(parameter.name().toUtf8(), parameter.value().toByteArray());

    return netReq;
}

QNetworkReply *Api::createNetworkReply(const QByteArray &verb, const QNetworkRequest &request, const QByteArray &data, int dataType)
{
    QNetworkReply *reply = nullptr;

    if (dataType == ApiRequest::RawData)
        reply = d->netMan->sendCustomRequest(request, verb, data);
    else if (dataType == ApiRequest::FileData) {
        QFile *file = new QFile(data);
        if (file->open(QIODevice::ReadOnly)) {
            reply = d->netMan->sendCustomRequest(request, verb, file);
            file->setParent(reply);
        } else {
            delete file;
            file = nullptr;
        }
    }

    return reply;
}

ApiPrivate::ApiPrivate(Api *qq) :
    q(qq),
    version(0),
    userAgent(QStringLiteral("libRestLink/") + QStringLiteral(RESTLINK_VERSION_STR)),
    netMan(nullptr)
{
    if (!QSslSocket::supportsSsl())
        restlinkWarning() << "SSL support not found";
}

ApiPrivate::~ApiPrivate()
{
}

void ApiPrivate::registerNetworkManager(QNetworkAccessManager *manager)
{
    QObject::connect(manager, &QNetworkAccessManager::sslErrors, q, &Api::processSslErrors);
}

void ApiPrivate::logRequest(const ApiRequest &request)
{
    restlinkInfo() << httpVerbFromRequestVerb(request.verb())
                   << ' ' << requestUrl(request, false).toString();
}

QUrl ApiPrivate::requestUrl(const ApiRequest &request, bool includeSecrets) const
{
    QString path = request.endpoint(true);
    QUrlQuery query;

    const QList<ApiRequestParameter> parameters = requestParameters(request, UrlContext);
    for (const ApiRequestParameter &parameter : parameters) {
        int useable = 0;
        useable += isUseableParameter(parameter, includeSecrets);
        useable += (parameter.scope() == ApiRequestParameter::UrlQueryScope);
        if (useable == 2)
            query.addQueryItem(parameter.name(), parameter.value().toByteArray());
    }

    QUrl url = this->url;
    url.setPath(url.path() + path);
    url.setQuery(query);
    return url;
}

QList<ApiRequestParameter> ApiPrivate::requestParameters(const ApiRequest &request, ParameterContext context) const
{
    ApiRequest finalRequest;

    QVector<ApiRequestParameter> parameters;
    parameters.append(this->parameters);
    parameters.append(remoteRequest(request).d->parameters);
    parameters.append(request.d->parameters);

    for (const ApiRequestParameter &parameter : parameters) {
        if (!isParameterMatchContext(parameter, context)) {
            continue;
        } else {
            finalRequest.addParameter(parameter);
        }
    }

    return finalRequest.parameters();
}

ApiRequest ApiPrivate::remoteRequest(const ApiRequest &request) const
{
    for (const ApiRequest &req : requests)
        if (req.endpoint() == request.endpoint())
            return req;
    return ApiRequest();
}

bool ApiPrivate::isUseableParameter(const ApiRequestParameter &parameter, bool secret) const
{
    if (parameter.hasFlag(ApiRequestParameter::SecretFlag) && !secret)
        return false;

    return parameter.isValid() && parameter.isEnabled();
}

bool ApiPrivate::isParameterMatchContext(const ApiRequestParameter &parameter, ParameterContext context) const
{
    switch (parameter.scope()) {
    case ApiRequestParameter::UrlPathScope:
    case ApiRequestParameter::UrlQueryScope:
        return context == UrlContext;

    case ApiRequestParameter::HeaderScope:
    default:
        return context == HeaderContext;
    }
}

QByteArray ApiPrivate::httpVerbFromRequestVerb(int verb)
{
    switch (verb) {
    case ApiRequest::GetRequest:
        return QByteArrayLiteral("GET");

    case ApiRequest::PostRequest:
        return QByteArrayLiteral("POST");

    case ApiRequest::PutRequest:
        return QByteArrayLiteral("PUT");

    case ApiRequest::PatchRequest:
        return QByteArrayLiteral("PATCH");

    case ApiRequest::DeleteRequest:
        return QByteArrayLiteral("DELETE");

    default:
        return QByteArray();
    }
}

}

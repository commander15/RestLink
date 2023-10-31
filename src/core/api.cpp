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
    const QByteArray verb = d->httpVerbFromRequestVerb(request.verb());
    QNetworkRequest netReq = createNetworkRequest(request);
    QNetworkReply *netReply = createNetworkReply(verb, netReq, request.data(), request.dataType());

    ApiReply *reply = new ApiReply(this);
    reply->setApiRequest(request);
    reply->setNetworkReply(netReply);
    return reply;
}

QList<ApiRequestParameter> Api::apiParameters() const
{
    RESTLINK_D(const Api);
    return d->parameters.toList();
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
    const QJsonArray params = config.value("parameters").toArray();
    for (const QJsonValue &value : params) {
        const QJsonObject param = value.toObject();

        ApiRequestParameter parameter;
        parameter.setName(param.value("name").toString());
        parameter.setValue(param.value("value").toVariant());

        const QString scope = param.value("scope").toString();
        if (scope == "URL_PATH")
            parameter.setScope(ApiRequestParameter::UrlPathScope);
        else if (scope == "URL_QUERY")
            parameter.setScope(ApiRequestParameter::UrlQueryScope);
        else if (scope == "HEADER")
            parameter.setScope(ApiRequestParameter::HeaderScope);

        if (param.contains("enabled"))
            parameter.setEnabled(param.value("enabled").toBool());

        d->parameters.append(parameter);
    }

#ifdef RESTLINK_D
    restlinkInfo() << d->name << " API version " << d->version << " configured with "
                   << d->parameters.size() << " parameter(s)";
#endif

    emit apiConfigured();
}

ApiConfigurationDownload *Api::downloadApiConfiguration(const QUrl &url)
{
    QNetworkRequest request(url);
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
    if (true)
        reply->ignoreSslErrors(errors);

    for (const QSslError &error : errors)
        restlinkWarning() << "SSL error: " << error.errorString();
}

QNetworkRequest Api::createNetworkRequest(const ApiRequest &request)
{
    QNetworkRequest netReq;
    netReq.setHeader(QNetworkRequest::UserAgentHeader, userAgent());
    netReq.setRawHeader("Accept", "application/json");
    netReq.setRawHeader("Connection", "keep-alive");

    QUrl url(apiUrl());
    url.setPath(url.path() + request.endpoint());

    QUrlQuery urlQuery(url.query());

    const QVector<ApiRequestParameter> parameters = d->parameters + request.d->parameters;
    for (const ApiRequestParameter &parameter : parameters) {
        if (!parameter.isEnabled() || !parameter.isValid())
            continue;

        switch (parameter.scope()) {
        case ApiRequestParameter::UrlPathScope:
            url.setPath(url.path() + '/' + parameter.value().toString());
            break;

        case ApiRequestParameter::UrlQueryScope:
            urlQuery.addQueryItem(parameter.name(), parameter.value().toString());
            break;

        case ApiRequestParameter::HeaderScope:
            netReq.setRawHeader(parameter.name().toUtf8(), parameter.value().toByteArray());
            break;
        }
    }

    url.setQuery(urlQuery);
    netReq.setUrl(url);
    return netReq;
}

QNetworkReply *Api::createNetworkReply(const QByteArray &verb, const QNetworkRequest &request, const QByteArray &data, int dataType)
{
    restlinkInfo() << verb << ' ' << request.url().toString();

    if (dataType == ApiRequest::RawData)
        return d->netMan->sendCustomRequest(request, verb, data);
    else if (dataType == ApiRequest::FileData) {
        QFile *file = new QFile(data);
        if (!file->open(QIODevice::ReadOnly)) {
            delete file;
            file = nullptr;
        }

        QNetworkReply *reply = d->netMan->sendCustomRequest(request, verb, file);
        if (file)
            file->setParent(reply);
        return reply;
    } else
        return nullptr;
}

ApiPrivate::ApiPrivate(Api *qq) :
    q(qq),
    version(0),
    userAgent(QStringLiteral("libRestLink/") + QStringLiteral(RESTLINK_VERSION_STR)),
    netMan(nullptr)
{
    if (!QSslSocket::supportsSsl())
        restlinkWarning() << "SSL support not found";
    else
        restlinkInfo() << "SSL " << QSslSocket::sslLibraryVersionString();
}

ApiPrivate::~ApiPrivate()
{
}

void ApiPrivate::registerNetworkManager(QNetworkAccessManager *manager)
{
    QObject::connect(manager, &QNetworkAccessManager::sslErrors, q, &Api::processSslErrors);
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

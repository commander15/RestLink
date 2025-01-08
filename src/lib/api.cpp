#include "api.h"
#include "api_p.h"
#include "body.h"

#include <RestLink/debug.h>
#include <RestLink/request.h>
#include <RestLink/response.h>
#include <RestLink/networkmanager.h>

#include <QtNetwork/qnetworkrequest.h>
#include <QtNetwork/qnetworkreply.h>
#include <QtNetwork/qsslsocket.h>

#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>

namespace RestLink {

/*!
 * \class RestLink::Api
 * \brief Represents an API interface that manages configuration, requests, and parameters.
 *
 * The Api class is responsible for creating network requests and managing
 * response data. It supports adding parameters to requests, configuring the API from different sources,
 * and emitting signals for configuration changes and network error events.
 *
 * \property QString Api::name
 * \brief The name of the API.
 *
 * \property QVersionNumber Api:version
 * \brief The version of the API.
 *
 * \property QUrl Api:url
 * \brief The base URL of the API.
 *
 * \property QString Api::userAgent
 * \brief The user agent string to be used in API requests.
 *
 * \signal void Api::nameChanged(const QString &name)
 * \brief Emitted when the API name changes.
 *
 * \signal void Api::versionChanged(int version)
 * \brief Emitted when the API version changes.
 *
 * \signal void Api::urlChanged(const QUrl &url)
 * \brief Emitted when the API URL changes.
 *
 * \signal void Api::userAgentChanged(const QString &agent)
 * \brief Emitted when the user agent changes.
 *
 * \signal void Api::configurationCompleted()
 * \brief Emitted when the API configuration is completed.
 *
 * \signal void Api::configurationFailed()
 * \brief Emitted when the API configuration fails.
 *
 * \see ApiBase, Request, RequestParameter
 */

/*!
 * \brief Constructs an Api object with an optional parent.
 *
 * This constructor initializes the API object, setting the parent if provided. The API object is ready to be configured and used to make requests.
 * The parent object is passed to the QObject constructor, which manages the object's memory and lifecycle.
 *
 * \param parent The parent QObject, or nullptr if no parent is specified.
 */
Api::Api(QObject *parent) :
    ApiBase(new ApiPrivate(this), parent)
{
}

Api::~Api()
{
}

/*!
 * \brief Returns the name of the API.
 *
 * This method retrieves the current name of the API as a string. The API name typically represents the identity or label associated with the API.
 *
 * \return A QString containing the name of the API.
 */
QString Api::name() const
{
    RESTLINK_D(const Api);
    return d->name;
}

/*!
 * \brief Sets the name of the API.
 *
 * This method sets a new name for the API. The API name is typically used for identification or labeling purposes.
 *
 * \param name The new name of the API.
 */
void Api::setName(const QString &name)
{
    RESTLINK_D(Api);
    if (d->name != name) {
        d->name = name;
        emit nameChanged(name);
    }
}

/*!
 * \brief Returns the version of the API.
 *
 * This method retrieves the current version of the API. The version is represented as an QVersionNumber and can be used to manage different versions of the API.
 *
 * \return The integer version number of the API.
 */
QVersionNumber Api::version() const
{
    RESTLINK_D(const Api);
    return d->version;
}

/*!
 * \brief Sets the version of the API.
 *
 * This method sets a new version number for the API. The version number is typically used to track API updates and compatibility.
 *
 * \param version The new version number for the API.
 */
void Api::setVersion(const QVersionNumber &version)
{
    RESTLINK_D(Api);
    if (d->version != version) {
        d->version = version;
        emit versionChanged(version);
    }
}

/*!
 * \brief Returns the URL of the API.
 *
 * This method retrieves the current URL of the API, which is used to define the endpoint for API requests.
 *
 * \return A QUrl representing the API URL.
 */
QUrl Api::url() const
{
    RESTLINK_D(const Api);
    return d->url;
}

/*!
 * \brief Sets the URL of the API.
 *
 * This method sets a new URL for the API, which will be used for making API requests.
 *
 * \param url The new URL of the API.
 */
void Api::setUrl(const QUrl &url)
{
    RESTLINK_D(Api);

    if (d->url == url)
        return;

    d->url = url;
    emit urlChanged(url);
}

QLocale Api::locale() const
{
    RESTLINK_D(const Api);
    return d->locale;
}

void Api::setLocale(const QLocale &locale)
{
    RESTLINK_D(Api);
    if (d->locale != locale) {
        d->locale = locale;
        emit localeChanged(locale);
    }
}

/*!
 * \brief Returns the user agent for the API.
 *
 * This method retrieves the current user agent string used for API requests. The user agent identifies the client software making the request.
 *
 * \return A QString representing the user agent for the API.
 */
QString Api::userAgent() const
{
    RESTLINK_D(const Api);
    return d->userAgent;
}

/*!
 * \brief Sets the user agent for the API.
 *
 * This method sets the user agent string used for API requests. The user agent identifies the client software making the request.
 *
 * \param agent The user agent string to be used for the API.
 */
void Api::setUserAgent(const QString &agent)
{
    RESTLINK_D(Api);
    if (d->userAgent != agent) {
        d->userAgent = agent;
        emit userAgentChanged(agent);
    }
}

/*!
 * \brief Configures the API with the given URL.
 *
 * This method configures the API by setting the URL to the given one, which is used for making API requests.
 *
 * \param url The URL to be set for the API.
 */
void Api::configure(const QUrl &url)
{
    RESTLINK_D(Api);

    QNetworkRequest request = d_ptr->netMan()->createNetworkRequest(GetOperation, Request(), Body(), this);
    request.setUrl(url);
    request.setPriority(QNetworkRequest::HighPriority);
    request.setTransferTimeout(3000);
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferNetwork);
    request.setAttribute(QNetworkRequest::CacheSaveControlAttribute, true);

    Response *reply = Response::create(static_cast<QNetworkAccessManager *>(d->netMan())->get(request), this);
    connect(reply, &Response::finished, this, [reply, this] {
        if (reply->isSuccess() && configure(reply->readJsonObject())) {
#ifdef RESTLINK_DEBUG
            restlinkInfo() << "API configured from " << reply->networkReply()->url().toString();
#endif
            emit configurationCompleted();
        } else {
            emit configurationFailed();

            if (reply->hasNetworkError())
                restlinkWarning() << reply->networkErrorString();
            else if (!reply->isHttpStatusSuccess())
                restlinkWarning() << reply->httpReasonPhrase();
            else
                restlinkWarning() << "Unknown error during configuration file download";
        }

        reply->deleteLater();
    });
}

/*!
 * \brief Configures the API with the given JSON configuration.
 *
 * This method configures the API using a JSON object. The configuration can include parameters like the API URL, version, and other settings.
 *
 * \param config A QJsonObject containing the configuration data for the API.
 * \return Returns true if the configuration was successful, false otherwise.
 */
bool Api::configure(const QJsonObject &config)
{
    if (!config.contains("name") || !config.contains("version") || !config.contains("url"))
        return false;

    RESTLINK_D(Api);

    if (config.contains("name"))
        setName(config.value("name").toString());

    if (config.contains("version")) {
        const QJsonValue version = config.value("version");
        switch (version.type()) {
        case QJsonValue::String:
            setVersion(QVersionNumber::fromString(version.toString()));
            break;

        case QJsonValue::Double:
            setVersion(QVersionNumber(version.toInt()));
            break;

        default:
            setVersion(QVersionNumber(1));
        }
    }

    if (config.contains("url"))
        setUrl(config.value("url").toString());

    d_ptr->internalRequest = Request::fromJsonbject(config);

    d->remoteRequests.clear();
    if (config.contains("requests")) {
        const QJsonArray requestArray = config.value("requests").toArray();
        for (const QJsonValue &requestValue : requestArray) {
            const QJsonObject requestObject = requestValue.toObject();
            ApiPrivate::RemoteRequest remote;
            remote.request = Request::fromJsonbject(requestObject);
            d->remoteRequests.append(remote);
        }
    }

#ifdef RESTLINK_DEBUG
    restlinkInfo() << "API '" << d->name << "' version " << d->version.toString() << " configured !";
#endif
    return true;
}

Response *Api::send(Operation operation, const Request &request, const Body &body)
{
    RESTLINK_D(Api);

    if (d->hasRemoteRequest(request))
        return ApiBase::send(operation, Request::merge(request, d->remoteRequest(request)), body);
    else
        return ApiBase::send(operation, request, body);
}

ApiPrivate::ApiPrivate(Api *qq) :
    ApiBasePrivate(qq),
    version(0),
    locale(QLocale::system()),
    userAgent(QStringLiteral("libRestLink/") + QStringLiteral(RESTLINK_VERSION_STR))
{
    if (!QSslSocket::supportsSsl())
        restlinkWarning() << "SSL support disabled, seem that relevant libraries are missing";
}

bool ApiPrivate::hasRemoteRequest(const Request &request) const
{
    auto it = std::find_if(remoteRequests.cbegin(), remoteRequests.cend(), [&request](const RemoteRequest &r) {
        return r.request.endpoint() == request.endpoint();
    });

    return (it != remoteRequests.end());
}

Request ApiPrivate::remoteRequest(const Request &request) const
{
    auto it = std::find_if(remoteRequests.cbegin(), remoteRequests.cend(), [&request](const RemoteRequest &r) {
        return r.request.endpoint() == request.endpoint();
    });

    if (it != remoteRequests.cend())
        return it->request;
    else
        return Request();
}

QByteArray ApiPrivate::remoteRequestData(const Request &request) const
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

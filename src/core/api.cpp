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

/*!
 * \class RestLink::Api
 * \brief Represents an API interface that manages configuration, requests, and parameters.
 *
 * The Api class is used to interact with a specific API, including managing the API's name, version,
 * URL, and request parameters. It provides methods for setting and retrieving these properties,
 * as well as configuring the API using both direct URLs and JSON configuration objects. The class
 * allows for adding, removing, and managing request parameters.
 *
 * \property QString apiName
 * \brief The name of the API.
 *
 * \property int apiVersion
 * \brief The version of the API.
 *
 * \property QUrl apiUrl
 * \brief The base URL of the API.
 *
 * \property int apiParameterCount
 * \brief The number of parameters associated with the API.
 *
 * \property QString userAgent
 * \brief The user agent string to be used in API requests.
 *
 * The Api class is responsible for creating network requests and managing
 * response data. It supports adding parameters to requests, configuring the API from different sources,
 * and emitting signals for configuration changes and network error events.
 *
 * \signal void apiNameChanged(const QString &name)
 * \brief Emitted when the API name changes.
 *
 * \signal void apiVersionChanged(int version)
 * \brief Emitted when the API version changes.
 *
 * \signal void apiUrlChanged(const QUrl &url)
 * \brief Emitted when the API URL changes.
 *
 * \signal void apiParametersChanged()
 * \brief Emitted when the API parameters change.
 *
 * \signal void userAgentChanged(const QString &agent)
 * \brief Emitted when the user agent changes.
 *
 * \signal void configurationCompleted()
 * \brief Emitted when the API configuration is completed.
 *
 * \signal void configurationFailed()
 * \brief Emitted when the API configuration fails.
 *
 * \see ApiBase, ApiRequest, ApiRequestParameter
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
QString Api::apiName() const
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
void Api::setApiName(const QString &name)
{
    RESTLINK_D(Api);
    if (d->name != name) {
        d->name = name;
        emit apiNameChanged(name);
    }
}

/*!
 * \brief Returns the version of the API.
 *
 * This method retrieves the current version of the API. The version is represented as an integer and can be used to manage different versions of the API.
 *
 * \return The integer version number of the API.
 */
int Api::apiVersion() const
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
void Api::setApiVersion(int version)
{
    RESTLINK_D(Api);
    if (d->version != version) {
        d->version = version;
        emit apiVersionChanged(version);
    }
}

/*!
 * \brief Returns the URL of the API.
 *
 * This method retrieves the current URL of the API, which is used to define the endpoint for API requests.
 *
 * \return A QUrl representing the API URL.
 */
QUrl Api::apiUrl() const
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
void Api::setApiUrl(const QUrl &url)
{
    RESTLINK_D(Api);

    if (d->url == url)
        return;

    d->url = url;
    emit apiUrlChanged(url);
}

/*!
 * \brief Returns the API parameter at the specified index.
 *
 * This method retrieves the API request parameter at the given index. It can be used to access parameters added to the API configuration.
 *
 * \param index The index of the desired API parameter.
 * \return The ApiRequestParameter at the specified index.
 */
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

/*!
 * \brief Returns the count of API parameters.
 *
 * This method returns the total number of API parameters that have been added to the API configuration.
 *
 * \return The number of parameters in the API configuration.
 */
int Api::apiParameterCount() const
{
    RESTLINK_D(const Api);
    return d->parameters.size();
}

/*!
 * \brief Returns the list of API parameters.
 *
 * This method retrieves all API parameters currently set in the API configuration.
 * It returns a list of ApiRequestParameter objects.
 *
 * \return A list of ApiRequestParameter objects representing the parameters.
 */
QList<ApiRequestParameter> Api::apiParameters() const
{
    RESTLINK_D(const Api);
    return d->parameters.toList();
}

/*!
 * \brief Adds a new parameter to the API.
 *
 * This method adds a new parameter to the API configuration. The parameter will be included in the API request.
 *
 * \param parameter The ApiRequestParameter to be added to the API.
 */
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

/*!
 * \brief Adds a list of parameters to the API.
 *
 * This method adds multiple parameters to the API configuration. All parameters in the list will be included in the API request.
 *
 * \param parameters A list of ApiRequestParameter objects to be added to the API.
 */
void Api::addParameters(const QList<ApiRequestParameter> &parameters)
{
    RESTLINK_D(Api);
    if (!parameters.isEmpty()) {
        d->parameters.append(parameters.toVector());
        emit apiParametersChanged();
    }
}

/*!
 * \brief Sets the API parameters.
 *
 * This method sets the list of parameters for the API configuration, replacing any existing parameters.
 *
 * \param parameters A list of ApiRequestParameter objects to set as the API parameters.
 */
void Api::setApiParameters(const QList<ApiRequestParameter> &parameters)
{
    RESTLINK_D(Api);
    const QList<ApiRequestParameter> currentParameters = d->parameters.toList();
    if (currentParameters == parameters)
        return;

    d->parameters = parameters.toVector();
    emit apiParametersChanged();
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

/*!
 * \brief Configures the API with the given JSON configuration.
 *
 * This method configures the API using a JSON object. The configuration can include parameters like the API URL, version, and other settings.
 *
 * \param config A QJsonObject containing the configuration data for the API.
 * \return Returns true if the configuration was successful, false otherwise.
 */
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

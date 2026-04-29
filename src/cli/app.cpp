#include "app.h"

#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>
#include <QtCore/qfile.h>
#include <QtCore/qtimer.h>

#include <RestLink/api.h>
#include <RestLink/request.h>
#include <RestLink/body.h>
#include <RestLink/response.h>
#include <RestLink/networkmanager.h>
#include <RestLink/pluginmanager.h>

// Infos options
#define SCHEMES_OPTIONS "schemes"
#define LIST_PLUGINS_OPTIONS "list-plugins"

// Config options
#define API_URL_OPTION      "api-url"
#define BEARER_TOKEN_OPTION "bearer-token"
#define CONFIG_OPTION       "config"
#define VERBOSE_OPTION      "verbose"
#define HEADERSONLY_OPTION  "headers-only"
#define BODYONLY_OPTION     "body-only"
#define TIMING_OPTION       "timing"

// HTTP Methods
#define HEAD_OPTION   "head"
#define GET_OPTION    "get"
#define POST_OPTION   "post"
#define PUT_OPTION    "put"
#define PATCH_OPTION  "patch"
#define DELETE_OPTION "delete"

// Parameters
#define PATH_OPTION   "path"
#define QUERY_OPTION  "query"
#define HEADER_OPTION "header"

// Body
#define DATA_OPTION "data"
#define JSON_OPTION "json"
#define FILE_OPTION "file"

using namespace RestLink;

App::App(int &argc, char *argv[])
    : QCoreApplication(argc, argv)
    , m_out(stdout)
    , m_pendingRequestCount(0)
    , m_api(nullptr)
{
}

void App::initParser()
{
    m_parser.setApplicationDescription("RestCli - The RestLink commandline tool");
    m_parser.addVersionOption();
    m_parser.addHelpOption();

    // Scheme option
    {
        QCommandLineOption option(SCHEMES_OPTIONS, "List supported url schemes.");
        m_parser.addOption(option);
    }

    // List Plugins option
    {
        QCommandLineOption option(LIST_PLUGINS_OPTIONS, "List all loaded plugins.");
        m_parser.addOption(option);
    }

    // Api Url option
    {
        QCommandLineOption option(API_URL_OPTION, "Set API base url.", "url");
        m_parser.addOption(option);
    }

    // Bearer Token option
    {
        QCommandLineOption option(BEARER_TOKEN_OPTION, "Set API bearer token.", "token");
        m_parser.addOption(option);
    }

    // Config option
    {
        QCommandLineOption option(CONFIG_OPTION, "Set the API config JSON file.");
        option.setValueName("config-file");
        option.setDefaultValue("APIConfig.json");
        m_parser.addOption(option);
    }

    // Verbose option
    {
        QCommandLineOption option(VERBOSE_OPTION, "Make the operation more talkative.");
        m_parser.addOption(option);
    }

    // Headers only option
    {
        QCommandLineOption option(HEADERSONLY_OPTION, "Only the response headers must be shown.");
        m_parser.addOption(option);
    }

    // Body only option
    {
        QCommandLineOption option(BODYONLY_OPTION, "Only the response body must be shown.");
        m_parser.addOption(option);
    }

    // Timing option
    {
        QCommandLineOption option(TIMING_OPTION, "Compute execution time.");
        m_parser.addOption(option);
    }

    // HEAD Option
    {
        QCommandLineOption option(HEAD_OPTION, "Send a HEAD request to the specified endpoint.");
        option.setValueName("endpoint");
        m_parser.addOption(option);
    }

    // GET Option
    {
        QCommandLineOption getOption(GET_OPTION, "Send a GET request to the specified endpoint.");
        getOption.setValueName("endpoint");
        m_parser.addOption(getOption);
    }

    // POST Option
    {
        QCommandLineOption postOption(POST_OPTION, "Send a POST request with data to the specified endpoint.");
        postOption.setValueName("endpoint");
        m_parser.addOption(postOption);
    }

    // PUT Option
    {
        QCommandLineOption putOption(PUT_OPTION, "Send a PUT request with data to the specified endpoint.");
        putOption.setValueName("endpoint");
        m_parser.addOption(putOption);
    }

    // PATCH Option
    {
        QCommandLineOption patchOption(PATCH_OPTION, "Send a PATCH request with data to the specified endpoint.");
        patchOption.setValueName("endpoint");
        m_parser.addOption(patchOption);
    }

    // DELETE Option
    {
        QCommandLineOption deleteOption(DELETE_OPTION, "Send a DELETE request to the specified endpoint.");
        deleteOption.setValueName("endpoint");
        m_parser.addOption(deleteOption);
    }

    // Path Option
    {
        QCommandLineOption option(PATH_OPTION, "Set an url path parameter.");
        option.setValueName("parameter=value");
        m_parser.addOption(option);
    }

    // Query Option
    {
        QCommandLineOption option(QUERY_OPTION, "Set an url query parameter.");
        option.setValueName("key=value");
        m_parser.addOption(option);
    }

    // Header Option
    {
        QCommandLineOption option(HEADER_OPTION, "Set a request header.");
        option.setValueName("name=value");
        m_parser.addOption(option);
    }

    // Data Option
    {
        QCommandLineOption option(DATA_OPTION, "Set the request raw data.");
        option.setValueName("data");
        m_parser.addOption(option);
    }

    // JSON Data Option
    {
        QCommandLineOption option(JSON_OPTION, "Set the request json data.");
        option.setValueName("data");
        m_parser.addOption(option);
    }

    // File Data Option
    {
        QCommandLineOption option(FILE_OPTION, "Set the request file data.");
        option.setValueName("file name");
        m_parser.addOption(option);
    }
}

void App::setApi(RestLink::Api *api)
{
    connect(api, &Api::configurationCompleted, this, &App::run);
    connect(api, &Api::configurationFailed, qApp, &QCoreApplication::quit);

    m_parser.process(*this);

    QUrl configUrl;
    if (m_parser.isSet(CONFIG_OPTION)) {
        const QString config = m_parser.value(CONFIG_OPTION);
        if (QFile::exists(config))
            configUrl = QUrl::fromLocalFile(config);
        else
            configUrl = QUrl::fromUserInput(config);
    } else if (qEnvironmentVariableIsSet("API_CONFIG_FILE")) {
        const QString config = qEnvironmentVariable("API_CONFIG_FILE");
        configUrl = QUrl::fromLocalFile(config);
    } else if (qEnvironmentVariableIsSet("API_CONFIG_URL")) {
        const QString config = qEnvironmentVariable("API_CONFIG_URL");
        configUrl = QUrl::fromLocalFile(config);
    }

    if (configUrl.isValid())
        api->configure(configUrl);
    else {
        if (qEnvironmentVariableIsSet("API_URL"))
            api->setUrl(QUrl::fromUserInput(qEnvironmentVariable("API_URL")));

        if (qEnvironmentVariableIsSet("BEARER_TOKEN"))
            api->setBearerToken(qEnvironmentVariable("BEARER_TOKEN"));

        QTimer::singleShot(0, this, &App::run);
    }

    m_api = api;
}

void App::run()
{
    if (m_parser.isSet(SCHEMES_OPTIONS)) {
        m_out << m_api->networkManager()->supportedSchemes().join(", ") << Qt::endl;
        quit();
        return;
    }

    if (m_parser.isSet(LIST_PLUGINS_OPTIONS)) {
        const QList<Plugin *> plugins = PluginManager::loadedPlugins();
        for (const Plugin *plugin : plugins) {
            m_out << "* " << plugin->name() << " (" << plugin->version() << ")\n";

            const QJsonObject metaData = plugin->metaData();
            const QStringList mainKeys = { "uuid", "description" };
            const QStringList blackKeys = { "name" };

            QStringList allKeys = metaData.keys();
            allKeys.removeIf([mainKeys, blackKeys](const QString &key) {
                return mainKeys.contains(key) || blackKeys.contains(key);
            });
            allKeys = mainKeys + allKeys;
            allKeys.append("schemes");

            auto getValue = [&metaData, &plugin](const QString &key) {
                if (metaData.contains(key))
                    return metaData.value(key).toVariant().toString();
                else if (key == "version")
                    return plugin->version();
                else if (key == "schemes")
                    return plugin->supportedSchemes().join(", ");
                else
                    return QString();
            };

            for (const QString &key : std::as_const(allKeys)) {
                const QString value = getValue(key);
                if (value.isEmpty())
                    continue;

                m_out << "  - " << key << ": " << value << '\n';
            }

            m_out << Qt::endl;
        }

        m_out << plugins.size() << " plugin(s) loaded." << Qt::endl;

        quit();
        return;
    }

    if (m_parser.isSet(API_URL_OPTION)) {
        m_api->setUrl(QUrl::fromUserInput(m_parser.value(API_URL_OPTION)));
    }

    if (m_parser.isSet(BEARER_TOKEN_OPTION)) {
        m_api->setBearerToken(m_parser.value(BEARER_TOKEN_OPTION));
    }

    if (m_parser.isSet(HEAD_OPTION)) {
        Request request = makeRequest(HEAD_OPTION);
        Response *response = m_api->head(request);
        monitorResponse(response);
        return;
    }

    if (m_parser.isSet(GET_OPTION)) {
        Request request = makeRequest(GET_OPTION);
        Response *response = m_api->get(request);
        monitorResponse(response);
        return;
    }

    if (m_parser.isSet(POST_OPTION)) {
        Request request = makeRequest(POST_OPTION);
        Response *response = m_api->post(request, makeBody());
        monitorResponse(response);
        return;
    }

    if (m_parser.isSet(PUT_OPTION)) {
        Request request = makeRequest(PUT_OPTION);
        Response *response = m_api->put(request, makeBody());
        monitorResponse(response);
        return;
    }

    if (m_parser.isSet(PATCH_OPTION)) {
        Request request = makeRequest(PATCH_OPTION);
        Response *response = m_api->patch(request, makeBody());
        monitorResponse(response);
        return;
    }

    if (m_parser.isSet(DELETE_OPTION)) {
        Request request = makeRequest(DELETE_OPTION);
        Response *response = m_api->deleteResource(request);
        monitorResponse(response);
        return;
    }

    m_out << "\033[1;31mNothing to do, have you forgotten the HTTP methods?\033[0m" << Qt::endl;
    m_out << "Reminder of class courses, HTTP methods are:" << Qt::endl;
    m_out << "\033[1;32mHEAD GET POST PUT PATCH DELETE\033[0m" << Qt::endl;

    QStringList responses = {
        "Don't forget next time, okay? ;)",
        "Come on, you can do better than that!",
        "HTTP methods: HEAD, GET, POST, PUT, PATCH, DELETE... Easy, right?",
    };
    m_out << responses[rand() % responses.size()] << Qt::endl;

    quit();
}

Request App::makeRequest(const QString &option)
{
    Request request(m_parser.value(option));

    if (m_parser.isSet(PATH_OPTION)) {
        const QStringList parameters = m_parser.values(PATH_OPTION);
        for (const QString &param : parameters) {
            QStringList p = param.split('=');
            if (p.size() >= 2)
                request.setPathParameter(p.first(), p.last());
        }
    }

    if (m_parser.isSet(QUERY_OPTION)) {
        const QStringList parameters = m_parser.values(QUERY_OPTION);
        for (const QString &param : parameters) {
            QStringList p = param.split('=');
            if (p.size() >= 2)
                request.addQueryParameter(p.first(), p.last());
            else if (p.size() == 1)
                request.addQueryParameter(p.first(), QVariant());
        }
    }

    if (m_parser.isSet(HEADER_OPTION)) {
        const QStringList headers = m_parser.values(HEADER_OPTION);
        for (const QString &header : headers) {
            QStringList p = header.split('=');
            if (p.size() >= 2)
                request.setHeader(p.first(), p.last());
        }
    }

    return request;
}

Body App::makeBody()
{
    if (m_parser.isSet(DATA_OPTION)) {
        return Body(m_parser.value(DATA_OPTION));
    }

    if (m_parser.isSet(JSON_OPTION)) {
        return Body(m_parser.value(JSON_OPTION), RESTLINK_MIME_JSON);
    }

    if (m_parser.isSet(FILE_OPTION))
        return Body(File(m_parser.value(FILE_OPTION)));

    return Body();
}

void App::monitorResponse(Response *response)
{
    if (response == nullptr)
        return;

    if (m_parser.isSet(VERBOSE_OPTION)) {
        switch (response->method()) {
        case AbstractRequestHandler::HeadMethod:
            m_out << "HEAD ";
            break;

        case AbstractRequestHandler::GetMethod:
            m_out << "GET ";
            break;

        case AbstractRequestHandler::PostMethod:
            m_out << "POST ";
            break;

        case AbstractRequestHandler::PutMethod:
            m_out << "PUT ";
            break;

        case AbstractRequestHandler::PatchMethod:
            m_out << "PATCH ";
            break;

        case AbstractRequestHandler::DeleteMethod:
            m_out << "DELETE ";
            break;

        default:
            break;
        }

        m_out << response->url().toString(QUrl::DecodeReserved);
        m_out << '\n' << Qt::endl;
    }

    QElapsedTimer *timer = nullptr;
    if (m_parser.isSet(TIMING_OPTION)) {
        timer = new QElapsedTimer();
        timer->start();
    }

    connect(response, &Response::finished, this, [this, response, timer] {
        qint64 timing = 0;
        if (timer) {
            timing = timer->elapsed();
            delete timer;
        }

        if (m_parser.isSet(HEADERSONLY_OPTION) || (!m_parser.isSet(BODYONLY_OPTION) && response->hasHttpStatusCode())) {
            m_out << "HTTP " << response->httpStatusCode() << ' ' << response->httpReasonPhrase() << Qt::endl;
            const QStringList headers = response->headerList();
            for (const QString &header : headers) {
                m_out << header.toUpper() << ": " << response->header(header) << Qt::endl;
            }
            m_out << Qt::endl;
        }

        if (response->hasHttpStatusCode()) {
            if (!m_parser.isSet(HEADERSONLY_OPTION)) {
                const QString type = response->header("Content-Type");
                if (type.startsWith("application/json")) {
                    const QJsonValue value = response->readJson();
                    QJsonDocument doc;
                    if (value.isObject())
                        doc.setObject(value.toObject());
                    else if (value.isArray())
                        doc.setArray(value.toArray());
                    m_out << doc.toJson(QJsonDocument::Indented);
                } else {
                    m_out << response->readBody();
                }
            }
        } else if (response->hasNetworkError()) {
            m_out << response->networkErrorString();
        } else {
            m_out << "error: something happened, we don't known what.";
        }

        response->deleteLater();

        if (timing > 0)
            m_out << "\nrequest ran in " << timing << " ms." << Qt::endl;
    });

    connect(response, &QObject::destroyed, this, [this] {
        --m_pendingRequestCount;

        if (m_pendingRequestCount == 0)
            quit();
    });

    ++m_pendingRequestCount;
}

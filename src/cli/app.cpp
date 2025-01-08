#include "app.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <RestLink/api.h>
#include <RestLink/request.h>
#include <RestLink/body.h>
#include <RestLink/response.h>

// Config option
#define CONFIG_OPTION   "config"
#define VERBOSE_OPTION  "verbose"
#define BODYONLY_OPTION "body-only"

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

    // Config option
    {
        QCommandLineOption option(CONFIG_OPTION, "Set the API config JSON file");
        option.setValueName("config-file");
        option.setDefaultValue("APIConfig.json");
        m_parser.addOption(option);
    }

    // Verbose option
    {
        QCommandLineOption option(VERBOSE_OPTION, "Make the operation more talkative");
        m_parser.addOption(option);
    }

    // Body only option
    {
        QCommandLineOption option(BODYONLY_OPTION, "Only the response body must be shown");
        m_parser.addOption(option);
    }

    // HEAD Option
    {
        QCommandLineOption option(HEAD_OPTION, "Send a HEAD request to the specified endpoint");
        option.setValueName("endpoint");
        m_parser.addOption(option);
    }

    // GET Option
    {
        QCommandLineOption getOption(GET_OPTION, "Send a GET request to the specified endpoint");
        getOption.setValueName("endpoint");
        m_parser.addOption(getOption);
    }

    // POST Option
    {
        QCommandLineOption postOption(POST_OPTION, "Send a POST request with data to the specified endpoint");
        postOption.setValueName("endpoint");
        m_parser.addOption(postOption);
    }

    // PUT Option
    {
        QCommandLineOption putOption(PUT_OPTION, "Send a PUT request with data to the specified endpoint");
        putOption.setValueName("endpoint");
        m_parser.addOption(putOption);
    }

    // PATCH Option
    {
        QCommandLineOption patchOption(PATCH_OPTION, "Send a PATCH request with data to the specified endpoint");
        patchOption.setValueName("endpoint");
        m_parser.addOption(patchOption);
    }

    // DELETE Option
    {
        QCommandLineOption deleteOption(DELETE_OPTION, "Send a DELETE request to the specified endpoint");
        deleteOption.setValueName("endpoint");
        m_parser.addOption(deleteOption);
    }

    // Path Option
    {
        QCommandLineOption option(PATH_OPTION, "Set an url path parameter");
        option.setValueName("parameter");
        m_parser.addOption(option);
    }

    // Query Option
    {
        QCommandLineOption option(QUERY_OPTION, "Set an url query parameter");
        option.setValueName("parameter");
        m_parser.addOption(option);
    }

    // Header Option
    {
        QCommandLineOption option(HEADER_OPTION, "Set a request header");
        option.setValueName("header");
        m_parser.addOption(option);
    }

    // Data Option
    {
        QCommandLineOption option(DATA_OPTION, "Set the request raw data");
        option.setValueName("data");
        m_parser.addOption(option);
    }

    // JSON Data Option
    {
        QCommandLineOption option(JSON_OPTION, "Set the request json data");
        option.setValueName("data");
        m_parser.addOption(option);
    }
}

void App::setApi(RestLink::Api *api)
{
    connect(api, &Api::configurationCompleted, this, &App::run);
    connect(api, &Api::configurationFailed, this, &QCoreApplication::quit);

    m_parser.process(*this);

    QUrl configUrl;
    if (m_parser.isSet(CONFIG_OPTION)) {
        configUrl = QUrl::fromUserInput(m_parser.value(CONFIG_OPTION));
        if (!configUrl.isValid())
            configUrl = QUrl::fromLocalFile(m_parser.value(CONFIG_OPTION));
    } else {
        const QString config = qEnvironmentVariable("API_CONFIG_FILE", m_parser.value(CONFIG_OPTION));
        configUrl = QUrl::fromLocalFile(config);
    }

    api->configure(configUrl);

    m_api = api;
}

void App::run()
{
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
                request.setQueryParameter(p.first(), p.last());
            else if (p.size() == 1)
                request.setQueryParameter(p.first(), QVariant());
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
    return Body();
}

void App::monitorResponse(Response *response)
{
    if (m_parser.isSet(VERBOSE_OPTION)) {
        switch (response->operation()) {
        case Api::HeadOperation:
            m_out << "HEAD ";
            break;

        case Api::GetOperation:
            m_out << "GET ";
            break;

        case Api::PostOperation:
            m_out << "POST ";
            break;

        case Api::PutOperation:
            m_out << "PUT ";
            break;

        case Api::PatchOperation:
            m_out << "PATCH ";
            break;

        case Api::DeleteOperation:
            m_out << "DELETE ";
            break;

        default:
            break;
        }

        m_out << response->url().toString(QUrl::DecodeReserved);
        m_out << '\n' << Qt::endl;
    }

    connect(response, &Response::finished, this, [this, response] {
        if (!m_parser.isSet(BODYONLY_OPTION) && response->hasHttpStatusCode()) {
            m_out << "HTTP " << response->httpStatusCode() << ' ' << response->httpReasonPhrase() << Qt::endl;
            const QByteArrayList headers = response->headerList();
            for (const QByteArray &header : headers) {
                m_out << header.toUpper() << ": " << response->header(header) << Qt::endl;
            }
            m_out << Qt::endl;
        }

        if (response->isSuccess()) {
            if (response->header("Content-Type").startsWith("application/json")) {
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
        } else if (response->hasNetworkError()) {
            m_out << response->networkErrorString();
        } else {
            m_out << "error: something happened, we don't known what.";
        }

        response->deleteLater();
    });

    connect(response, &QObject::destroyed, this, [this] {
        --m_pendingRequestCount;

        if (m_pendingRequestCount == 0)
            quit();
    });

    ++m_pendingRequestCount;
}

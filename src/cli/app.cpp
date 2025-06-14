#include "app.h"

#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>

#include <RestLink/api.h>
#include <RestLink/request.h>
#include <RestLink/body.h>
#include <RestLink/response.h>

// Config option
#define CONFIG_OPTION   "config"
#define VERBOSE_OPTION  "verbose"
#define BODYONLY_OPTION "body-only"
#define TIMING_OPTION   "timing"

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

    // Timing option
    {
        QCommandLineOption option(TIMING_OPTION, "compute execution time");
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

    // File Data Option
    {
        QCommandLineOption option(FILE_OPTION, "Set the request file data");
        option.setValueName("file name");
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

        if (!m_parser.isSet(BODYONLY_OPTION) && response->hasHttpStatusCode()) {
            m_out << "HTTP " << response->httpStatusCode() << ' ' << response->httpReasonPhrase() << Qt::endl;
            const QStringList headers = response->headerList();
            for (const QString &header : headers) {
                m_out << header.toUpper() << ": " << response->header(header) << Qt::endl;
            }
            m_out << Qt::endl;
        }

        if (response->hasHttpStatusCode()) {
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

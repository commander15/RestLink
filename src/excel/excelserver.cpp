#include "excelserver.h"
#include "excelserver_p.h"

#include "excelsheet.h"

#include <RestLink/queryparameter.h>
#include <RestLink/serverresponse.h>

#include <xlsxworkbook.h>

#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>

namespace RestLink {

ExcelServer::ExcelServer(QObject *parent)
    : RestLink::Server(new ExcelServerPrivate(this), parent)
{
}

void ExcelServer::handleConfiguration(const Request &request, const QJsonObject &body, ExcelSheet *sheet, ServerResponse *response)
{
    if (body.contains("header_row")) {
        const int row = body.value("header_row").toInt();

        QXlsx::CellRange dim = sheet->propertyDimension();
        dim.setFirstRow(row);
        dim.setLastRow(row);
        sheet->setPropertyDimension(dim);
    }
}

void ExcelServer::handleGet(const Request &request, ExcelSheet *sheet, ServerResponse *response)
{
    RESTLINK_D(ExcelServer);

    int row = -1;

    if (row <= -1) {
        int page = (request.hasQueryParameter("page") ? request.queryParameter("page").value().toInt() : -1);
        int limit = (request.hasQueryParameter("limit") ? request.queryParameter("limit").value().toInt() : -1);

        QJsonObject object;
        object.insert("data", sheet->readObjects(page, limit));
        if (page > 0 && limit > 0) {
            object.insert("page", page);
            object.insert("items_per_page", limit);
        }

        response->setBody(object);
        response->setHttpStatusCode(200);
    } else if (row < sheet->objectCount()) {
        response->setBody(sheet->readObject(row));
        response->setHttpStatusCode(200);
    } else {
        response->setHttpStatusCode(404);
    }
}

bool ExcelServer::init()
{
    return true;
}

void ExcelServer::cleanup()
{
}

bool ExcelServer::maintain()
{
    return true;
}

void ExcelServer::processRequest(ApiBase::Operation operation, const Request &request, const Body &body, Response *res)
{
    RESTLINK_D(ExcelServer);

    ServerResponse *response = static_cast<ServerResponse *>(res);

    ExcelDocument *document = d->documentForUrl(res->property("excelUrl").toUrl());
    if (!document) {
        response->setHttpStatusCode(404);
        response->complete();
        return;
    }

    QString sheetName;
    int row = -1;
    d->extractEndpointData(request.endpoint(), &sheetName, &row);

    ExcelSheet *sheet = document->sheet(sheetName);
    if (!sheet) {
        response->setHttpStatusCode(404);
        response->complete();
        return;
    }

    RequestProcessing processing = request.processing();
    if (processing) {
        document->selectSheet(sheet->name());
        processing(request, body, sheet->workSheet(), response);
        response->complete();
        return;
    }

    //QJsonObject object = QJsonDocument::fromJson(body.data()).object();

    switch (operation) {
    case Api::GetOperation:
        handleGet(request, sheet, response);
        break;
    }

    response->complete();
}

Response *ExcelServer::createResponse(ApiBase::Operation operation, const Request &request, const Body &body, Api *api)
{
    Response *response = Server::createResponse(operation, request, body, api);
    response->setProperty("excelUrl", api->url());
    return response;
}

RestLink::ExcelServerPrivate::ExcelServerPrivate(ExcelServer *q)
    : ServerPrivate(Server::Synchronous, q)
{
    autoStart = true;
}

bool ExcelServerPrivate::prepareDocument(const Request &request, ExcelDocument *doc, int *row, ServerResponse *response)
{
    QString sheetName;
    *row = -1;
    extractEndpointData(request.endpoint(), &sheetName, row);

    int sheetIndex = doc->sheetNames().indexOf(sheetName);
    if (sheetIndex < 0) {
        response->setHttpStatusCode(400);
        response->complete();
        return false;
    }

    if (!doc->selectSheet(sheetIndex)) {
        response->setHttpStatusCode(500);
        response->complete();
        return false;
    }

    return true;
}

void ExcelServerPrivate::extractEndpointData(const QString &endpoint, QString *sheet, int *row)
{
    QStringList path = endpoint.split('/', Qt::SkipEmptyParts);

    if (path.size() > 0)
        *sheet = path.at(0);

    if (path.size() > 1)
        *row = path.at(1).toInt();
}

ExcelDocument *ExcelServerPrivate::documentForUrl(const QUrl &url)
{
    if (documents.contains(url))
        return documents.value(url);

    ExcelDocument *doc = new ExcelDocument(url.path());
    if (!doc->load()) {
        delete doc;
        return nullptr;
    }

    documents.insert(url, doc);
    return doc;
}

} // namespace RestLink

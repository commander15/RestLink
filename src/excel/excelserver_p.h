#ifndef EXCELSERVER_P_H
#define EXCELSERVER_P_H

#include "excelserver.h"
#include "exceldocument.h"

#include <RestLink/private/server_p.h>

namespace RestLink {

class ExcelServerPrivate : public ServerPrivate
{
public:
    ExcelServerPrivate(ExcelServer *q);

    ExcelDocument *documentForUrl(const QUrl &url);
    static bool prepareDocument(const Request &request, ExcelDocument *doc, int *row, ServerResponse *response);

    static void extractEndpointData(const QString &endpoint, QString *sheet, int *row);

    QHash<QUrl, ExcelDocument *> documents;
};

}

#endif // EXCELSERVER_P_H

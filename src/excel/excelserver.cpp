#include "excelserver.h"

#include <RestLink/serverrequest.h>
#include <RestLink/serverresponse.h>

namespace RestLink {

ExcelServer::ExcelServer(QObject *parent)
    : RestLink::Server(parent)
{
}

void ExcelServer::configure(Api::Operation op, const ServerRequest &request, ServerResponse *response)
{
    if (op != Api::PostOperation) {
        response->setHttpStatusCode(400);
        response->complete();
        return;
    }

    /*if (body.contains("header_row")) {
        const int row = body.value("header_row").toInt();

        QXlsx::CellRange dim = sheet->propertyDimension();
        dim.setFirstRow(row);
        dim.setLastRow(row);
        sheet->setPropertyDimension(dim);
    }*/

    response->setHttpStatusCode(200);
    response->complete();
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

} // namespace RestLink

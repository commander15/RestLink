#ifndef EXCELSERVER_H
#define EXCELSERVER_H

#include <RestLink/server.h>

namespace RestLink {

class ExcelSheet;
class ExcelDocument;
class ServerResponse;

class ExcelServer : public Server
{
    Q_OBJECT

public:
    explicit ExcelServer(QObject *parent = nullptr);

    QStringList supportedSchemes() const override
    { return { "excel", "xlsx" }; }

    void handleConfiguration(Api::Operation op, const Request &request, const QJsonObject &body, ExcelSheet *sheet, ServerResponse *response);

    void handleGet(const Request &request, ExcelSheet *sheet, ServerResponse *response);

protected:
    bool init() override;
    void cleanup() override;
    bool maintain() override;

    void processRequest(ApiBase::Operation operation, const Request &request, const Body &body, Response *res) override;
    Response *createResponse(ApiBase::Operation operation, const Request &request, const Body &body, Api *api) override;
};

} // namespace RestLink

#endif // EXCELSERVER_H

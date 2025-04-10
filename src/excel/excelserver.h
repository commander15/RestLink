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

    QString handlerName() const override
    { return QStringLiteral("Excel"); }

    QStringList supportedSchemes() const override
    { return { "xlsx" }; }

    void configure(Api::Operation op, const ServerRequest &request, ServerResponse *response);

protected:
    bool init() override;
    void cleanup() override;
    bool maintain() override;
};

} // namespace RestLink

#endif // EXCELSERVER_H

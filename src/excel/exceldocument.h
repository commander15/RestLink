#ifndef EXCELDOCUMENT_H
#define EXCELDOCUMENT_H

#include <RestLink/resourcecontroller.h>

#include <xlsxdocument.h>

namespace RestLink {

class ExcelSheet;

class ExcelDocument : public QXlsx::Document, public AbstractResourceController
{
    Q_OBJECT

public:
    ExcelDocument(const QUrl &url, QObject *parent = nullptr);

    QString endpoint() const override;

    bool canProcessRequest(const ServerRequest &request) const override;
    void processRequest(ApiBase::Operation operation, const ServerRequest &request, ServerResponse *response) override;

    void index(const ServerRequest &request, ServerResponse *response) override;
    void show(const ServerRequest &request, ServerResponse *response) override;
    void update(const ServerRequest &request, ServerResponse *response) override;
    void store(const ServerRequest &request, ServerResponse *response) override;
    void destroy(const ServerRequest &request, ServerResponse *response) override;

    ExcelSheet *sheet(const QString &name);

private:
    QList<ExcelSheet *> m_sheets;
};

} // namespace RestLink

#endif // EXCELDOCUMENT_H

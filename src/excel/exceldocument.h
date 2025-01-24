#ifndef EXCELDOCUMENT_H
#define EXCELDOCUMENT_H

#include <xlsxdocument.h>

namespace RestLink {

class ExcelSheet;

class ExcelDocument : public QXlsx::Document
{
    Q_OBJECT

public:
    ExcelDocument(const QString &fileName, QObject *parent = nullptr);

    ExcelSheet *sheet(const QString &name);

private:
    QList<ExcelSheet *> m_sheets;
};

} // namespace RestLink

#endif // EXCELDOCUMENT_H

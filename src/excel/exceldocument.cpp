#include "exceldocument.h"

#include "excelsheet.h"

namespace RestLink {

ExcelDocument::ExcelDocument(const QString &fileName, QObject *parent)
    : QXlsx::Document(fileName, parent)
{
}

ExcelSheet *ExcelDocument::sheet(const QString &name)
{
    auto it = std::find_if(m_sheets.begin(), m_sheets.end(), [name](const ExcelSheet *sheet) {
        return (sheet->name() == name);
    });

    if (it != m_sheets.end())
        return *it;
    else {
        auto sheet = new ExcelSheet(name, this);
        m_sheets.append(sheet);
        return sheet;
    }
}

} // namespace RestLink

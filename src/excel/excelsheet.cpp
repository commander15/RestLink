#include "excelsheet.h"

#include "exceldocument.h"

#include <xlsxworkbook.h>
#include <xlsxworksheet.h>

#include <QtCore/qjsonarray.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonvalue.h>

using namespace QXlsx;

namespace RestLink {

ExcelSheet::ExcelSheet(const QString &sheetName, ExcelDocument *doc)
    : QObject(doc)
    , m_sheet(nullptr)
    , m_doc(doc)
{
    auto sheet = static_cast<Document *>(doc)->sheet(sheetName);
    if (sheet && sheet->sheetType() == AbstractSheet::ST_WorkSheet) {
        m_sheet = static_cast<Worksheet *>(sheet);
        computeDimensions();
        updateProperties();
    }
}

QString ExcelSheet::name() const
{
    return m_sheet->sheetName();
}

bool ExcelSheet::isValid() const
{
    return m_sheet != nullptr;
}

QStringList ExcelSheet::propertyNames() const
{
    return m_properties.values();
}

QJsonArray ExcelSheet::readObjects(int page, int limit) const
{
    const int total = objectCount();

    if (page == -1)
        page = 1;

    if (limit == -1)
        limit = total;

    int offset = (page - 1) * limit;
    if (offset >= total)
        return QJsonArray();

    int max = offset + limit;
    if (max > total)
        max = total;

    QJsonArray objects;
    for (int i(offset); i < max; ++i)
        objects.append(readObject(i));
    return objects;
}

QJsonObject ExcelSheet::readObject(int row) const
{
    QJsonObject object;

    const QStringList properties = m_properties.values();
    for (const QString &property : properties)
        object.insert(property, readProperty(property, row));

    return object;
}

void ExcelSheet::writeObject(int row, const QJsonObject &object)
{
    const QStringList properties = object.keys();
    for (const QString &property : properties)
        writeProperty(property, object.value(property), row);
}

QJsonValue ExcelSheet::readProperty(const QString &name, int row) const
{
    const int colum = m_properties.key(name);
    return QJsonValue::fromVariant(colum >= 0 ? m_sheet->read(dataRow(row), colum) : QVariant());
}

void ExcelSheet::writeProperty(const QString &name, const QJsonValue &value, int row)
{
    const int colum = m_properties.key(name);
    if (colum >= 0)
        m_sheet->write(dataRow(row), colum, value);
}

int ExcelSheet::objectCount() const
{
    return m_dataDimension.lastRow() - m_dataDimension.firstRow();
}

CellRange ExcelSheet::propertyDimension() const
{
    return m_propertyDimension;
}

void ExcelSheet::setPropertyDimension(const CellRange &dim)
{
    if (dim.isValid()) {
        m_propertyDimension = dim;
        m_dataDimension.setFirstRow(dim.firstRow() + 1);

        updateProperties();
    }
}

CellRange ExcelSheet::dataDimension() const
{
    return m_dataDimension;
}

void ExcelSheet::setDataDimension(const CellRange &dim)
{
    m_dataDimension = dim;
}

void ExcelSheet::updateProperties()
{
    m_properties.clear();
    for (int i(m_propertyDimension.firstColumn()); i <= m_propertyDimension.lastColumn(); ++i) {
        QVariant value = m_sheet->read(m_propertyDimension.firstRow(), i);

        // If nothing found, consider there is a sub menu down
        if (value.isNull())
            value = m_sheet->read(m_propertyDimension.firstRow() + 1, i);

        m_properties.insert(i, value.toString());
    }
}

void ExcelSheet::computeDimensions()
{
    CellRange dim = m_sheet->dimension();

    m_propertyDimension.setFirstRow(dim.firstRow());
    m_propertyDimension.setLastRow(dim.firstRow());
    m_propertyDimension.setFirstColumn(dim.firstColumn());
    m_propertyDimension.setLastColumn(dim.lastColumn());

    m_dataDimension = dim;
    m_dataDimension.setFirstRow(dim.firstRow() + 1);
}

int ExcelSheet::dataRow(int row) const
{
    return m_dataDimension.firstRow() + row;
}

} // namespace RestLink

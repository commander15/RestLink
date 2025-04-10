#ifndef EXCELSHEET_H
#define EXCELSHEET_H

#include <QtCore/qobject.h>

#include <xlsxworksheet.h>

namespace RestLink {

class ExcelDocument;

class ExcelSheet : public QObject
{
    Q_OBJECT

public:
    ExcelSheet(const QString &sheetName, ExcelDocument *doc);

    QString name() const;

    bool isValid() const;

    QStringList propertyNames() const;

    QJsonArray readObjects(int page = -1, int limit = -1) const;

    QJsonObject readObject(int row) const;
    void writeObject(int row, const QJsonObject &object);

    QJsonValue readProperty(const QString &name, int row) const;
    void writeProperty(const QString &name, const QJsonValue &value, int row);

    int objectCount() const;

    QXlsx::CellRange propertyDimension() const;
    void setPropertyDimension(const QXlsx::CellRange &dim);

    QXlsx::CellRange dataDimension() const;
    void setDataDimension(const QXlsx::CellRange &dim);

    inline QXlsx::Worksheet *workSheet() const
    { return m_sheet; }

    inline ExcelDocument *document() const
    { return m_doc; }

private:
    void updateProperties();
    void computeDimensions();

    int dataRow(int row) const;

    QMap<int, QString> m_properties;
    QXlsx::CellRange m_propertyDimension;
    QXlsx::CellRange m_dataDimension;

    QXlsx::Worksheet *m_sheet;
    ExcelDocument *m_doc;
};

} // namespace RestLink

#endif // EXCELSHEET_H

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QtCore/qelapsedtimer.h>

#include <xlsxdocument.h>

namespace RestLink {
namespace Excel {

class DocumentManager;

class Document
{
public:
    Document(const QString &fileName, DocumentManager *manager);
    ~Document();

    bool selectSheet(const QString &name);

    QVariantMap readRecord(int line) const;
    void writeRecord(int line, const QVariantMap &data);

    bool saveChanges();

private:
    bool expired() const;
    void restartTimer();

    QXlsx::Document m_doc;
    bool m_dirty;
    QElapsedTimer m_timer;

    friend class DocumentManager;
};

} // namespace Excel
} // namespace RestLink

#endif // DOCUMENT_H

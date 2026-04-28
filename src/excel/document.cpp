#include "document.h"

#include "documentmanager.h"

namespace RestLink {
namespace Excel {

Document::Document(const QString &fileName, DocumentManager *manager)
    : m_doc(fileName, manager)
    , m_dirty(false)
{
    m_timer.start();
}

Document::~Document()
{
    saveChanges();
}

bool Document::selectSheet(const QString &name)
{
    return m_doc.selectSheet(name);
}

QVariantMap Document::readRecord(int line) const
{
    QVariantMap data;
    return data;
}

void Document::writeRecord(int line, const QVariantMap &data)
{
    // ...
    m_dirty = true;
}

bool Document::saveChanges()
{
    if (!m_dirty)
        return true;

    if (m_doc.save()) {
        m_dirty = false;
        return true;
    }

    return false;
}

bool Document::expired() const
{
    // Expired after 30 mins
    return m_timer.elapsed() > 1800000L;
}

void Document::restartTimer()
{
    m_timer.restart();
}

} // namespace Excel
} // namespace RestLink

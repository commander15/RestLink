#include "documentmanager.h"
#include "document.h"

#include <RestLink/serverrequest.h>

namespace RestLink {
namespace Excel {

DocumentManager::DocumentManager(QObject *parent)
    : QObject(parent)
{}

DocumentManager::~DocumentManager()
{
    const QList<QUrl> urls = m_docs.keys();
    for (const QUrl &url : urls)
        delete m_docs.take(url);
}

Document *DocumentManager::requestDocument(const ServerRequest &request)
{
    Document *doc = nullptr;

    if (m_docs.contains(request.baseUrl())) {
        doc = m_docs.value(request.baseUrl());
    } else {
        const QString fileName = request.baseUrl().path();
        doc = new Document(fileName, this);
        m_docs.insert(request.baseUrl(), doc);
    }

    doc->restartTimer();
    return doc;
}

void DocumentManager::releaseDocuments()
{
    const QList<QUrl> urls = m_docs.keys();
    for (const QUrl &url : urls) {
        Document *doc = m_docs.value(url);
        if (doc->expired())
            delete m_docs.take(url);
    }
}

} // namespace Excel
} // namespace RestLink

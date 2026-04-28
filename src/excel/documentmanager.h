#ifndef DOCUMENTMANAGER_H
#define DOCUMENTMANAGER_H

#include <QObject>
#include <QtCore/qhash.h>

namespace RestLink {

class ServerRequest;

namespace Excel {

class Document;

class DocumentManager : public QObject
{
    Q_OBJECT

public:
    explicit DocumentManager(QObject *parent = nullptr);
    ~DocumentManager();

    Document *requestDocument(const ServerRequest &request);
    void releaseDocuments();

private:
    QHash<QUrl, Document *> m_docs;
};

} // namespace Excel
} // namespace RestLink

#endif // DOCUMENTMANAGER_H

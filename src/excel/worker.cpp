#include "worker.h"

#include <RestLink/serverrequest.h>

#include <xlsxdocument.h>

namespace RestLink {
namespace Excel {

Worker::Worker(QObject *parent)
    : RestLink::AbstractServerWorker{Synchronous, parent}
    , m_manager(nullptr)
{}

Worker::~Worker()
{}

bool Worker::init()
{
    m_manager = new DocumentManager();
    return true;
}

void Worker::cleanup()
{
    delete m_manager;
    m_manager = nullptr;
}

bool Worker::maintain()
{
    m_manager->releaseDocuments();
    return true;
}

void *Worker::createDataSource(const ServerRequest &request)
{
    return m_manager->requestDocument(request);
}

void Worker::clearDataSource(const ServerRequest &request, void *source)
{
    Q_UNUSED(request)
    Q_UNUSED(source)
}

} // namespace Excel
} // namespace RestLink

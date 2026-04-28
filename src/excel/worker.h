#ifndef WORKER_H
#define WORKER_H

#include <RestLink/abstractserverworker.h>

#include "documentmanager.h"

namespace RestLink {
namespace Excel {

class Worker final : public RestLink::AbstractServerWorker
{
    Q_OBJECT

public:
    explicit Worker(QObject *parent = nullptr);
    ~Worker();

protected:
    bool init() override;
    void cleanup() override;
    bool maintain() override;

    void *createDataSource(const ServerRequest &request) override;
    void clearDataSource(const ServerRequest &request, void *source) override;

private:
    DocumentManager *m_manager;
};

} // namespace Excel
} // namespace RestLink

#endif // WORKER_H

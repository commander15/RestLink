#include <RestLink/plugin.h>
#include <RestLink/server.h>

#include "worker.h"

#define RESTLINK_EXCEL_IID "com.restlink.excel"

namespace RestLink {
namespace Excel {

class Q_DECL_EXPORT Plugin final : public RestLink::Plugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID RESTLINK_EXCEL_IID FILE "metadata.json")

public:
    explicit Plugin(QObject *parent = nullptr)
        : RestLink::Plugin(parent) {}

    QString version() const override {
        return QStringLiteral(RESTLINK_VERSION_STR);
    }

    QStringList supportedSchemes() const override {
        return { "xlsx" };
    }

    AbstractRequestHandler *createHandler(QObject *parent) override {
        return Server::create<Worker>(QStringLiteral("RestLink Excel"), supportedSchemes(), parent);
    }
};

} // Excel namespace
} // RestLink namespace

#include "plugin.moc"

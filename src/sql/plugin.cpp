#include <RestLink/plugin.h>
#include <RestLink/server.h>

#include <routing/router.h>

#include <QtCore/qcoreapplication.h>

#include <QtSql/qsqldatabase.h>

namespace RestLink {
namespace Sql {

class Plugin : public RestLink::Plugin
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID RESTLINK_PLUGIN_IID FILE "metadata.json")

public:
    explicit Plugin(QObject *parent = nullptr)
        : RestLink::Plugin(parent) {}

    AbstractRequestHandler *createHandler() override
    {
        const QStringList drivers = QSqlDatabase::drivers();

        QStringList schemes;
        std::transform(drivers.begin(), drivers.end(), std::back_inserter(schemes), [](const QString &name) {
            return name.mid(1).toLower();
        });

        return Server::create<Router>(QStringLiteral("SQL"), schemes, qApp);
    }
};

} // namespace Sql
} // namespace RestLink

#include "plugin.moc"

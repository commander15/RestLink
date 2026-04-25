#include <RestLink/plugin.h>
#include <RestLink/server.h>

#include <routing/router.h>

#include <QtSql/qsqldatabase.h>

#define RESTLINK_SQL_PLUGIN_IID "com.restlink.sql"

namespace RestLink {
namespace Sql {

class Plugin final : public RestLink::Plugin
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID RESTLINK_SQL_PLUGIN_IID FILE "metadata.json")

public:
    explicit Plugin(QObject *parent = nullptr)
        : RestLink::Plugin(parent) {}

    QString version() const override {
        return QStringLiteral(RESTLINK_VERSION_STR);
    }

    QStringList supportedSchemes() const override {
        const QStringList availableDrivers = QSqlDatabase::drivers();

        QStringList schemes;
        std::transform(availableDrivers.begin(), availableDrivers.end(), std::back_inserter(schemes), [](const QString &name) {
            return (name.startsWith('Q') ? name.mid(1).toLower() : name);
        });

        return schemes;
    }

    AbstractRequestHandler *createHandler(QObject *parent) override {
        const QStringList schemes = supportedSchemes();
        return Server::create<Router>(QStringLiteral("RestLink SQL"), schemes, parent);
    }
};

} // namespace Sql
} // namespace RestLink

#include "plugin.moc"

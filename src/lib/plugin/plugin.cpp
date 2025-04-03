#include "plugin.h"
#include "plugin_p.h"

namespace RestLink {

Plugin::Plugin(QObject *parent)
    : QObject{parent}
{
}

Plugin::~Plugin()
{
}

QString Plugin::name() const
{
    return m_metaData.value("name").toString();
}

QJsonObject Plugin::metaData() const
{
    return m_metaData;
}

void Plugin::setMetaData(const QJsonObject &metaData)
{
    m_metaData = metaData;
}

} // namespace RestLink

#include "plugin.h"
#include "plugin_p.h"

/**
 * @file plugin.h
 *
 * @brief The RestLink::Plugin class header file
 */

/**
 * @def RESTLINK_PLUGIN_IID
 * @brief Interface ID for RestLink Plugin interface used by Qt's plugin system.
 *
 * This string uniquely identifies the Plugin interface for use with Q_DECLARE_INTERFACE
 * and qobject_cast when loading plugins dynamically.
 */

namespace RestLink {

/**
 * @class RestLink::Plugin
 * @brief Base class for RestLink plugins that provide custom request handlers.
 *
 * Plugins implementing this class can be dynamically loaded and are expected to return
 * an instance of AbstractRequestHandler via createHandler(). Metadata such as UUID and name
 * can be attached using setMetaData().
 *
 * When creating a plugin, consider the **RESTLINK_PLUGIN_IID** macro for the IID.
 */

/**
 * @brief Constructs a Plugin instance.
 * @param parent The parent QObject.
 */
Plugin::Plugin(QObject *parent)
    : QObject{parent}
{
}

/**
 * @brief Destroys the Plugin instance.
 */
Plugin::~Plugin()
{
}

/**
 * @brief Returns the plugin UUID from metadata.
 * @return UUID as a QString.
 */
QString Plugin::uuid() const
{
    return m_metaData.value("uuid").toString();
}

/**
 * @brief Returns the plugin name from metadata.
 * @return Name as a QString.
 */
QString Plugin::name() const
{
    return m_metaData.value("name").toString();
}

/**
 * @brief Returns the raw plugin metadata.
 * @return A QJsonObject containing metadata.
 */
QJsonObject Plugin::metaData() const
{
    return m_metaData;
}

/**
 * @brief Sets the plugin metadata.
 * @param metaData A QJsonObject containing metadata fields such as uuid and name.
 */
void Plugin::setMetaData(const QJsonObject &metaData)
{
    m_metaData = metaData;
}

} // namespace RestLink

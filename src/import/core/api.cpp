#include "api.h"

#include "utils.h"

namespace RestLink {
namespace Qml {

Api::Api(QObject *parent)
    : RestLink::Api(parent)
{
}

QUrl Api::configurationUrl() const
{
    return m_configUrl;
}

void Api::setConfigurationUrl(const QUrl &url)
{
    if (m_configUrl == url)
        return;

    m_configUrl = url;
    emit configurationUrlChanged();
}

QQmlListProperty<RequestParameter> Api::parameters()
{
    return QQmlListProperty<RequestParameter>(this, &m_parameters);
}

void Api::classBegin()
{
}

void Api::componentComplete()
{
    Utils::fillParameters(this, m_parameters);

    if (m_configUrl.isValid())
        configure(m_configUrl);
}

} // namespace Qml
} // namespace RestLink

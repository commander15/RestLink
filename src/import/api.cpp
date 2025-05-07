#include "api.h"

#include "request.h"

namespace RestLink {
namespace Qml {

Api::Api(QObject *parent)
    : RestLink::Api(parent)
    , m_initialized(false)
    , m_parametersProperty(this, &m_parameters)
{
    connect(this, &Api::configurationCompleted, this, [this] {
        m_initialized = true;
        emit ready();
    });
}

bool Api::isReady() const
{
    return m_initialized && url().isValid();
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

    //configure(url);
}

void Api::init()
{
    // Register parameters
    Request::fillParameters(this, this);

    if (m_configUrl.isValid())
        configure(m_configUrl);
    else {
        m_initialized = true;
        emit ready();
    }
}

} // namespace Qml
} // namespace RestLink

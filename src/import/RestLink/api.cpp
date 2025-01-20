#include "api.h"

#include "request.h"

namespace RestLink {
namespace Qml {

Api::Api(QObject *parent)
    : RestLink::Api(parent)
    , m_initialized(false)
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

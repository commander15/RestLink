#include "networkmanager.h"

#include <RestLink/cache.h>
#include <RestLink/cookiejar.h>

namespace RestLink {

/**
 * @class NetworkManager
 * @brief A custom network manager for handling HTTP requests and responses in RestLink.
 *
 * This class extends QNetworkAccessManager to provide additional functionality,
 * including custom caching and cookie management, as well as redirect policy control.
 * It is designed to be the core network handling component of RestLink.
 */

/**
 * @brief Constructs a NetworkManager with a specified parent.
 *
 * Initializes the network manager with a default cache and cookie jar.
 * Also sets the redirect policy to SameOriginRedirectPolicy to ensure
 * secure handling of redirections within the same origin.
 *
 * @param parent The parent object for this NetworkManager. Defaults to nullptr.
 */
NetworkManager::NetworkManager(QObject *parent)
    : QNetworkAccessManager{parent}
{
    setRedirectPolicy(QNetworkRequest::SameOriginRedirectPolicy);

    setCache(new Cache(this));
    setCookieJar(new CookieJar(this));
}

}

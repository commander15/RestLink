#include "cookiejar.h"

namespace RestLink {

/**
 * @class CookieJar
 * @brief A custom cookie jar for managing HTTP cookies in RestLink.
 *
 * This class extends QNetworkCookieJar to provide custom cookie handling
 * for the RestLink network manager. It allows for advanced control over
 * cookies in the HTTP requests and responses.
 */

/**
 * @brief Constructs a CookieJar with a specified parent.
 *
 * Initializes the custom cookie jar. The parent object will be passed
 * to the QNetworkCookieJar constructor to maintain proper ownership.
 *
 * @param parent The parent object for this CookieJar. Defaults to nullptr.
 */
CookieJar::CookieJar(QObject *parent)
    : QNetworkCookieJar(parent)
{
}

} // namespace RestLink

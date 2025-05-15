#include "cookiejar.h"
#include "cookiejar_p.h"

#include <RestLink/fileutils.h>

#include <QtCore/qfile.h>
#include <QtCore/qiodevice.h>
#include <QtCore/qstandardpaths.h>

#include <QtNetwork/qnetworkcookie.h>

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
    , d_ptr(new CookieJarPrivate(this))
{
    load();
}

CookieJar::~CookieJar()
{
}

bool CookieJar::load()
{
    return loadFromFile(d_ptr->fileName);
}

bool CookieJar::save()
{
    return saveToFile(d_ptr->fileName);
}

bool CookieJar::loadFromFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return false;
    return loadFromDevice(&file);
}

bool CookieJar::saveToFile(const QString &fileName) const
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;
    return saveToDevice(&file);
}

bool CookieJar::loadFromDevice(QIODevice *device)
{
    if (device->isReadable()) {
        loadFromData(device->readAll());
        return true;
    } else {
        return false;
    }
}

bool CookieJar::saveToDevice(QIODevice *device) const
{
    if (device->isWritable()) {
        QByteArray data;
        saveToData(&data);
        return device->write(data) != -1;
    } else {
        return false;
    }
}

void CookieJar::loadFromData(const QByteArray &data)
{
    const QList<QNetworkCookie> cookies = QNetworkCookie::parseCookies(data);
    setAllCookies(cookies);
}

void CookieJar::saveToData(QByteArray *data) const
{
    const QList<QNetworkCookie> cookies = allCookies();
    for (const QNetworkCookie &cookie : cookies)
        data->append(cookie.toRawForm(QNetworkCookie::Full) + '\n');
}

CookieJarPrivate::CookieJarPrivate(CookieJar *q)
    : fileName(FileUtils::generateCookieFile())
{
}

QString RestLink::CookieJarPrivate::generateCookieFile()
{
    static const QString cookiesDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/RestLink/Cookie";
    static int index = 0;
    return cookiesDir + '/' + QString::number(index++);
}

} // namespace RestLink

#ifndef RESTLINK_COOKIEJAR_H
#define RESTLINK_COOKIEJAR_H

#include <RestLink/global.h>

#include <QtNetwork/qnetworkcookiejar.h>

namespace RestLink {

class CookieJarPrivate;
class RESTLINK_EXPORT CookieJar : public QNetworkCookieJar
{
    Q_OBJECT

public:
    explicit CookieJar(QObject *parent = nullptr);
    virtual ~CookieJar();

    bool load();
    bool save();

    bool loadFromFile(const QString &fileName);
    bool saveToFile(const QString &fileName) const;

    bool loadFromDevice(QIODevice *device);
    bool saveToDevice(QIODevice *device) const;

    void loadFromData(const QByteArray &data);
    void saveToData(QByteArray *data) const;

private:
    QScopedPointer<CookieJarPrivate> d_ptr;
};

} // namespace RestLink

#endif // RESTLINK_COOKIEJAR_H

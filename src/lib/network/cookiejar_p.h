#ifndef RESTLINK_COOKIEJAR_P_H
#define RESTLINK_COOKIEJAR_P_H

#include "cookiejar.h"

namespace RestLink {

class CookieJarPrivate
{
public:
    CookieJarPrivate(CookieJar *q);

    static QString generateCookieFile();

    QString fileName;
};

}

#endif // RESTLINK_COOKIEJAR_P_H

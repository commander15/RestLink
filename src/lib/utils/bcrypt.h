#ifndef RESTLINK_BCRYPT_H
#define RESTLINK_BCRYPT_H

#include <RestLink/global.h>

namespace RestLink {

class RESTLINK_EXPORT Bcrypt
{
public:
    static bool validatePassword(const QString &password, const QString &hash);
    static QString generateHash(const QString &password, int rounds = 12);
};

} // namespace RestLink

#endif // RESTLINK_BCRYPT_H

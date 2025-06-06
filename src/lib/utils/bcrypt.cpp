#include "bcrypt.h"

#include <bcrypt.h>

#include <QtCore/qstring.h>

namespace RestLink {

bool Bcrypt::validatePassword(const QString &password, const QString &hash)
{
    return bcrypt::validatePassword(password.toStdString(), hash.toStdString());
}

QString Bcrypt::generateHash(const QString &password, int rounds)
{
    return QString::fromStdString(bcrypt::generateHash(password.toStdString(), rounds));
}

} // namespace RestLink

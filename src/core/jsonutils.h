#ifndef RESTLINK_JSONUTILS_H
#define RESTLINK_JSONUTILS_H

#include <RestLink/global.h>

class QJsonObject;
class QJsonArray;
class QJsonValue;

namespace RestLink {

class RESTLINK_EXPORT JsonUtils
{
public:
    static QString jsonToString(const QJsonObject &object);
    static QString jsonToString(const QJsonArray &array);
    static QString jsonToString(const QJsonValue &value);

    static QByteArray jsonToByteArray(const QJsonValue &value);
};

}

#endif // RESTLINK_JSONUTILS_H

#ifndef RESTLINK_JSONUTILS_H
#define RESTLINK_JSONUTILS_H

#include <RestLink/global.h>

class QJsonObject;
class QJsonArray;
class QJsonValue;

namespace RestLink {

class JsonUtils
{
public:
    static RESTLINK_EXPORT QString jsonToString(const QJsonObject &object);
    static RESTLINK_EXPORT QString jsonToString(const QJsonArray &array);
    static RESTLINK_EXPORT QString jsonToString(const QJsonValue &value);

    static RESTLINK_EXPORT QByteArray jsonToByteArray(const QJsonValue &value);
};

}

#endif // RESTLINK_JSONUTILS_H

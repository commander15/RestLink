#include "jsonutils.h"

#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>

namespace RestLink {

QString JsonUtils::jsonToString(const QJsonObject &object)
{
    QJsonDocument doc;
    doc.setObject(object);
    return doc.toJson(QJsonDocument::Compact);
}

QString JsonUtils::jsonToString(const QJsonArray &array)
{
    QJsonDocument doc;
    doc.setArray(array);
    return doc.toJson(QJsonDocument::Compact);
}

QString JsonUtils::jsonToString(const QJsonValue &value)
{
    return jsonToByteArray(value);
}

QByteArray JsonUtils::jsonToByteArray(const QJsonValue &value)
{
    QJsonDocument doc;
    if (value.isObject())
        doc.setObject(value.toObject());
    else if (value.isArray())
        doc.setArray(value.toArray());
    return doc.toJson(QJsonDocument::Compact);
}

}

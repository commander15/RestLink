#include "parseddata.h"

#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>
#include <QtCore/qjsonvalue.h>

namespace RestLink {
namespace Sql {

void ParsedData::beginParsing(const QJsonObject &object)
{
    s_objects.append(object);
    s_level = s_objects.count() - 1;
}

void ParsedData::endParsing()
{
    s_objects.removeLast();

    if (s_objects.isEmpty())
        s_level = 0;
    else
        s_level = s_objects.count() - 1;
}

void ParsedData::attribute(const QString &name, bool *boolean)
{
    attribute(name, false, boolean);
}

void ParsedData::attribute(const QString &name, const Callback<bool> &defaultCallback, bool *boolean)
{
    attribute(name, defaultCallback.exec(s_objects[s_level]), boolean);
}

void ParsedData::attribute(const QString &name, bool defaultValue, bool *boolean)
{
    if (s_objects[s_level].contains(name))
        *boolean = s_objects[s_level].value(name).toBool();
    else
        *boolean = defaultValue;
}

void ParsedData::attribute(const QString &name, QString *string)
{
    return attribute(name, QString(), string);
}

void ParsedData::attribute(const QString &name, const Callback<QString> &defaultCallback, QString *string)
{
    attribute(name, defaultCallback.exec(s_objects[s_level]), string);
}

void ParsedData::attribute(const QString &name, const QString &defaultValue, QString *string)
{
    if (s_objects[s_level].contains(name))
        *string = s_objects[s_level].value(name).toString();
    else
        *string = defaultValue;
}

void ParsedData::attribute(const QString &name, QStringList *list)
{
    attribute(name, QStringList(), list);
}

void ParsedData::attribute(const QString &name, const Callback<QStringList> &defaultCallback, QStringList *list)
{
    attribute(name, defaultCallback.exec(s_objects[s_level]), list);
}

void ParsedData::attribute(const QString &name, const QStringList &defaultValue, QStringList *list)
{
    if (!s_objects[s_level].contains(name)) {
        *list = defaultValue;
        return;
    }

    const QJsonArray array = s_objects[s_level].value(name).toArray();
    for (const QJsonValue &value : array)
        list->append(value.toString());
}

int ParsedData::s_level(0);
QList<QJsonObject> ParsedData::s_objects;

} // namespace Sql
} // namespace RestLink

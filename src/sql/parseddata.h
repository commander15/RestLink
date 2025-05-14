#ifndef PARSEDDATA_H
#define PARSEDDATA_H

#include <QtCore/qglobal.h>

#include <functional>

class QJsonObject;

namespace RestLink {
namespace Sql {

class ParsedData
{
public:
    template<typename T>
    class Callback {
    public:
        //Callback(const T &value) : m_callback([value] { return value; }) {}
        Callback(const std::function<T(const QJsonObject &object)> &callback)
            : m_callback(callback) {}

        T exec(const QJsonObject &object) const {
            return m_callback(object);
        }

    private:
        std::function<T(const QJsonObject &object)> m_callback;
    };

    virtual ~ParsedData() = default;

    static void beginParsing(const QJsonObject &object);
    static void endParsing();

    static void attribute(const QString &name, bool *boolean);
    static void attribute(const QString &name, const Callback<bool> &defaultCallback, bool *boolean);
    static void attribute(const QString &name, bool defaultValue, bool *boolean);

    static void attribute(const QString &name, QString *string);
    static void attribute(const QString &name, const Callback<QString> &defaultCallback, QString *string);
    static void attribute(const QString &name, const QString &defaultValue, QString *string);

    static void attribute(const QString &name, QStringList *list);
    static void attribute(const QString &name, const Callback<QStringList> &defaultCallback, QStringList *list);
    static void attribute(const QString &name, const QStringList &defaultValue, QStringList *list);

private:
    static int s_level;
    static QList<QJsonObject> s_objects;
};

} // namespace Sql
} // namespace RestLink

#endif // PARSEDDATA_H

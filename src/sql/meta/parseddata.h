#ifndef PARSEDDATA_H
#define PARSEDDATA_H

#include <global.h>

#include <QtCore/qglobal.h>

#include <functional>

class QJsonObject;
class QJsonArray;

namespace RestLink {
namespace Sql {

class SQL_EXPORT ParsedData
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

    virtual bool isValid() const;

    virtual void save(QJsonObject *object) const = 0;

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

    static void attribute(const QString &name, QVariantHash *hash);
    static void attribute(const QString &name, const Callback<QVariantHash> &defaultCallback, QVariantHash *hash);
    static void attribute(const QString &name, const QVariantHash &defaultValue, QVariantHash *hash);

    static void attribute(const QString &name, QJsonArray *array);
    static void attribute(const QString &name, const Callback<QJsonArray> &defaultCallback, QJsonArray *array);
    static void attribute(const QString &name, const QJsonArray &defaultValue, QJsonArray *array);

private:
    static int s_level;
    static QList<QJsonObject> s_objects;
};

} // namespace Sql
} // namespace RestLink

#endif // PARSEDDATA_H

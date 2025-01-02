#ifndef RESTLINK_HEADER_H
#define RESTLINK_HEADER_H

#include <RestLink/global.h>

#include <QtCore/qstring.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qvariant.h>
#include <QtCore/qdatetime.h>

namespace RestLink {

class RESTLINK_EXPORT Header
{
public:
    Header();
    Header(const QString &name, const QVariant &value);
    Header(const Header &other) = default;
    Header(Header &&other) = default;

    Header &operator=(const Header &other);

    QString name() const;
    void setName(const QString &name);

    QVariant value() const;
    void setValue(const QVariant &value);

    bool hasValue(const QVariant &value) const;
    void addValue(const QVariant &value);
    void removeValue(const QVariant &value);

    QVariantList values() const;
    void setValues(const QVariantList &values);

    bool isEnabled() const;
    void setEnabled(bool enabled = true);

    static Header fromJsonObject(const QJsonObject &object);
    QJsonObject toJsonObject() const;

private:
    QString m_name;
    QVariantList m_values;
    bool m_enabled;
};

typedef QList<Header> HeaderList;

}

#endif // RESTLINK_HEADER_H

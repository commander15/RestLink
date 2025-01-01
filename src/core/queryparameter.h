#ifndef RESTLINK_QUERYPARAMETER_H
#define RESTLINK_QUERYPARAMETER_H

#include <RestLink/global.h>

#include <QtCore/qshareddata.h>

class QJsonObject;

namespace RestLink {

class QueryParameterData;
class RESTLINK_EXPORT QueryParameter
{
public:
    enum ParameterFlag {
        NoFlag = 0x0,
        AuthenticationFlag = 0x1,
        SecretFlag = 0x2
    };
    Q_DECLARE_FLAGS(ParameterFlags, ParameterFlag)

    QueryParameter();
    QueryParameter(const QString &name, const QVariant &value);
    QueryParameter(const QueryParameter &other);
    ~QueryParameter();

    QueryParameter &operator=(const QueryParameter &other);
    QueryParameter &operator=(QueryParameter &&other);

    QString name() const;
    void setName(const QString &name);

    QVariant value() const;
    void setValue(const QVariant &value);

    bool hasValue(const QVariant &value) const;
    void addValue(const QVariant &value);
    void removeValue(const QVariant &value);

    QVariantList values() const;
    void setValues(const QVariantList &values);

    bool hasFlag(ParameterFlag flag) const;
    ParameterFlags flags() const;
    void setFlag(ParameterFlag flag, bool on = true);
    void setFlags(const ParameterFlags &flags);

    bool isEnabled() const;
    void setEnabled(bool enabled = true);

    bool isValid() const;

    QJsonObject toJsonObject() const;
    static QueryParameter fromJsonbject(const QJsonObject &object);

    void swap(QueryParameter &other);

    static QueryParameter merge(const QueryParameter &r1, const QueryParameter &r2);

private:
    QSharedDataPointer<QueryParameterData> d_ptr;
};

} // namespace RestLink

#endif // RESTLINK_QUERYPARAMETER_H

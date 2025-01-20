#ifndef RESTLINK_PARAMETER_H
#define RESTLINK_PARAMETER_H

#include <RestLink/global.h>

#include <QtCore/qshareddata.h>
#include <QtCore/qlist.h>

class QJsonObject;

namespace RestLink {

class ParameterData;
class RESTLINK_EXPORT Parameter
{
public:
    enum Flag {
        NoFlag = 0x0,
        Authentication = 0x1,
        Secret = 0x2
    };
    Q_DECLARE_FLAGS(Flags, Flag)

    enum Type {
        Base,
        PathParameter,
        QueryParameter,
        Header
    };

    Parameter();
    Parameter(const Parameter &other);
    Parameter(Parameter &&other);
    ~Parameter();

    Parameter &operator=(const Parameter &other);
    Parameter &operator=(Parameter &&other);

    QString name() const;
    void setName(const QString &name);

    QVariant value() const;
    void setValue(const QVariant &value);

    bool hasValue(const QVariant &value) const;
    void addValue(const QVariant &value);
    void removeValue(const QVariant &value);

    QList<QVariant> values() const;
    void setValues(const QList<QVariant> &values);

    bool hasFlag(Flag flag) const;
    Flags flags() const;
    void setFlag(Flag flag, bool on = true);
    void setFlags(const Flags &flags);

    //bool isPathParameter() const;
    //bool isQueryParameter() const;
    //bool isHeader() const;
    Type type() const;

    bool isValid() const;

    QJsonObject toJsonObject() const;
    static Parameter fromJsonObject(const QJsonObject &object, Type type);

    static Parameter merge(const Parameter &p1, const Parameter &p2);

    bool operator==(const Parameter &other) const;
    bool operator!=(const Parameter &other) const;

protected:
    Parameter(ParameterData *d);

    static void dataFromJsonObject(ParameterData *data, const QJsonObject &object);

    QSharedDataPointer<ParameterData> d_ptr;
};

} // namespace RestLink

#endif // RESTLINK_PARAMETER_H

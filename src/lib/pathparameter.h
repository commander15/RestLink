#ifndef RESTLINK_PATHPARAMETER_H
#define RESTLINK_PATHPARAMETER_H

#include <RestLink/global.h>

#include <QtCore/qvariant.h>

namespace RestLink {

class PathParameter
{
public:
    PathParameter();
    PathParameter(const QString &name, const QVariant &value);
    PathParameter(const PathParameter &other) = default;
    PathParameter(PathParameter &&other) = default;

    PathParameter &operator=(const PathParameter &other);
    PathParameter &operator=(PathParameter &&other);

    QString name() const;
    void setName(const QString &name);

    QVariant value() const;
    void setValue(const QVariant &value);

    bool isValid() const;

    bool operator==(const PathParameter &other);
    inline bool operator!=(const PathParameter &other)
    { return !operator==(other); }

private:
    QString m_name;
    QVariant m_value;
};

typedef QList<PathParameter> PathParameterList;

} // namespace RestLink

#endif // RESTLINK_PATHPARAMETER_H

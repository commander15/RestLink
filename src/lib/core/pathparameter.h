#ifndef RESTLINK_PATHPARAMETER_H
#define RESTLINK_PATHPARAMETER_H

#include <RestLink/global.h>
#include <RestLink/parameter.h>

namespace RestLink {

class PathParameterData;
class RESTLINK_EXPORT PathParameter : public Parameter
{
public:
    PathParameter();
    PathParameter(const QString &name, const QVariant &value);
    PathParameter(const PathParameter &other) = default;
    PathParameter(PathParameter &&other) = default;

    PathParameter &operator=(const PathParameter &other);
    PathParameter &operator=(PathParameter &&other);

    static PathParameter fromJsonObject(const QJsonObject &object);

    bool operator==(const PathParameter &other);
    inline bool operator!=(const PathParameter &other)
    { return !operator==(other); }

protected:
    PathParameter(const QSharedDataPointer<ParameterData> &d);

    friend class Parameter;
};

typedef QList<PathParameter> PathParameterList;

} // namespace RestLink

#endif // RESTLINK_PATHPARAMETER_H

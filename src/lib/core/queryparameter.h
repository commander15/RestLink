#ifndef RESTLINK_QUERYPARAMETER_H
#define RESTLINK_QUERYPARAMETER_H

#include <RestLink/global.h>
#include <RestLink/parameter.h>

namespace RestLink {

class QueryParameterData;
class RESTLINK_EXPORT QueryParameter : public Parameter
{
public:
    QueryParameter();
    QueryParameter(const QString &name, const QVariant &value);
    QueryParameter(const QueryParameter &other);
    ~QueryParameter();

    using Parameter::operator=;

    static QueryParameter fromJsonObject(const QJsonObject &object);

    using Parameter::operator==;
    using Parameter::operator!=;

protected:
    QueryParameter(const QSharedDataPointer<ParameterData> &d);

    friend class Parameter;
};

typedef QList<QueryParameter> QueryParameterList;

} // namespace RestLink

#endif // RESTLINK_QUERYPARAMETER_H

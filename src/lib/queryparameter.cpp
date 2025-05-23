#include "queryparameter.h"
#include "queryparameter_p.h"

#include <QtCore/qvariant.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>

namespace RestLink {

QueryParameter::QueryParameter()
    : Parameter(new QueryParameterData())
{
    setValue("");
}

QueryParameter::QueryParameter(const QString &name, const QVariant &value)
    : Parameter(new QueryParameterData())
{
    setName(name);
    setValue(value);
}

QueryParameter::QueryParameter(const QueryParameter &other)
    : Parameter(other)
{
}

QueryParameter::QueryParameter(const QSharedDataPointer<ParameterData> &d)
    : Parameter(d)
{
}

/*!
 * \brief Destructor for QueryParameter.
 *
 * Cleans up resources held by the QueryParameter object.
 */
QueryParameter::~QueryParameter()
{
}

QueryParameter QueryParameter::fromJsonObject(const QJsonObject &object)
{
    QueryParameter p;
    dataFromJsonObject(p.d_ptr, object);
    return p;
}

} // namespace RestLink

#include "utils.h"

#include "requestparameter.h"

#include <RestLink/requestinterface.h>
#include <RestLink/pathparameter.h>
#include <RestLink/queryparameter.h>
#include <RestLink/header.h>

namespace RestLink {
namespace Qml {

void Utils::fillParameters(RequestInterface *interface, const QList<RequestParameter *> &parameters)
{
    for (RequestParameter *parameterObject : parameters) {
        if (!parameterObject->isEnabled())
            continue;

        const Parameter parameter = parameterObject->parameter();

        switch (parameter.type()) {
        case Parameter::PathParameterType:
            interface->setPathParameter(parameter.toPathParameter());
            break;

        case Parameter::QueryParameterType:
            interface->addQueryParameter(parameter.toQueryParameter());
            break;

        case Parameter::HeaderType:
            interface->setHeader(parameter.toHeader());

        default:
            break;
        }
    }
}

} // namespace Qml
} // namespace RestLink

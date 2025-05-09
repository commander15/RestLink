#ifndef UTILS_H
#define UTILS_H

#include <QtCore/qglobal.h>

namespace RestLink {

class RequestInterface;

namespace Qml {

class RequestParameter;

class Utils
{
public:
    static void fillParameters(RequestInterface *interface, const QList<RequestParameter *> &parameters);
};

} // namespace Qml

} // namespace RestLink

#endif // UTILS_H

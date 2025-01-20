#ifndef RESTLINK_HEADER_H
#define RESTLINK_HEADER_H

#include <RestLink/global.h>
#include <RestLink/parameter.h>
#include <RestLink/parameterlist.h>

namespace RestLink {

class HeaderData;
class RESTLINK_EXPORT Header : public Parameter
{
public:
    Header();
    Header(const QString &name, const QVariant &value);
    Header(const Header &other) = default;
    Header(Header &&other) = default;

    Header &operator=(const Header &other);

    static Header fromJsonObject(const QJsonObject &object);

protected:
    Header(const QSharedDataPointer<ParameterData> &d);

    friend class Parameter;
};

typedef ParameterList<Header> HeaderList;

}

#endif // RESTLINK_HEADER_H

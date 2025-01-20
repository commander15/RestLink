#ifndef RESTLINK_PARAMETER_P_H
#define RESTLINK_PARAMETER_P_H

#include "parameter.h"

#include <QtCore/qshareddata.h>
#include <QtCore/qvariant.h>

namespace RestLink {

class ParameterData : public QSharedData
{
public:
    QString name;
    QVariantList values;
    Parameter::Flags flags;

    virtual QString validateName(const QString &name) const
    { return name; }

    virtual QVariant validateValue(const QVariant &value) const
    { return value; }

    QVariantList validateValues(const QVariantList &values) const
    {
        QVariantList validatedValues;
        for (const QVariant &value : values) {
            const QVariant validatedValue = validateValue(value);
            if (validatedValue.isValid())
                validatedValues.append(validatedValue);
        }
        return validatedValues;
    }

    Parameter::Flags validateFlags(Parameter::Flags flags) const
    {
        if (flags.testFlag(Parameter::Authentication) && !flags.testFlag(Parameter::Secret))
            flags.setFlag(Parameter::Secret);
        return flags;
    }

    virtual bool isValid() const
    { return !name.isEmpty() && !values.isEmpty(); }

    virtual Parameter::Type type() const
    { return Parameter::Base; }
};

} // namespace RestLink

#endif // RESTLINK_PARAMETER_P_H

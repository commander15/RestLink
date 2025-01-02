#ifndef RESTLINK_QUERYPARAMETER_P_H
#define RESTLINK_QUERYPARAMETER_P_H

#include "queryparameter.h"

#include <QtCore/qvariant.h>

namespace RestLink {

class QueryParameterData : public QSharedData
{
public:
    static QString validateName(const QString &name);
    static QVariantList validateValues(const QVariantList &values);
    static QueryParameter::ParameterFlags validateFlags(QueryParameter::ParameterFlags flags);

    QString name;
    QVariantList values;
    QueryParameter::ParameterFlags flags;
    bool enabled = true;
};

}

#endif // RESTLINK_QUERYPARAMETER_P_H

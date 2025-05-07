#ifndef REQUESTPARAMETER_H
#define REQUESTPARAMETER_H

#include <QtQml/qqmlengine.h>

#include <RestLink/parameter.h>

namespace RestLink {
namespace Qml {

class RequestParameter : public QObject
{
    QML_NAMED_ELEMENT(RequestParameterBase)
    QML_ADDED_IN_VERSION(1, 0)
    
    Q_OBJECT
    Q_PROPERTY(QString name MEMBER m_name NOTIFY nameChanged)
    Q_PROPERTY(QVariant value MEMBER m_value NOTIFY valueChanged)
    Q_PROPERTY(int type MEMBER m_type NOTIFY typeChanged)
    Q_PROPERTY(bool enabled MEMBER m_enabled NOTIFY enabledChanged)
    
public:
    enum ParameterType {
        Path = Parameter::PathParameterType,
        Query = Parameter::QueryParameterType,
        Header = Parameter::HeaderType
    };
    Q_ENUM(ParameterType)

    explicit RequestParameter(QObject *parent = nullptr);

    bool isEnabled() const;
    Parameter parameter() const;
    
signals:
    void nameChanged();
    void valueChanged();
    void typeChanged();
    void enabledChanged();
    
private:
    QString m_name;
    QVariant m_value;
    int m_type;
    bool m_enabled;
};

} // namespace Qml
} // namespace RestLink

#endif // REQUESTPARAMETER_H

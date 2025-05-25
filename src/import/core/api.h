#ifndef API_H
#define API_H

#include <RestLink/api.h>

#include <QtQml/qqmlengine.h>

#include "types.h"
#include "requestparameter.h"

namespace RestLink {
namespace Qml {

class Api : public RestLink::Api, public QQmlParserStatus
{
    QML_ELEMENT
    QML_EXTENDED(ForeignApi)
    QML_ADDED_IN_VERSION(1, 0)

    Q_INTERFACES(QQmlParserStatus)

    Q_OBJECT
    Q_PROPERTY(QUrl configurationUrl MEMBER m_configUrl NOTIFY configurationUrlChanged FINAL)
    Q_PROPERTY(QQmlListProperty<RestLink::Qml::RequestParameter> parameters READ parameters FINAL)

    Q_CLASSINFO("DefaultProperty", "parameters")

public:
    Api(QObject *parent = nullptr);

    QUrl configurationUrl() const;
    void setConfigurationUrl(const QUrl &url);
    Q_SIGNAL void configurationUrlChanged();

    QQmlListProperty<RequestParameter> parameters();

    void classBegin() override;
    void componentComplete() override;

private:
    QUrl m_configUrl;

    QList<RequestParameter *> m_parameters;
};

} // namespace Qml
} // namespace RestLink

#endif // API_H

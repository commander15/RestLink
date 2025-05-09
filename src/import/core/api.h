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
    Q_PROPERTY(bool ready READ isReady NOTIFY ready FINAL)
    Q_PROPERTY(QUrl configurationUrl MEMBER m_configUrl NOTIFY configurationUrlChanged FINAL)
    Q_PROPERTY(QQmlListProperty<RestLink::Qml::RequestParameter> parameters MEMBER m_parametersProperty FINAL)

    Q_CLASSINFO("DefaultProperty", "parameters")

public:
    Api(QObject *parent = nullptr);

    bool isReady() const;
    Q_SIGNAL void ready();

    QUrl configurationUrl() const;
    Q_SLOT void setConfigurationUrl(const QUrl &url);
    Q_SIGNAL void configurationUrlChanged();

    void classBegin() override;
    void componentComplete() override;

private:
    QUrl m_configUrl;
    bool m_ready;

    QList<RequestParameter *> m_parameters;
    QQmlListProperty<RequestParameter> m_parametersProperty;
};

} // namespace Qml
} // namespace RestLink

#endif // API_H

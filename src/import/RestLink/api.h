#ifndef API_H
#define API_H

#include <RestLink/api.h>

#include <QtQml/qqmlengine.h>

namespace RestLink {
namespace Qml {

struct _Api
{
    Q_GADGET
    QML_ELEMENT
    QML_FOREIGN(RestLink::Api)
};

class Api : public RestLink::Api
{
    QML_NAMED_ELEMENT(ApiBase)
    QML_ADDED_IN_VERSION(1, 0)
    QML_EXTENDED(_Api)

    Q_OBJECT
    Q_PROPERTY(QUrl configurationUrl MEMBER m_configUrl NOTIFY configurationUrlChanged)

public:
    Api(QObject *parent = nullptr);

    bool isReady() const;

public slots:
    void init();

signals:
    void configurationUrlChanged();
    void ready();

private:
    void registerParameters();

    QUrl m_configUrl;
    bool m_initialized;
};

} // namespace Qml
} // namespace RestLink

#endif // API_H

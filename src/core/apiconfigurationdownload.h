#ifndef RESTLINK_APICONFIGURATIONDOWNLOAD_H
#define RESTLINK_APICONFIGURATIONDOWNLOAD_H

#include <RestLink/global.h>
#include <RestLink/apireply.h>

#include <QtCore/qobject.h>

class QUrl;
class QNetworkReply;
class QNetworkAccessManager;

namespace RestLink {
class Api;

class ApiConfigurationDownloadPrivate;
class RESTLINK_EXPORT ApiConfigurationDownload : public ApiReply
{
    Q_OBJECT

public:
    ~ApiConfigurationDownload();

    bool isAutoConfigurable() const;
    Q_SLOT void enableAutoconfiguration();
    Q_SLOT void disableAutoConfiguration();

protected:
    explicit ApiConfigurationDownload(Api *api);

    void processData() override;

    friend class Api;
};

} // namespace RestLink

#endif // RESTLINK_APICONFIGURATIONDOWNLOAD_H

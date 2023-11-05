#include "apiconfigurationdownload.h"
#include "apiconfigurationdownload_p.h"

#include <RestLink/debug.h>
#include <RestLink/api.h>

#include <QtNetwork/qnetworkreply.h>

#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>

namespace RestLink {

ApiConfigurationDownload::ApiConfigurationDownload(Api *api) :
    ApiReply(new ApiConfigurationDownloadPrivate(this), api)
{
}

ApiConfigurationDownload::~ApiConfigurationDownload()
{
}

bool ApiConfigurationDownload::isAutoConfigurable() const
{
    RESTLINK_D(const ApiConfigurationDownload);
    return d->autoConfigure;
}

void ApiConfigurationDownload::enableAutoconfiguration()
{
    RESTLINK_D(ApiConfigurationDownload);
    d->autoConfigure = true;
}

void ApiConfigurationDownload::disableAutoConfiguration()
{
    RESTLINK_D(ApiConfigurationDownload);
    d->autoConfigure = false;
}

void ApiConfigurationDownload::processData()
{
    RESTLINK_D(ApiConfigurationDownload);

    if (d->autoConfigure) {
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(d->netData, &error);

        if (error.error == QJsonParseError::NoError)
            static_cast<Api *>(d->api)->configureApi(doc.object());
        else
            restlinkWarning() << "JSON error at offset " << error.offset << ": " << error.errorString();
    }

    ApiReply::processData();
}

ApiConfigurationDownloadPrivate::ApiConfigurationDownloadPrivate(ApiConfigurationDownload *qq) :
    ApiReplyPrivate(qq),
    autoConfigure(false)
{
}

ApiConfigurationDownloadPrivate::~ApiConfigurationDownloadPrivate()
{
}

}

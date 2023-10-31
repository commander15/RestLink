#include "restlinkapi.h"
#include "restlinkapirequest.h"

#include <RestLink/apirequest.h>
#include <RestLink/apiconfigurationdownload.h>

#include <QtNetwork/qnetworkaccessmanager.h>

#include <QtCore/qcoreevent.h>

RestLinkApi::RestLinkApi(QObject *parent) :
    RestLink::Api(parent),
    m_ready(true),
    m_cache(nullptr)
{
    connect(this, &RestLink::Api::apiConfigured, this, [this] {setReady(true);});
}

RestLinkApi::~RestLinkApi()
{
}

QUrl RestLinkApi::apiConfigurationUrl() const
{
    return m_apiConfigUrl;
}

void RestLinkApi::setApiConfigurationUrl(const QUrl &url)
{
    if (m_apiConfigUrl != url) {
        m_apiConfigUrl = url;
        emit apiConfigurationUrlChanged(url);

        RestLink::ApiConfigurationDownload *download = downloadApiConfiguration(url);
        download->enableAutoconfiguration();
        connect(download, &RestLink::ApiReply::finished, download, &QObject::deleteLater);

        setReady(false);
    }
}

QString RestLinkApi::cacheDir() const
{
    return (m_cache ? m_cache->cacheDirectory() : QString());
}

void RestLinkApi::setCacheDir(const QString &dir)
{
    if (!dir.isEmpty()) {
        if (!m_cache) {
            m_cache = new QNetworkDiskCache(this);
            networkAccessManager()->setCache(m_cache);
        }

        if (m_cache->cacheDirectory() != dir) {
            m_cache->setCacheDirectory(dir);
            emit cacheDirChanged();
        }
    } else if (m_cache) {
        networkAccessManager()->setCache(nullptr);
        delete m_cache;
        m_cache = nullptr;

        emit cacheDirChanged();
    }
}

bool RestLinkApi::isReady() const
{
    return m_ready;
}

void RestLinkApi::setReady(bool ready)
{
    if (m_ready != ready) {
        m_ready = ready;
        emit readyChanged();
    }
}

void RestLinkApi::registerParameters()
{
    addParameters(RestLinkApiRequest::apiRequestParameters(this));
}

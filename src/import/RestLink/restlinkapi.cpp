#include "restlinkapi.h"
#include "restlinkapirequest.h"

#include <RestLink/debug.h>
#include <RestLink/apirequest.h>
#include <RestLink/apiconfigurationdownload.h>
#include <RestLink/apicache.h>

#include <QtNetwork/qnetworkaccessmanager.h>

#include <QtCore/qcoreevent.h>
#include <QtCore/qstandardpaths.h>
#include <QtCore/qfile.h>

RestLinkApi::RestLinkApi(QObject *parent) :
    RestLink::Api(parent),
    m_ready(true),
    m_net(networkAccessManager()),
    m_cache(static_cast<RestLink::ApiCache*>(networkCache()))
{
    //connect(this, &RestLink::Api::apiNameChanged, this, &RestLinkApi::updateCacheDir);
    connect(this, &RestLink::Api::apiConfigured, this, [this] {setReady(true);});
    connect(this, &RestLink::Api::apiConfigurationFailed, this, [this](const QString &error) {
        restlinkWarning() << error;
        configureApi(m_apiConfigUrl);
    });
}

RestLinkApi::~RestLinkApi()
{
}

void RestLinkApi::init()
{
    registerParameters();
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

        if (url.isValid()) {
            configureApi(url);
            setReady(false);
        }
    }
}

int RestLinkApi::cacheSize() const
{
    return m_cache->maximumCacheSize() / (1024 * 1024);
}

void RestLinkApi::setCacheSize(int size)
{
    m_cache->setMaximumCacheSize(size * 1024 * 1024);
    emit cacheSizeChanged();
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

QString RestLinkApi::parameterName(int index) const
{
    return apiParameter(index).name();
}

QVariant RestLinkApi::parameterValue(int index) const
{
    return apiParameter(index).value();
}

int RestLinkApi::parameterScope(int index) const
{
    return apiParameter(index).scope();
}

bool RestLinkApi::isParameterEnabled(int index) const
{
    return apiParameter(index).isEnabled();
}

void RestLinkApi::registerParameters()
{
    addParameters(RestLinkApiRequest::apiRequestParameters(this));
}

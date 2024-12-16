#include "restlinkapi.h"
#include "restlinkapirequest.h"

#include <RestLink/debug.h>
#include <RestLink/apirequest.h>
#include <RestLink/apicache.h>

#include <QtNetwork/qnetworkaccessmanager.h>

#include <QtCore/qcoreevent.h>
#include <QtCore/qstandardpaths.h>
#include <QtCore/qfile.h>

RestLinkApi::RestLinkApi(QObject *parent) :
    RestLink::Api(parent),
    m_cache(nullptr)
{
    connect(this, &RestLink::Api::configurationCompleted, this, &RestLinkApi::ready);
}

RestLinkApi::~RestLinkApi()
{
}

void RestLinkApi::init()
{
    registerParameters();

    if (m_apiConfigUrl.isValid())
        configure(m_apiConfigUrl);
    else if (isReady())
        emit ready();
}

bool RestLinkApi::isReady() const
{
    return url().isValid();
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
    }
}

RestLink::ApiCache *RestLinkApi::cache() const
{
    return m_cache;
}

void RestLinkApi::setCache(RestLink::ApiCache *cache)
{
    if (m_cache != cache) {
        networkAccessManager()->setCache(cache);
        m_cache = cache;
        emit cacheChanged();
    }
}

QString RestLinkApi::parameterName(int index) const
{
    return parameter(index).name();
}

QVariant RestLinkApi::parameterValue(int index) const
{
    return parameter(index).value();
}

int RestLinkApi::parameterScope(int index) const
{
    return parameter(index).scope();
}

bool RestLinkApi::isParameterEnabled(int index) const
{
    return parameter(index).isEnabled();
}

void RestLinkApi::registerParameters()
{
    addParameters(RestLinkApiRequest::apiRequestParameters(this));
}

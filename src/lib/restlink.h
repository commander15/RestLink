#ifndef RESTLINK_H
#define RESTLINK_H

/*!
 * \file restlink.h
 * \brief Central include header for the RestLink library.
 *
 * This header includes all core components of the RestLink framework,
 * providing access to requests, responses, utilities, plugins, and handlers.
 * Include this file to use the full RestLink API in your project.
 */

#include <RestLink/api.h>

#include <RestLink/request.h>
#include <RestLink/pathparameter.h>
#include <RestLink/queryparameter.h>
#include <RestLink/header.h>
#include <RestLink/body.h>
#include <RestLink/response.h>

#include <RestLink/cache.h>
#include <RestLink/cookiejar.h>
#include <RestLink/networkmanager.h>

#include <RestLink/pluginmanager.h>

#include <RestLink/abstractrequestinterceptor.h>

#include <RestLink/httputils.h>
#include <RestLink/compressionutils.h>


#endif // RESTLINK_H

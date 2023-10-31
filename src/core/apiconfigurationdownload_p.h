#ifndef RESTLINK_APICONFIGURATIONDOWNLOAD_P_H
#define RESTLINK_APICONFIGURATIONDOWNLOAD_P_H

#include "apiconfigurationdownload.h"

#include <RestLink/private/apireply_p.h>

namespace RestLink {

class ApiConfigurationDownloadPrivate : public ApiReplyPrivate
{
public:
    ApiConfigurationDownloadPrivate(ApiConfigurationDownload *qq);
    ~ApiConfigurationDownloadPrivate();

    bool autoConfigure;
};

}

#endif // RESTLINK_APICONFIGURATIONDOWNLOAD_P_H

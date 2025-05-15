#ifndef RESTLINK_ABSTRACTREQUESTINTERCEPTOR_H
#define RESTLINK_ABSTRACTREQUESTINTERCEPTOR_H

#include <QtCore/qloggingcategory.h>

#include <RestLink/global.h>
#include <RestLink/abstractrequesthandler.h>

namespace RestLink {

class RESTLINK_EXPORT AbstractRequestInterceptor
{
public:
    virtual ~AbstractRequestInterceptor() = default;

    virtual void intercept(AbstractRequestHandler::Method method, Request &request, Body &body) = 0;
};

class RESTLINK_EXPORT LogRequestInterceptor : public AbstractRequestInterceptor
{
public:
    LogRequestInterceptor(const char *category, QtMsgType enableForLevel = QtDebugMsg);
    LogRequestInterceptor(QLoggingCategory *category);
    ~LogRequestInterceptor();

    void intercept(AbstractRequestHandler::Method method, Request &request, Body &body) override;

private:
    QLoggingCategory *m_category;
    bool m_deleteCategory;
};

}

#endif // RESTLINK_ABSTRACTREQUESTINTERCEPTOR_H

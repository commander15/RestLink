#ifndef RESTLINK_REQUESTINTERCEPTOR_H
#define RESTLINK_REQUESTINTERCEPTOR_H

#include <RestLink/global.h>
#include <RestLink/requesthandler.h>

#include <QtCore/qloggingcategory.h>

namespace RestLink {

class RESTLINK_EXPORT RequestInterceptor
{
public:
    virtual ~RequestInterceptor() = default;

    virtual Request intercept(RequestHandler::Method method, const Request &request, const Body &body) = 0;
};

class RESTLINK_EXPORT LogRequestInterceptor : public RequestInterceptor
{
public:
    LogRequestInterceptor(const char *category, QtMsgType enableForLevel = QtDebugMsg);
    LogRequestInterceptor(QLoggingCategory *category);
    ~LogRequestInterceptor();

    Request intercept(RequestHandler::Method method, const Request &request, const Body &body) override;

private:
    QLoggingCategory *m_category;
    bool m_deleteCategory;
};

}

#endif // RESTLINK_REQUESTINTERCEPTOR_H

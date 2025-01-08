#ifndef RESTLINK_SQLRESPONSE_H
#define RESTLINK_SQLRESPONSE_H

#include <RestLink/global.h>

#include <RestLink/private/internalresponse.h>

namespace RestLink {

class RESTLINK_EXPORT SqlResponse : public InternalResponse
{
    Q_OBJECT

public:
    ~SqlResponse();

    QString query() const;
    bool isSingleResult() const;
    void setQuery(const QString &query, bool singleResult);

protected:
    SqlResponse(Api *api);

    friend class SqlHandler;
};

} // namespace RestLink

#endif // RESTLINK_SQLRESPONSE_H

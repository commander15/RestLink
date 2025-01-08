#ifndef RESTLINK_SIMPLERESPONSE_H
#define RESTLINK_SIMPLERESPONSE_H

#include <RestLink/global.h>
#include <RestLink/response.h>

namespace RestLink {

class RESTLINK_EXPORT SimpleResponse : public Response
{
    Q_OBJECT

public:
    explicit SimpleResponse(Api *api);

    bool hasHeader(const QByteArray &name) const override;
    QByteArray header(const QByteArray &name) const override;
    QByteArrayList headerList() const override;
    void setHeaders(const QList<Header> &headers);

    QByteArray readBody() override;
    void setData(const QByteArray &data);
};

} // namespace RestLink

#endif // RESTLINK_SIMPLERESPONSE_H

#ifndef RESTLINK_COMPRESSIONUTILS_H
#define RESTLINK_COMPRESSIONUTILS_H

#include <RestLink/global.h>

namespace RestLink {

class RESTLINK_EXPORT CompressionUtils
{
public:
    static QByteArray decompress(const QByteArray &input, const QByteArray &algorithm);

#ifdef ZLIB_LIB
    static QByteArray decompressGzip(const QByteArray &input);
    static QByteArray decompressDeflate(const QByteArray &input);
#endif

    static QList<QByteArray> supportedAlgorithms();
};

}

#endif // RESTLINK_COMPRESSIONUTILS_H

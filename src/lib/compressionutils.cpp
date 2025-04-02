#include "compressionutils.h"

#include <QtCore/qbytearray.h>
#include <QtCore/qbytearraylist.h>

#ifdef ZLIB_LIB
#   include <zlib.h>
#endif

namespace RestLink {

QByteArray CompressionUtils::decompress(const QByteArray &input, const QByteArray &algorithm)
{
    if (algorithm.isEmpty())
        return input;
#ifdef ZLIB_LIB
    else if (algorithm == "gzip")
        return decompressGzip(input);
    else if (algorithm == "deflate")
        return decompressDeflate(input);
#endif
    else
        return input;
}

#ifdef ZLIB_LIB

QByteArray CompressionUtils::decompressGzip(const QByteArray &input)
{
    z_stream strm = {};
    inflateInit2(&strm, 16 + MAX_WBITS); // 16 + MAX_WBITS enables gzip decoding

    QByteArray decompressed(1024, Qt::Uninitialized);
    strm.next_in = (Bytef*)input.data();
    strm.avail_in = input.size();

    QByteArray output;
    do {
        strm.next_out = (Bytef*)decompressed.data();
        strm.avail_out = decompressed.size();
        inflate(&strm, Z_NO_FLUSH);
        output.append(decompressed.data(), decompressed.size() - strm.avail_out);

    } while (strm.avail_out == 0);

    inflateEnd(&strm);
    return output;
}

QByteArray CompressionUtils::decompressDeflate(const QByteArray &input)
{
    z_stream strm = {};
    inflateInit(&strm); // Only deflate decoding

    QByteArray decompressed(1024, Qt::Uninitialized);
    strm.next_in = (Bytef*)input.data();
    strm.avail_in = input.size();

    QByteArray output;
    do {
        strm.next_out = (Bytef*)decompressed.data();
        strm.avail_out = decompressed.size();
        inflate(&strm, Z_NO_FLUSH);
        output.append(decompressed.data(), decompressed.size() - strm.avail_out);
    } while (strm.avail_out == 0);

    inflateEnd(&strm);
    return output;
}

#endif

QList<QByteArray> CompressionUtils::supportedAlgorithms()
{
    QByteArrayList algorithms;

#ifdef ZLIB_LIB
    algorithms << "gzip" << "deflate";
#endif

    return algorithms;
}

}

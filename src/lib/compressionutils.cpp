#include "compressionutils.h"

#include <QtCore/qbytearray.h>
#include <QtCore/qbytearraylist.h>

#ifdef ZLIB_LIB
#   include <zlib.h>
#endif

namespace RestLink {

/*!
 * \class RestLink::CompressionUtils
 * \brief Provides utility methods for decompressing data using supported compression algorithms.
 *
 * This class supports decompressing data compressed using algorithms like gzip and deflate, leveraging
 * the zlib library when available. It includes methods for decompressing data streams based on the
 * specified algorithm and provides a list of supported algorithms.
 */

/*!
 * \brief Decompresses the input data based on the specified compression algorithm.
 *
 * The method checks the algorithm parameter and decompresses the data using the corresponding algorithm.
 * If the algorithm is not recognized, the method returns the original input data.
 *
 * \param input The compressed data.
 * \param algorithm The algorithm to use for decompression (e.g., "gzip" or "deflate").
 * \return The decompressed data.
 */
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

/*!
 * \brief Decompresses the input data using gzip compression.
 *
 * This method uses the zlib library to decompress data that is gzip compressed.
 *
 * \param input The compressed data.
 * \return The decompressed data.
 */
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

/*!
 * \brief Decompresses the input data using deflate compression.
 *
 * This method uses the zlib library to decompress data that is deflate compressed.
 *
 * \param input The compressed data.
 * \return The decompressed data.
 */
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

/*!
 * \brief Returns a list of supported compression algorithms.
 *
 * This method returns a list of compression algorithms that the library can handle,
 * based on the available libraries and the current system configuration.
 *
 * \return A list of supported algorithms as QByteArrays.
 */
QList<QByteArray> CompressionUtils::supportedAlgorithms()
{
    QByteArrayList algorithms;

#ifdef ZLIB_LIB
    algorithms << "gzip" << "deflate";
#endif

    return algorithms;
}

}

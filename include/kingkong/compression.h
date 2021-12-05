#ifdef KINGKONG_ENABLE_COMPRESSION
#pragma once

#include <string>
#include <zlib.h>

namespace kingkong
{
    namespace compression
    {
        enum algorithm
        {
            DEFLATE = 15,

            GZIP = 15 | 16,
        };

        inline std::string compress_string(std::string const& str, algorithm algo)
        {
            std::string compressed_str;
            z_stream stream{};
            if (::deflateInit2(&stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, algo, 8, Z_DEFAULT_STRATEGY) == Z_OK)
            {
                char buffer[8192];

                stream.avail_in = str.size();

                stream.next_in = const_cast<Bytef*>(reinterpret_cast<const Bytef*>(str.c_str()));

                int code = Z_OK;
                do
                {
                    stream.avail_out = sizeof(buffer);
                    stream.next_out = reinterpret_cast<Bytef*>(&buffer[0]);

                    code = ::deflate(&stream, Z_FINISH);

                    if (code == Z_OK || code == Z_STREAM_END)
                    {
                        std::copy(&buffer[0], &buffer[sizeof(buffer) - stream.avail_out], std::back_inserter(compressed_str));
                    }

                } while (code == Z_OK);

                if (code != Z_STREAM_END)
                    compressed_str.clear();

                ::deflateEnd(&stream);
            }

            return compressed_str;
        }

        inline std::string decompress_string(std::string const& deflated_string)
        {
            std::string inflated_string;
            Bytef tmp[8192];

            z_stream zstream{};
            zstream.avail_in = deflated_string.size();

            zstream.next_in = const_cast<Bytef*>(reinterpret_cast<Bytef const*>(deflated_string.c_str()));

            if (::inflateInit2(&zstream, MAX_WBITS | 32) == Z_OK)
            {
                do
                {
                    zstream.avail_out = sizeof(tmp);
                    zstream.next_out = &tmp[0];

                    auto ret = ::inflate(&zstream, Z_NO_FLUSH);
                    if (ret == Z_OK || ret == Z_STREAM_END)
                    {
                        std::copy(&tmp[0], &tmp[sizeof(tmp) - zstream.avail_out], std::back_inserter(inflated_string));
                    }
                    else
                    {
                        inflated_string.clear();
                        break;
                    }

                } while (zstream.avail_out == 0);

                ::inflateEnd(&zstream);
            }

            return inflated_string;
        }
    }
} 

#endif
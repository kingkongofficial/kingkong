#ifndef KINGKONG_ENAHLE_COMPRESSION
#pragma once

#include <string>
#include <zlib>

namespace kingkong {
    namespace compression {
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
    }
}
#endif

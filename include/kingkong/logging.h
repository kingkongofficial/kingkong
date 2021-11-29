#pragma once

#include "kingkong/settings.h"
#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

namespace kingkong {
    enum class LogLevel
    {
    #ifndef ERROR
    #ifndef DEBUG
        DEBUG = 0,
        INFO,
        WARNING,
        ERROR,
        CRITICAL,
    #endif
    #endif
        Debug = 0,
        Info,
        Warning,
        Error,
        Critical,
    };

    class ILogHandler
    {
    public:
        virtual void log(std::string message, LogLevel level) = 0;
    };

    class CerrLogHandler : public ILogHandler
    {
    public:
        void log(std::string message, LogLevel level) override
        {
            std::string prefix
            
            switch (level)
            {

            }
        }

    private:
        static std::string timestamp()
        {
            char data[32];
            time_t t = time(0);
            tm my_tm;

            size_t sz = strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", &my_tm);
            return std::string(date, date + sz);

        }
    };
}

#pragma once

#include "utils/exception/FeatherRuntimeException.hpp"
#include "utils/logger/LogSeverity.hpp"

#include <algorithm>
#include <fstream>
#include <mutex>
#include <iostream>
#include <sstream>

namespace feather::utils::logger
{
    /***************************************************************************************************
 * Logger is the actual class used for Logging, it uses LogPolicy as log output
***************************************************************************************************/
    template <typename LogPolicy>
    class Logger
    {
    public:
        template <LogSeverity level = DEBUG, typename... Args>
        void operator()(Args... args)
        {
#ifdef _DEBUG_MODE_
            loggingMutex.lock();
            switch (level)
            {
            case DEBUG:
                buffer << "<DEBUG>";
                break;
            case WARNING:
                buffer << "<WARNING>";
                break;
            case ERROR:
                buffer << "<ERROR>";
                break;
            }
            putTime();
            print_impl(args...);
#endif
        }

        void setMaxLogFileSize(unsigned long maxLogFileSize)
        {
#ifdef _DEBUG_MODE_
            static_cast<LogPolicy *>(this)->setMaxLogFileSize(maxLogFileSize);
#endif
        }

        ~Logger()
        {
        }

    protected:
        std::stringstream buffer;
        std::mutex loggingMutex;
        std::string d_time;

    private:
        static constexpr auto END_OF_LOGGING = "***END OF LOGGING***";

        void putTime()
        {
#ifdef _DEBUG_MODE_
            time_t raw_time;
            time(&raw_time);
            std::string localTime = ctime(&raw_time);
            localTime.erase(std::remove(localTime.begin(), localTime.end(), '\n'), localTime.end());
            buffer << localTime;
#endif
        }

        template <typename First, typename... Rest>
        void print_impl(First first, Rest... rest)
        {
#ifdef _DEBUG_MODE_
            buffer << " " << first;
            print_impl(rest...);
#endif
        }

        void print_impl()
        {
#ifdef _DEBUG_MODE_
            static_cast<LogPolicy *>(this)->write(buffer.str());
            buffer.str("");
#endif
        }
    };
} // namespace feather::utils::logger

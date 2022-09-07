#pragma once

#include "utils/logger/Logger.hpp"

#include <sstream>

namespace feather::utils::logger::policy
{
class MemoryLogPolicy : public Logger<MemoryLogPolicy>
{
    std::stringstream output;

public:
    void setMaxLogFileSize(unsigned long maxLogFileSizeArg)
    {
        //Ignore
    }

    void write(const std::string &content)
    {
        output << " " << content << std::endl;
        loggingMutex.unlock();
    }

    ~MemoryLogPolicy()
    {
        //print_impl(END_OF_LOGGING);
    }
};
} // namespace feather::utils::logger::policy

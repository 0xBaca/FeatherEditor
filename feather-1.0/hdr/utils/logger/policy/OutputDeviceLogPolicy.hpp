#pragma once

namespace feather::utils::logger::policy
{
class OutputDeviceLogPolicy : public Logger<OutputDeviceLogPolicy>
{
public:
    void setMaxLogFileSize(unsigned long maxLogFileSizeArg)
    {
        //Ignore
    }

    void write(const std::string &content)
    {
        std::cout << " " << content << std::endl;
        loggingMutex.unlock();
    }

    ~OutputDeviceLogPolicy()
    {
        //print_impl(END_OF_LOGGING);
    }
};
} // namespace feather::utils::logger::policy

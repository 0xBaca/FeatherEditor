#pragma once

#include "utils/logger/Logger.hpp"

#ifdef _DEBUG_MODE_
#include "boost/contract.hpp"
#endif

namespace feather::utils::logger::policy
{
    /***************************************************************************************************
     * FileLogPolicy is used as template argument of LogPolicy
     * is used as logging to file called log.txt which should be at the same directory as executable
     ***************************************************************************************************/
    class FileLogPolicy : public Logger<FileLogPolicy>
    {
    public:
        FileLogPolicy(std::string logFileName)
            : d_maxLogFileSize(DEFAULT_MAX_LOG_FILE_NAME)
        {
#ifdef _DEBUG_MODE_
            logFile.open(logFileName, std::ofstream::out | std::ofstream::app);
            if (!logFile.good())
            {
                throw utils::exception::FeatherRuntimeException("Failed to create policy object");
            }
#endif
        }

        void setMaxLogFileSize(unsigned long maxLogFileSizeArg)
        {
            d_maxLogFileSize = maxLogFileSizeArg;
        }

        void write(const std::string content)
        {
#ifdef _DEBUG_MODE_
            boost::contract::check fileStateIsGood = boost::contract::function()
                                                         .precondition([&]
                                                                       { BOOST_CONTRACT_ASSERT((logFile.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                         .postcondition([&]
                                                                        { BOOST_CONTRACT_ASSERT((logFile.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
            if (static_cast<size_t>(logFile.tellp()) + content.length() > d_maxLogFileSize)
            {
                logFile.seekp(0, std::ios::beg);
            }
            logFile << " " << content << std::endl;
            loggingMutex.unlock();
#endif
        }

    private:
        static constexpr size_t DEFAULT_MAX_LOG_FILE_NAME = 1;
        std::ofstream logFile;
        size_t d_maxLogFileSize;
    };
} // namespace feather::utils::logger::policy

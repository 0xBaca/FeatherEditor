#pragma once

#include "config/ReadMode.hpp"
#include "utils/Filesystem.hpp"
#include "utils/logger/policy/FileLogPolicy.hpp"

#include <any>
#include <map>
#include <memory>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

namespace feather::config
{
    namespace logPolicyNs = feather::utils::logger::policy;

    class Configuration
    {
    public:
        explicit Configuration(std::string const &, std::shared_ptr<logPolicyNs::FileLogPolicy>);
        static const std::string CONFIG_CACHE_SIZE;
        static const std::string CONFIG_COLOR;
        static const std::string CONFIG_LOCALE;
        static const std::string CONFIG_MODE;
        static const std::string CONFIG_MAX_LOG_SIZE;
        static const std::string CONFIG_RELAX_MEMORY;
        static const std::string CONFIG_STORAGE_DIRECTORY;
        static const std::string CONFIG_TAB_WIDTH;

        //parameter names
        static const std::string CONFIG_CACHE_ON_NAME;
        static const std::string CONFIG_CACHE_SIZE_NAME;
        static const std::string CONFIG_COLOR_NAME;
        static const std::string CONFIG_LOCALE_NAME;
        static const std::string CONFIG_MODE_NAME;
        static const std::string CONFIG_MAX_LOG_SIZE_NAME;
        static const std::string CONFIG_RELAX_MEMORY_NAME;
        static const std::string CONFIG_STORAGE_DIRECTORY_NAME;
        static const std::string CONFIG_TAB_WIDTH_NAME;

        //default values
        static const std::string DEFAULT_FILE_NAME;
        static const std::string DEFAULT_STORAGE_DIRECTORY;
        static const READ_MODE DEFAULT_MODE;
        static const bool DEFAULT_CACHE_ON;
        static const bool DEFAULT_COLOR;
        static const size_t DEFAULT_CACHE_SIZE;
        static const std::string DEFAULT_LOCALE;
        static const size_t DEFAULT_MAX_LOG_SIZE;
        static const size_t DEFAULT_MEMORY_RELAXED;
        static const size_t DEFAULT_TAB_WIDTH;
        static std::string getHomeDirectory();
        static std::string getLogFilePath();

        static std::map<std::string, std::any> configNameToDefaultValueMapping;

        bool isCacheOn() const;
        bool isForceLoadChanges() const;
        bool isHexMode() const;
        bool isTerminalSupportColors() const;
        void forceHexMode(std::string &) const;
        void forceTextMode(std::string &) const;
        void overrideConfiguration() const;
        std::string getConfigurationFileName() const;
        size_t getCacheSize() const;
        std::string getStorageDirectory() const;
        std::string getLocale() const;
        READ_MODE getMode() const;
        size_t getMaxLogFileSize() const;
        size_t getMemoryBytesRelaxed() const;
        size_t getTabWidth() const;
        ~Configuration();

    private:
        template <typename RetValue>
        void getProperty(std::string name, std::string const &jsonKey, RetValue &property)
        {
            try
            {
                if (jsonDocument.FindMember(jsonKey.c_str()) != jsonDocument.MemberEnd())
                {
                    setProperty(name, jsonDocument[jsonKey.c_str()], property);
                }
                else
                {
                    if (CONFIG_CACHE_SIZE == jsonKey)
                    {
                        cacheSize = DEFAULT_CACHE_SIZE;
                        (*log)(feather::utils::logger::INFO, "\tConfiguration key", jsonKey, "does not exist, setting default value", DEFAULT_CACHE_SIZE, "Bytes");
                    }
                    else if (CONFIG_STORAGE_DIRECTORY == jsonKey)
                    {
                        storageDirectory = DEFAULT_STORAGE_DIRECTORY;
                        (*log)(feather::utils::logger::INFO, "\tConfiguration key", jsonKey, "does not exist, setting default value", DEFAULT_STORAGE_DIRECTORY);
                    }
                    else if (CONFIG_LOCALE == jsonKey)
                    {
                        locale = DEFAULT_LOCALE;
                        (*log)(feather::utils::logger::INFO, "\tConfiguration key", jsonKey, "does not exist, setting default value", DEFAULT_LOCALE);
                    }
                    else if (CONFIG_MODE == jsonKey)
                    {
                        mode = DEFAULT_MODE;
                        (*log)(feather::utils::logger::INFO, "\tConfiguration key", jsonKey, "does not exist, setting default value", "READ_MODE");
                    }
                    else if (CONFIG_MAX_LOG_SIZE == jsonKey)
                    {
                        maxLogFileSize = DEFAULT_MAX_LOG_SIZE;
                        (*log)(feather::utils::logger::INFO, "\tConfiguration key", jsonKey, "does not exist, setting default value", DEFAULT_MAX_LOG_SIZE, "Bytes");
                    }
                    else if (CONFIG_RELAX_MEMORY == jsonKey)
                    {
                        memoryRelaxed = DEFAULT_MEMORY_RELAXED;
                        (*log)(feather::utils::logger::INFO, "\tConfiguration key", jsonKey, "does not exist, setting default value", DEFAULT_MEMORY_RELAXED, "Bytes");
                    }
                    else if (CONFIG_TAB_WIDTH == jsonKey)
                    {
                        (*log)(feather::utils::logger::INFO, "\tConfiguration key", jsonKey, "does not exist, setting default value", DEFAULT_TAB_WIDTH);
                        tabWidth = DEFAULT_TAB_WIDTH;
                    }
                }
            }
            catch (...)
            {
                (*log)(feather::utils::logger::INFO, "\tConfiguration key", jsonKey, "does not exist, skipping...\n");
            }
        }
        void parseConfigurationFile();
        rapidjson::Document jsonDocument;
        std::string configFilePath;
        mutable bool cacheOn;
        mutable bool forceLoadChanges;
        mutable size_t cacheSize;
        mutable size_t tabWidth;
        mutable size_t maxLogFileSize;
        mutable size_t memoryRelaxed;
        mutable std::string storageDirectory;
        mutable std::string locale;
        mutable READ_MODE mode;
        std::shared_ptr<logPolicyNs::FileLogPolicy> log;
        std::unique_ptr<utils::Filesystem> filesystem;
        Configuration(const Configuration &) = delete;
        Configuration &operator=(const Configuration &) = delete;
        void getDefaultConfiguration();
        void setProperty(std::string const &, rapidjson::Value const &, READ_MODE &);
        void setProperty(std::string const &, rapidjson::Value const &, std::string &);
        void setProperty(std::string const &, rapidjson::Value const &, bool &);
        void setProperty(std::string const &, rapidjson::Value const &, size_t &);
    };
} // namespace feather::config

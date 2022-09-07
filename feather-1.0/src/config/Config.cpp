#include "config/Config.hpp"
#include "utils/Filesystem.hpp"
#include "utils/ProgramOptionsParser.hpp"
#include "utils/exception/FeatherInvalidArgumnetException.hpp"
#include "utils/helpers/Lambda.hpp"

#include "rapidjson/istreamwrapper.h"

namespace feather::config
{
    Configuration::Configuration(std::string const &configFile, std::shared_ptr<logPolicyNs::FileLogPolicy> logArg)
        : configFilePath(configFile), cacheOn(false), cacheSize(0), maxLogFileSize(0), log(logArg), filesystem(std::make_unique<utils::Filesystem>())
    {
#ifdef _FEATHER_TEST_
        configFilePath = "./tst/ut/helpers/" + configFile;
#else
        configFilePath = Configuration::getHomeDirectory() + "/.feather/" + configFile;
#endif
        (*log)(feather::utils::logger::INFO, "\n");
        (*log)(feather::utils::logger::INFO, "*** STARTING FEATHER ***");
        (*log)(feather::utils::logger::INFO, "Maximum file size feather will handle on this machine", 1UL << ((sizeof(size_t) * 8UL) - 1UL), "Bytes!");
        (*log)(feather::utils::logger::INFO, "Reading configuration file", configFilePath);
        std::ifstream ifs(configFilePath);
        if (ifs.good())
        {
            rapidjson::IStreamWrapper isw(ifs);
            if (jsonDocument.ParseStream(isw).HasParseError())
            {
                (*log)(feather::utils::logger::INFO, "Improperly formatted configuration file", configFilePath, "at pos :", (unsigned)jsonDocument.GetErrorOffset(), ". Proceeding with default configuration");
                getDefaultConfiguration();
            }
            else
            {
                parseConfigurationFile();
            }
        }
        else
        {
            (*log)(feather::utils::logger::INFO, "Could not open configuration file", configFilePath, "proceeding with default configuration :");
            getDefaultConfiguration();
        }
    }

    void Configuration::getDefaultConfiguration()
    {
        this->configFilePath = DEFAULT_FILE_NAME;
        (*log)(feather::utils::logger::INFO, "\t", "Configuration file :", DEFAULT_FILE_NAME);
        this->cacheOn = DEFAULT_CACHE_ON;
        (*log)(feather::utils::logger::INFO, "\t", CONFIG_CACHE_ON_NAME, ":", DEFAULT_CACHE_ON ? "ENABLED" : "DISABLED");
        (*log)(feather::utils::logger::INFO, "\t", CONFIG_COLOR_NAME, ":", DEFAULT_COLOR ? "ENABLED" : "DISABLED");
        this->cacheSize = DEFAULT_CACHE_SIZE;
        (*log)(feather::utils::logger::INFO, "\t", CONFIG_CACHE_SIZE_NAME, ":", DEFAULT_CACHE_ON ? std::to_string(DEFAULT_CACHE_SIZE) : "N/A");
        this->storageDirectory = DEFAULT_STORAGE_DIRECTORY;
        (*log)(feather::utils::logger::INFO, "\t", CONFIG_STORAGE_DIRECTORY_NAME, ":", DEFAULT_STORAGE_DIRECTORY);
        this->locale = DEFAULT_LOCALE;
        (*log)(feather::utils::logger::INFO, "\t", CONFIG_LOCALE_NAME, ":", DEFAULT_LOCALE);
        this->mode = DEFAULT_MODE;
        (*log)(feather::utils::logger::INFO, "\t", CONFIG_MODE_NAME, ":", mode == READ_MODE::TEXT ? "TEXT" : "HEX");
        this->maxLogFileSize = DEFAULT_MAX_LOG_SIZE;
        (*log)(feather::utils::logger::INFO, "\t", CONFIG_MAX_LOG_SIZE_NAME, ":", std::to_string(DEFAULT_MAX_LOG_SIZE) + "B");
        this->memoryRelaxed = DEFAULT_MEMORY_RELAXED;
        (*log)(feather::utils::logger::INFO, "\t", CONFIG_RELAX_MEMORY_NAME, ":", std::to_string(DEFAULT_MEMORY_RELAXED) + "B");
        this->tabWidth = DEFAULT_TAB_WIDTH;
        (*log)(feather::utils::logger::INFO, "\t", CONFIG_TAB_WIDTH_NAME, ":", DEFAULT_TAB_WIDTH, "spaces");
    }

    size_t Configuration::getCacheSize() const
    {
        return cacheSize;
    }

    std::string Configuration::getStorageDirectory() const
    {
        return storageDirectory.back() != '/' ? storageDirectory + '/' : storageDirectory;
    }

    std::string Configuration::getConfigurationFileName() const
    {
        return configFilePath;
    }

    std::string Configuration::getHomeDirectory()
    {
        return std::string(std::getenv("HOME"));
    }

    std::string Configuration::getLocale() const
    {
        return locale;
    }

    std::string Configuration::getLogFilePath()
    {
        if (!std::make_unique<utils::Filesystem>()->isFolderExist(Configuration::getHomeDirectory() + "/.feather/"))
        {
            std::make_unique<utils::Filesystem>()->createDirectory(Configuration::getHomeDirectory() + "/", ".feather/");
        }
        return Configuration::getHomeDirectory() + "/.feather/feather.log";
    }

    READ_MODE Configuration::getMode() const
    {
        return mode;
    }

    size_t Configuration::getMaxLogFileSize() const
    {
        return maxLogFileSize;
    }

    size_t Configuration::getMemoryBytesRelaxed() const
    {
        return memoryRelaxed;
    }

    size_t Configuration::getTabWidth() const
    {
        return tabWidth;
    }

    void Configuration::parseConfigurationFile()
    {
        getProperty(CONFIG_CACHE_SIZE_NAME, CONFIG_CACHE_SIZE, cacheSize);
        getProperty(CONFIG_LOCALE_NAME, CONFIG_LOCALE, locale);
        getProperty(CONFIG_MODE_NAME, CONFIG_MODE, mode);
        getProperty(CONFIG_MAX_LOG_SIZE_NAME, CONFIG_MAX_LOG_SIZE, maxLogFileSize);
        getProperty(CONFIG_RELAX_MEMORY_NAME, CONFIG_RELAX_MEMORY, memoryRelaxed);
        getProperty(CONFIG_STORAGE_DIRECTORY_NAME, CONFIG_STORAGE_DIRECTORY, storageDirectory);
        getProperty(CONFIG_TAB_WIDTH_NAME, CONFIG_TAB_WIDTH, tabWidth);

        // Read key mappings
    }

    bool Configuration::isHexMode() const
    {
        return getMode() == READ_MODE::HEX;
    }

    bool Configuration::isTerminalSupportColors() const
    {
#ifndef _FEATHER_TEST_
        return has_colors();
#endif
        return true;
    }

    void Configuration::forceHexMode(std::string &fileName) const
    {
        (*log)(utils::logger::INFO, "\t", "Detected file", fileName, "as hex, changing mode respecively!");
        mode = READ_MODE::HEX;
    }

    void Configuration::forceTextMode(std::string &fileName) const
    {
        (*log)(utils::logger::INFO, "\t", "Detected file", fileName, "as text, changing mode respecively!");
        mode = READ_MODE::TEXT;
    }

    void Configuration::overrideConfiguration() const
    {
        if (auto cacheSizeProvided = utils::ProgramOptionsParser::getCache())
        {
            if ((cacheSizeProvided.value().second == "B" || cacheSizeProvided.value().second == "b") && cacheSizeProvided.value().first < 1024UL)
            {
                (*log)(utils::logger::INFO, "\t", "Can't set cache to less than 1KB, setting to 1KB");
                this->cacheSize = 1024UL;
            }
            else
            {
                (*log)(utils::logger::INFO, "\t", "Overriding", CONFIG_CACHE_SIZE_NAME, ":", cacheSizeProvided.value().first, cacheSizeProvided.value().second);
                cacheSize = utils::helpers::Lambda::convertToBytes(cacheSizeProvided.value().first, cacheSizeProvided.value().second);
            }
            cacheOn = true;
        }
        if (auto locale = utils::ProgramOptionsParser::getLocale())
        {
            (*log)(utils::logger::INFO, "\t", "Overriding", CONFIG_LOCALE_NAME, ":", locale.value());
            this->locale = locale.value();
        }
        if (auto maxLogFileSize = utils::ProgramOptionsParser::getLogSize())
        {
            if ((maxLogFileSize.value().second == "B" || maxLogFileSize.value().second == "b") && maxLogFileSize.value().first < 1024UL)
            {
                (*log)(utils::logger::INFO, "\t", "Can't set log size to less than 1KB, setting to 1KB");
            }
            (*log)(utils::logger::INFO, "\t", "Overriding", CONFIG_MAX_LOG_SIZE_NAME, ":", maxLogFileSize.value().first, maxLogFileSize.value().second);
            this->maxLogFileSize = utils::helpers::Lambda::convertToBytes(maxLogFileSize.value().first, maxLogFileSize.value().second);
        }
        if (auto memoryRelaxedProvided = utils::ProgramOptionsParser::getRelax())
        {
#ifndef _FEATHER_TEST_
            if ((memoryRelaxedProvided.value().second == "B" || memoryRelaxedProvided.value().second == "b") && memoryRelaxedProvided.value().first < 1024UL)
            {
                (*log)(utils::logger::INFO, "\t", "Can't set relaxation to less than 1KB, setting to 1KB");
                this->memoryRelaxed = 1024UL;
            }
            else
#endif
            {
                (*log)(utils::logger::INFO, "\t", "Overriding", CONFIG_RELAX_MEMORY_NAME, ":", memoryRelaxedProvided.value().first, memoryRelaxedProvided.value().second);
                this->memoryRelaxed = utils::helpers::Lambda::convertToBytes(memoryRelaxedProvided.value().first, memoryRelaxedProvided.value().second);
            }
        }
        if (auto storageDirectory = utils::ProgramOptionsParser::getStorage())
        {
            (*log)(utils::logger::INFO, "\t", "Overriding", CONFIG_STORAGE_DIRECTORY_NAME, ":", storageDirectory.value());
            this->storageDirectory = filesystem->getAbsolutePath(storageDirectory.value());
        }
        if (utils::ProgramOptionsParser::getExistingWindowUUID())
        {
            if (std::make_unique<utils::Filesystem>()->isFolderExist(this->getStorageDirectory() + "/" + utils::ProgramOptionsParser::getExistingWindowUUID()->getString()))
            {
                (*log)(utils::logger::INFO, "\t", "Loading existing UUID :", utils::ProgramOptionsParser::getExistingWindowUUID()->getString());
            }
        }
        if (auto tabWidth = utils::ProgramOptionsParser::getTab())
        {
            (*log)(utils::logger::INFO, "\t", "Overriding", CONFIG_TAB_WIDTH_NAME, ":", tabWidth.value().first, tabWidth.value().second);
            this->tabWidth = tabWidth.value().first;
        }
        if (utils::ProgramOptionsParser::isHex() && (this->mode != READ_MODE::HEX) && utils::ProgramOptionsParser::isHex().value())
        {
            (*log)(utils::logger::INFO, "\t", "Overriding", CONFIG_MODE_NAME, ": HEX");
            this->mode = READ_MODE::HEX;
        }
        else if (this->mode != READ_MODE::TEXT)
        {
            (*log)(utils::logger::INFO, "\t", "Overriding", CONFIG_MODE_NAME, ": TEXT");
            this->mode = READ_MODE::TEXT;
        }
        forceLoadChanges = utils::ProgramOptionsParser::isForceLoadChanges();
    }

    bool Configuration::isCacheOn() const
    {
        return cacheOn;
    }

    bool Configuration::isForceLoadChanges() const
    {
        return forceLoadChanges;
    }

    void Configuration::setProperty(std::string const &paramName, rapidjson::Value const &originParam, READ_MODE &value)
    {
        if (!originParam.IsNull() && "hex" == std::string(originParam.GetString()))
        {
            value = READ_MODE::HEX;
        }
        else
        {
            value = std::any_cast<READ_MODE>(configNameToDefaultValueMapping[paramName]);
        }
        (*log)(utils::logger::INFO, "\t", "Setting", paramName, "to", READ_MODE::TEXT == value ? "TEXT" : "HEX");
    }

    void Configuration::setProperty(std::string const &paramName, rapidjson::Value const &originParam, std::string &value)
    {
        if (!originParam.IsNull() && originParam.IsString() && !std::string(originParam.GetString()).empty())
        {
            value = originParam.GetString();
        }
        else
        {
            value = std::any_cast<std::string>(configNameToDefaultValueMapping[paramName]);
        }
        (*log)(utils::logger::INFO, "\t", "Setting", paramName, "to", value);
    }

    void Configuration::setProperty(std::string const &paramName, rapidjson::Value const &originParam, bool &value)
    {
        if (!originParam.IsNull() && originParam.IsBool() && (true == originParam.GetBool()))
        {
            value = true;
        }
        else
        {
            value = std::any_cast<bool>(configNameToDefaultValueMapping[paramName]);
        }
        (*log)(utils::logger::INFO, "\t", "Setting", paramName, "to", value ? "TRUE" : "FALSE");
    }

    void Configuration::setProperty(std::string const &paramName, rapidjson::Value const &originParam, size_t &value)
    {
        if (!originParam.IsNull() && originParam.IsString())
        {
            if (paramName == CONFIG_CACHE_SIZE_NAME || paramName == CONFIG_RELAX_MEMORY_NAME || paramName == CONFIG_MAX_LOG_SIZE_NAME)
            {
                try
                {
                    auto valueWithUnit = utils::helpers::Lambda::getValueWithUnit(originParam.GetString());
                    if ((valueWithUnit.second == "B" || valueWithUnit.second == "b") && valueWithUnit.first < 1024UL)
                    {
                        (*log)(utils::logger::INFO, "\t", "Can't set :", paramName, "value to less than 1KB, setting to default.");
                        value = std::any_cast<size_t>(configNameToDefaultValueMapping[paramName]);
                    }
                    else
                    {
                        value = utils::helpers::Lambda::convertToBytes(valueWithUnit.first, valueWithUnit.second);
                    }
                }
                catch (utils::exception::FeatherInvalidArgumentException const &e)
                {
                    (*log)(utils::logger::INFO, "\t", "Invalid value for :", paramName);
                    value = std::any_cast<size_t>(configNameToDefaultValueMapping[paramName]);
                }
                (*log)(utils::logger::INFO, "\t", "Setting", paramName, "to", value, "Bytes");
                return;
            }
        }
        else
        {
            value = std::any_cast<size_t>(configNameToDefaultValueMapping[paramName]);
        }
        if (paramName == CONFIG_CACHE_SIZE_NAME || paramName == CONFIG_RELAX_MEMORY_NAME || paramName == CONFIG_MAX_LOG_SIZE_NAME)
        {
            (*log)(utils::logger::INFO, "\t", "Setting", paramName, "to", value, "Bytes");
        }
        else
        {
            (*log)(utils::logger::INFO, "\t", "Setting", paramName, "to", value);
        }
    }

    Configuration::~Configuration()
    {
    }

    // Configuration file names
    const std::string Configuration::CONFIG_CACHE_SIZE = "cache";
    const std::string Configuration::CONFIG_COLOR = "color";
    const std::string Configuration::CONFIG_LOCALE = "locale";
    const std::string Configuration::CONFIG_MODE = "mode";
    const std::string Configuration::CONFIG_MAX_LOG_SIZE = "logSize";
    const std::string Configuration::CONFIG_RELAX_MEMORY = "relax";
    const std::string Configuration::CONFIG_STORAGE_DIRECTORY = "storage";
    const std::string Configuration::CONFIG_TAB_WIDTH = "tab";

    // Configuration name
    const std::string Configuration::CONFIG_CACHE_ON_NAME = "Cache";
    const std::string Configuration::CONFIG_CACHE_SIZE_NAME = "Cache size";
    const std::string Configuration::CONFIG_COLOR_NAME = "Colors";
    const std::string Configuration::CONFIG_LOCALE_NAME = "Locale";
    const std::string Configuration::CONFIG_MODE_NAME = "Mode";
    const std::string Configuration::CONFIG_MAX_LOG_SIZE_NAME = "Maximum log file size";
    const std::string Configuration::CONFIG_RELAX_MEMORY_NAME = "Memory relaxation";
    const std::string Configuration::CONFIG_STORAGE_DIRECTORY_NAME = "Storage directory";
    const std::string Configuration::CONFIG_TAB_WIDTH_NAME = "Tabulator to spaces";

    // Default values
    const std::string Configuration::DEFAULT_FILE_NAME = std::string("N/A");
    const std::string Configuration::DEFAULT_STORAGE_DIRECTORY = std::string("/tmp");
    const bool Configuration::DEFAULT_CACHE_ON = true;
    const bool Configuration::DEFAULT_COLOR = true;
    const size_t Configuration::DEFAULT_CACHE_SIZE = ONE_MEGABYTE;
    const size_t Configuration::DEFAULT_MAX_LOG_SIZE = 1024UL;
    const size_t Configuration::DEFAULT_MEMORY_RELAXED = ONE_MEGABYTE;
    const size_t Configuration::DEFAULT_TAB_WIDTH = 4UL;
    const std::string Configuration::DEFAULT_LOCALE = "";
    const READ_MODE Configuration::DEFAULT_MODE = READ_MODE::TEXT;

    std::map<std::string, std::any> Configuration::configNameToDefaultValueMapping = {
        {Configuration::CONFIG_CACHE_ON_NAME, Configuration::DEFAULT_CACHE_ON},
        {Configuration::CONFIG_CACHE_SIZE_NAME, Configuration::DEFAULT_CACHE_SIZE},
        {Configuration::CONFIG_COLOR_NAME, Configuration::DEFAULT_COLOR},
        {Configuration::CONFIG_LOCALE_NAME, Configuration::DEFAULT_LOCALE},
        {Configuration::CONFIG_MODE_NAME, Configuration::DEFAULT_MODE},
        {Configuration::CONFIG_MAX_LOG_SIZE_NAME, Configuration::DEFAULT_MAX_LOG_SIZE},
        {Configuration::CONFIG_RELAX_MEMORY_NAME, Configuration::DEFAULT_MEMORY_RELAXED},
        {Configuration::CONFIG_STORAGE_DIRECTORY_NAME, Configuration::DEFAULT_STORAGE_DIRECTORY},
        {Configuration::CONFIG_TAB_WIDTH_NAME, Configuration::DEFAULT_TAB_WIDTH}};
} // namespace feather::config

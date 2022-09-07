#include "utils/ProgramOptionsParser.hpp"
#include "utils/exception/FeatherInvalidArgumnetException.hpp"
#include "utils/helpers/Lambda.hpp"

#include <algorithm>
#include <cstring>
#include <iostream>

namespace feather::utils
{

    void ProgramOptionsParser::clear()
    {
        getCommandLineOptions() = {
            {"--cache", std::nullopt},
            {"--force", std::nullopt},
            //{"--locale", std::nullopt},
            {"--mode", std::nullopt},
            {"--relax", std::nullopt},
            {"--storage", std::nullopt},
            {"--tab", std::nullopt},
            {"--sessionId", std::nullopt}};
    }

    std::pair<size_t, std::string> ProgramOptionsParser::stringToLong(std::string const &arg, bool skipUnit)
    {
        if (arg.empty() || '-' == arg[0] || '0' == arg[0])
        {
            throw utils::exception::FeatherInvalidArgumentException("Invalid value : " + arg);
        }
        try
        {
            if (skipUnit)
            {
                return std::make_pair(std::stoull(arg), "");
            }
            return utils::helpers::Lambda::getValueWithUnit(arg);
        }
        catch (const std::exception &e)
        {
            throw utils::exception::FeatherInvalidArgumentException("Invalid value : " + arg);
        }
    }

    std::map<std::string, std::optional<std::string>> &ProgramOptionsParser::getCommandLineOptions()
    {
        static std::map<std::string, std::optional<std::string>> commandLineOptions = {
            {"--cache", std::nullopt},
            {"--force", std::nullopt},
            //{"--locale", std::nullopt},
            {"--mode", std::nullopt},
            {"--relax", std::nullopt},
            {"--storage", std::nullopt},
            {"--tab", std::nullopt},
            {"--sessionId", std::nullopt}};
        return commandLineOptions;
    }

    int ProgramOptionsParser::parseArguments(int argc, const char *argv[])
    {
        if (argc < 2)
        {
            showHelp();
            return -1;
        }
        else if (2 == argc && !strcmp(argv[1], "--version"))
        {
            std::cout << "Feather V 1.0" << std::endl;
            return -1;
        }
        else if (2 == argc && !strcmp(argv[1], "--help"))
        {
            showHelp();
            return -1;
        }
        for (int arg = 2; arg < argc; ++arg)
        {
            std::string currValue(argv[arg]);
            auto pivot = currValue.find_first_of('=');
            if (pivot != std::string::npos && getCommandLineOptions().count(currValue.substr(0, pivot)))
            {
                try
                {
                    getCommandLineOptions()[currValue.substr(0, pivot)] = currValue.substr(pivot + 1, strlen(argv[arg]));
                }
                catch (const std::exception &e)
                {
                    getCommandLineOptions()[currValue.substr(0, pivot)] = std::nullopt;
                }
            }
            else if (currValue == "--force")
            {
                getCommandLineOptions()["--force"] = "true";
            }
            else
            {
                printUnrecognizedArgument(std::string(argv[arg]));
                return -1;
            }
        }
        return 0;
    }

    std::optional<utils::datatypes::Uuid> ProgramOptionsParser::getExistingWindowUUID()
    {
        if (auto windowUUID = getCommandLineOptions()["--sessionId"])
        {
            return utils::datatypes::Uuid(windowUUID.value());
        }
        return std::nullopt;
    }

    std::optional<std::pair<size_t, std::string>> ProgramOptionsParser::getCache()
    {
        if (auto cacheOpt = getCommandLineOptions()["--cache"])
        {
            return stringToLong(getCommandLineOptions()["--cache"].value());
        }
        return std::nullopt;
    }

    std::optional<std::string> ProgramOptionsParser::getLocale()
    {
        //return getCommandLineOptions()["--locale"];
        return std::nullopt;
    }

    std::optional<std::pair<size_t, std::string>> ProgramOptionsParser::getLogSize()
    {
        /*
        Deprecated
        if (getCommandLineOptions()["--logSize"])
        {
            return stringToLong(getCommandLineOptions()["--logSize"].value());
        }
        */
        return std::nullopt;
    }

    std::optional<std::pair<size_t, std::string>> ProgramOptionsParser::getRelax()
    {
        if (getCommandLineOptions()["--relax"])
        {
            return stringToLong(getCommandLineOptions()["--relax"].value());
        }
        return std::nullopt;
    }

    std::optional<std::string> ProgramOptionsParser::getStorage()
    {
        return getCommandLineOptions()["--storage"];
    }

    std::optional<std::pair<size_t, std::string>> ProgramOptionsParser::getTab()
    {
        if (getCommandLineOptions()["--tab"])
        {
            return stringToLong(getCommandLineOptions()["--tab"].value(), true);
        }
        return std::nullopt;
    }

    std::optional<bool> ProgramOptionsParser::isHex()
    {
        if (getCommandLineOptions()["--mode"])
        {
            auto modeValue = getCommandLineOptions()["--mode"].value();
            if (modeValue.empty() || (modeValue != "text" && modeValue != "hex"))
            {
                throw utils::exception::FeatherInvalidArgumentException("Invalid value : " + modeValue);
            }
            return getCommandLineOptions()["--mode"].value() == "hex";
        }
        return std::nullopt;
    }

    bool ProgramOptionsParser::isForceLoadChanges()
    {
        return getCommandLineOptions()["--force"].has_value();
    }

    void ProgramOptionsParser::printUnrecognizedArgument(std::string const &argument)
    {
        std::cerr << "Illegal option : " << argument << std::endl;
    }

    void ProgramOptionsParser::showHelp()
    {
        std::cout << "Usage: feather [file] [--mode=[hex/text]] [--force]\n               [--relax=[num]] [--storage=[text]]\n               [--tab=[num]] [--sessionId=[text]]\n";
    }
} // namespace feather::utils

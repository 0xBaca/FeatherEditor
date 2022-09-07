#pragma once

#include "utils/datatypes/Global.hpp"
#include "utils/datatypes/Uuid.hpp"

#include <map>
#include <optional>

namespace feather::utils
{
    struct ProgramOptionsParser
    {
        static void clear();
        static std::pair<size_t, std::string> stringToLong(std::string const &, bool = false);
        static std::map<std::string, std::optional<std::string>> &getCommandLineOptions();
        static int parseArguments(int, const char *[]);
        static std::optional<utils::datatypes::Uuid> getExistingWindowUUID();
        static std::optional<std::pair<size_t, std::string>> getCache();
        static std::optional<std::string> getLocale();
        static std::optional<std::pair<size_t, std::string>> getLogSize();
        static std::optional<std::pair<size_t, std::string>> getRelax();
        static std::optional<std::string> getStorage();
        static std::optional<std::pair<size_t, std::string>> getTab();
        static std::optional<bool> isHex();
        static bool isNoColor();
        static bool isForceLoadChanges();
        static void printUnrecognizedArgument(std::string const &);
        static void showHelp();
    };
} // namespace feather::utils

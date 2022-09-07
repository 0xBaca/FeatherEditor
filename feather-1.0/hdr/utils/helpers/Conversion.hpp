#pragma once

#include <string>
#include <vector>

namespace feather::utils::helpers
{
    class Conversion
    {
    public:
        static char32_t vectorOfCharToChar32_t(std::vector<char> const &);
        static size_t u32StringTosize_t(std::u32string);
        static std::string squeezeu32String(std::u32string);
        static std::string stripSpecialCharacters(std::string);
        static std::string toHexString(char32_t);
        static std::string toHexString(std::u32string);
        static std::string toHiddenFile(std::string);
        static std::u32string escapeSlashCharacters(std::u32string const &);
        static std::u32string escapeSpecialCharacters(std::u32string &);
        static std::u32string getColoredStringHexMode(std::u32string const &);
        static std::u32string removeWhiteCharactersOnBothEnds(std::u32string const &);
        static std::u32string size_tToU32String(size_t);
        static std::u32string unescapeSlashes(std::u32string);
        static std::vector<char> char32ToVector(char32_t);
        static std::vector<std::u32string> splitString(std::u32string);
        static std::vector<std::u32string> splitStringWithQuote(std::u32string);
    };
} // namespace feather::utils::helpers

#include "utils/Utf8Util.hpp"

#include <iomanip>

namespace feather::utils::helpers
{
    std::vector<char> Conversion::char32ToVector(char32_t c)
    {
        char *c_ptr = reinterpret_cast<char *>(&c);
        std::vector<char> result(sizeof(char32_t), 0);
        for (size_t idx = 0; idx < sizeof(char32_t); ++idx)
        {
            result[idx] = (*c_ptr++);
        }
        return result;
    }

    std::u32string Conversion::escapeSlashCharacters(std::u32string const &string)
    {
        std::u32string toReturn;
        std::set<size_t> allPositions;
        size_t lastFoundPos = 0;
        while (std::string::npos != (lastFoundPos = string.find(U"\\/", lastFoundPos)))
        {
            allPositions.insert(lastFoundPos);
            ++lastFoundPos;
        }
        for (size_t idx = 0; idx < string.size();)
        {
            if (allPositions.count(idx))
            {
                toReturn.push_back('/');
                idx += 2UL;
            }
            else
            {
                toReturn.push_back(string[idx++]);
            }
        }
        return toReturn;
    }

    std::u32string Conversion::escapeSpecialCharacters(std::u32string &string)
    {
        // New lines
        std::u32string toReturn;
        std::set<size_t> allPositions;
        size_t lastFoundPos = 0;
        while (std::string::npos != (lastFoundPos = string.find(U"\\n", lastFoundPos)))
        {
            allPositions.insert(lastFoundPos);
            ++lastFoundPos;
        }
        for (size_t idx = 0; idx < string.size();)
        {
            if (allPositions.count(idx))
            {
                toReturn.push_back(0x0A);
                idx += 2UL;
            }
            else
            {
                toReturn.push_back(string[idx++]);
            }
        }

        // Tabs
        std::u32string toReturn2;
        allPositions.clear();
        lastFoundPos = 0;
        while (std::string::npos != (lastFoundPos = toReturn.find(U"\\t", lastFoundPos)))
        {
            allPositions.insert(lastFoundPos);
            ++lastFoundPos;
        }
        for (size_t idx = 0; idx < toReturn.size();)
        {
            if (allPositions.count(idx))
            {
                toReturn2.push_back(0x09);
                idx += 2UL;
            }
            else
            {
                toReturn2.push_back(toReturn[idx++]);
            }
        }

        // Backslashes
        std::u32string toReturn3;
        allPositions.clear();
        lastFoundPos = 0;
        while (std::string::npos != (lastFoundPos = toReturn2.find(U"\\\\", lastFoundPos)))
        {
            allPositions.insert(lastFoundPos);
            lastFoundPos += 2;
        }
        for (size_t idx = 0; idx < toReturn2.size();)
        {
            if (allPositions.count(idx))
            {
                toReturn3.push_back('\\');
                idx += 2UL;
            }
            else
            {
                toReturn3.push_back(toReturn2[idx++]);
            }
        }

        // Validation check, odd number of backslashes is invalid string
        size_t backslashCounter = 0;
        for (size_t idx = 0; idx < toReturn3.size(); ++idx)
        {
            if (toReturn3[idx] == '\\')
            {
                ++backslashCounter;
                if (backslashCounter == 3UL)
                {
                    return U"";
                }
            }
            else
            {
                backslashCounter = 0;
            }
        }
        return toReturn3;
    }

    std::u32string Conversion::removeWhiteCharactersOnBothEnds(std::u32string const &input)
    {
        auto firstNonBlank = input.begin(), lastNonBlank = input.end();
        while (utils::helpers::Lambda::isWhiteChar(*firstNonBlank))
        {
            ++firstNonBlank;
        }
        while (lastNonBlank > firstNonBlank && utils::helpers::Lambda::isWhiteChar(*std::prev(lastNonBlank)))
        {
            --lastNonBlank;
        }
        return {firstNonBlank, lastNonBlank};
    }

    std::string Conversion::toHiddenFile(std::string path)
    {
        if (std::string::npos == path.rfind('/'))
        {
            return '.' + path + ".fea";
        }
        std::string pathWithoutFileName = path.substr(0, path.rfind('/'));
        std::string fileName = path.substr(path.rfind('/') + 1UL);
        return pathWithoutFileName + "/." + fileName + ".fea";
    }

    std::vector<std::u32string> Conversion::splitString(std::u32string input)
    {
        std::vector<std::u32string> result;
        size_t currIdx = 0;
        std::u32string currString;
        while (currIdx != input.size())
        {
            if (input[currIdx] == U' ')
            {
                if (!currString.empty())
                {
                    result.push_back(currString);
                    currString.clear();
                }
            }
            else
            {
                currString.push_back(input[currIdx]);
            }
            ++currIdx;
        }
        if (!currString.empty())
        {
            result.push_back(currString);
        }
        return result;
    }

    std::vector<std::u32string> Conversion::splitStringWithQuote(std::u32string input)
    {
        input = unescapeSlashes(input);
        std::pair<int, int> escapedQuotes(-1, -1);
        auto getNoUnescapedQuotes = [&](char32_t quote)
        {
            short noEscapeQuotes = 0;
            for (size_t i = 0; i < input.size(); ++i)
            {
                if ((0 == i || input[i - 1] != '\\' || i == input.size() - 1) && (input[i] == quote))
                {
                    ++noEscapeQuotes;
                    if (escapedQuotes.first == -1)
                    {
                        escapedQuotes.first = i;
                    }
                    else
                    {
                        escapedQuotes.second = i;
                    }
                }
            }
            return noEscapeQuotes;
        };
        int unescapedDoubleQuotes = getNoUnescapedQuotes(U'\"');
        if (unescapedDoubleQuotes == 1 || unescapedDoubleQuotes > 2)
        {
            return {};
        }
        int unescapedSingleQuotes = getNoUnescapedQuotes(U'\'');
        if (unescapedSingleQuotes == 1 || unescapedSingleQuotes > 2)
        {
            return {};
        }

        if (!unescapedSingleQuotes && !unescapedDoubleQuotes)
        {
            auto splitted = splitString(input);
            if (splitted.size() > 3)
            {
                return {};
            }
            return splitted;
        }
        auto quotedString = input.substr(escapedQuotes.first + 1, escapedQuotes.second - (escapedQuotes.first + 1));
        input.erase(escapedQuotes.first - 1);
        auto result = splitString(input);
        if (!quotedString.empty())
        {
            result.push_back(quotedString);
        }
        return result;
    }

    std::string Conversion::stripSpecialCharacters(std::string input)
    {
        for (unsigned int i = 0; i < input.length(); ++i)
        {
            input.erase(std::remove(input.begin(), input.end(), '\''), input.end());
        }

        for (unsigned int i = 0; i < input.length(); ++i)
        {
            input.erase(std::remove(input.begin(), input.end(), '\\'), input.end());
        }
        return input;
    }

    std::u32string Conversion::size_tToU32String(size_t number)
    {
        std::u32string stringVal;
        if (0 == number)
        {
            stringVal.push_back(U'0');
        }
        else
        {
            while (number)
            {
                stringVal.push_back(U'0' + number % 10);
                number /= 10;
            }
        }
        std::reverse(stringVal.begin(), stringVal.end());
        return stringVal;
    }

    std::string Conversion::squeezeu32String(std::u32string string)
    {
        std::string result;
        for (char32_t c : string)
        {
            char *cPtr = (char *)&c;
            for (size_t len = 0; len < utils::Utf8Util::getCharacterBytesTakenLight(c, false); ++len)
            {
                result.push_back(*cPtr++);
            }
        }
        return result;
    }

    size_t Conversion::u32StringTosize_t(std::u32string number)
    {
        size_t base = 10ULL, currResult = 0ULL;
        for (char32_t e : number)
        {
            currResult = currResult * base + Utf8Util::getDigit(e);
        }
        return currResult;
    }

    std::u32string Conversion::unescapeSlashes(std::u32string string)
    {
        std::u32string result;
        if (string.empty())
        {
            return result;
        }
        for (size_t i = 0; i < string.length();)
        {
            if (string[i] == U'\\' && (i < string.length() - 1) && string[i + 1] == U'\\')
            {
                result.push_back(U'\\');
                i += 2;
            }
            else
            {
                result.push_back(string[i]);
                i += 1;
            }
        }
        return result;
    }

    std::string Conversion::toHexString(char32_t c)
    {
        std::stringstream hexStringStream;
        unsigned short charBytesTaken = utils::Utf8Util::getCharacterBytesTakenLight(c);
        hexStringStream << std::setfill('0') << std::setw(charBytesTaken * 2UL) << std::hex << std::uppercase << c;
        std::string hexString = hexStringStream.str();
        std::string resultString;
        for (int idx = hexString.size() - 2; idx >= 0; idx -= 2)
        {
            resultString.push_back(hexString[idx]);
            resultString.push_back(hexString[idx + 1UL]);
        }
        return resultString;
    }

    std::string Conversion::toHexString(std::u32string utf8String)
    {
        std::string result;
        for (unsigned char c : utils::Utf8Util::flatterUtf8String(utf8String))
        {
            std::stringstream ss;
            ss << std::hex << static_cast<unsigned int>(c);
            ss << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << (int)(unsigned char)c;
            result.push_back(ss.str()[0]);
            result.push_back(ss.str()[1]);
        }
        return result;
    }

    std::u32string Conversion::getColoredStringHexMode(std::u32string const &input)
    {
        std::u32string wordToColor;
        for (size_t i = 0; i < input.size(); ++i)
        {
            if (i != 0 && !(i % 2))
            {
                wordToColor.push_back(U' ');
            }
            wordToColor.push_back(std::toupper(input[i]));
        }
        return wordToColor;
    }

    char32_t Conversion::vectorOfCharToChar32_t(std::vector<char> const &input)
    {
        char32_t result = 0;
        if (input.empty())
        {
            return result;
        }
        size_t charBytesTaken = utils::Utf8Util::getCharacterBytesTakenEncoded(input[0]);
        char *resultPtr = (char *)(&result);
        for (size_t idx = 0; idx < charBytesTaken; ++idx)
        {
            *resultPtr++ = input[idx];
        }
        return result;
    }
} // namespace feather::utils::helpers

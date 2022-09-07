#pragma once

#include "config/Config.hpp"
#include "utils/helpers/Conversion.hpp"
#include "utils/helpers/Lambda.hpp"

#include <codecvt>
#include <list>
#include <locale>
#include <numeric>

#include <curses.h>

extern std::chrono::time_point<std::chrono::steady_clock> lastKeyPressed;
extern std::unique_ptr<const feather::config::Configuration> configuration;
extern std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> toCodePointConverter;
extern std::wstring_convert<std::codecvt_utf8<wchar_t>> toUtf8BytesConverter;
extern bool isHexMode;

namespace feather::utils
{
  struct Utf8Util
  {
    inline static std::vector<char> flatterUtf8String(std::u32string &string, bool isCallerMainWindow = true)
    {
      std::vector<char> flatteredString;
      for (char32_t e : string)
      {
        size_t charLength = getCharacterBytesTakenLight(e, isCallerMainWindow);
        for (char *c = reinterpret_cast<char *>(&e); charLength--; c++)
        {
          flatteredString.push_back(*c);
        }
      }
      return flatteredString;
    }

    inline static unsigned short getCharacterBytesTakenEncoded(char c)
    {
      if ((c & 0xF0) == 0xF0)
      {
        return 4;
      }
      else if ((c & 0xE0) == 0xE0)
      {
        return 3;
      }
      else if ((c & 0xC0) == 0xC0)
      {
        return 2;
      }
      return 1;
    }

    template<typename C>
    inline static unsigned short getCharacterBytesTaken(C probe, bool isCallerMainWindow = true)
    {
      // In hex mode we only print ascii characters and all the others are treated as if would take 1B.
      if (probe.empty())
      {
        return 0;
      }
      else if (isHexMode && isCallerMainWindow)
      {
        return 1;
      }
      for (size_t currIdx = 0; currIdx < probe.size(); ++currIdx)
      {
        unsigned short length = getCharacterBytesTakenEncoded(probe[0]);
        if (length > probe.size())
        {
          return 1UL;
        }
        // Sort out fake UTF-8 characters
        unsigned short idx = 1UL;
        for (; idx < length && isMiddleOfCharacter(probe[idx], isCallerMainWindow); ++idx)
          ;
        if (idx == length)
        {
          char32_t candidateCharacter = 0;
          char *c_ptr = reinterpret_cast<char *>(&candidateCharacter);
          for (int idx = 0; idx < length; ++idx)
          {
            *c_ptr++ = probe[idx];
          }
          if (isValidUtf8Character(candidateCharacter))
          {
            return length;
          }
        }
      }
      return 1;
    }

    inline static unsigned short getCharacterBytesTakenLight(char32_t probe, bool isCallerMainWindow = true)
    {
      // In hex mode we only print ascii characters and all the others are treated as if would take 1B.
      if (0 == probe)
      {
        return 1;
      }
      else if (isHexMode && isCallerMainWindow)
      {
        return 1;
      }
      for (size_t currIdx = 0; currIdx < sizeof(char32_t); ++currIdx)
      {
        unsigned short length = getCharacterBytesTakenEncoded(*((char*)&probe));
        if (length > sizeof(char32_t))
        {
          return 1UL;
        }
        // Sort out fake UTF-8 characters
        unsigned short idx = 1UL;
        char *probePtr = (char*)&probe;
        ++probePtr;
        for (; idx < length && isMiddleOfCharacter(*probePtr, isCallerMainWindow); ++idx)
        {
          ++probePtr;
        }
        if (idx == length)
        {
          char32_t candidateCharacter = 0;
          char *c_ptr = reinterpret_cast<char *>(&candidateCharacter);
          char *probePtr = reinterpret_cast<char *>(&probe);
          for (int idx = 0; idx < length; ++idx)
          {
            *c_ptr++ = *probePtr++;
          }
          if (isValidUtf8Character(candidateCharacter))
          {
            return length;
          }
        }
      }
      return 1;
    }

    inline static unsigned short getCharacterBytesTakenBackward(std::vector<char> &&probe)
    {
      // In hex mode we only print ascii characters and all the others are treated as if would take 1B.
      if (isHexMode)
      {
        return 1;
      }
      size_t longestUtf8Character = 1UL;
      for (int currIdx = probe.size() - 1UL; currIdx >= 0; --currIdx)
      {
        size_t length = getCharacterBytesTakenEncoded(probe[currIdx]);
        if (length + currIdx != probe.size())
        {
          continue;
        }
        // Sort out fake UTF-8 characters
        unsigned short idx = currIdx + 1UL;
        for (; idx < probe.size() && isMiddleOfCharacter(probe[idx]); ++idx)
          ;
        if (idx == probe.size())
        {
          char32_t candidateCharacter = 0;
          char *c_ptr = reinterpret_cast<char *>(&candidateCharacter);
          for (size_t idx = currIdx; idx < probe.size(); ++idx)
          {
            *c_ptr++ = probe[idx];
          }
          if (isValidUtf8Character(candidateCharacter))
          {
            longestUtf8Character = std::max(longestUtf8Character, length);
          }
        }
      }
      return longestUtf8Character;
    }

    inline static bool isAsciiControlCharacter(int c)
    {
      return ((c & 1 << 7) || (c == KEY_RESIZE)) && (c != KEY_UP) && (c != KEY_DOWN) && (c != KEY_LEFT) && (c != KEY_RIGHT);
    }

    inline static wchar_t getCodePoint(char32_t utf8Char, bool isCallerMainWindow)
    {
      if ((isHexMode && isCallerMainWindow) || utf8Char < static_cast<char32_t>(128))
      {
        return utf8Char;
      }

      wchar_t codePoint = 0;
      try
      {
        codePoint = toCodePointConverter.from_bytes((char *)&utf8Char, (char *)&utf8Char + getCharacterBytesTakenLight(utf8Char, isCallerMainWindow))[0];
      }
      catch (std::range_error &e)
      {
        codePoint = -1;
      }
      return codePoint;
    }

    inline static std::string getBytesFromWideString(std::wstring const &wideString)
    {
      try
      {
        return toUtf8BytesConverter.to_bytes(wideString);
      }
      catch (std::range_error &e)
      {
      }
      return "";
    }

    inline static size_t getFirstValidUtf8ChraracterPosition(std::vector<char> const &buffer)
    {
      size_t pos = 0UL;
      while (pos != buffer.size() && isMiddleOfCharacter(buffer[pos]))
      {
        ++pos;
      }
      return pos;
    }

    inline static size_t getPrevCharacterStartPos(std::vector<char> const &buffer, size_t pos)
    {
      if (!pos)
      {
        return pos;
      }
      return buffer.size() - getCharacterBytesTakenBackward((pos <= 4UL) ? std::vector<char>{buffer.begin(), std::next(buffer.begin(), pos)} : utils::helpers::Lambda::getSubVector(buffer, pos, 4UL));
    }

    inline static int getDigit(char32_t c)
    {
      return (char)c - '0';
    }

    inline static bool isBeginingOfCharacter(char c, bool ignoreHexMode = false)
    {
      if ((isHexMode && !ignoreHexMode) || (0 == c))
      {
        return true;
      }
      return !isMiddleOfCharacter(c);
    }

    inline static bool isDigit(char32_t c, bool isCallerMainWindow = false)
    {
      return (1ULL == getCharacterBytesTakenLight(c, isCallerMainWindow)) && std::isdigit((char)c);
    }

    // Middle of chraracter starts with leftmost bits 10
    inline static bool isMiddleOfCharacter(char c, bool isCallerMainWindow = true)
    {
      return (!isHexMode || !isCallerMainWindow) && ((c & 0xC0) == 0x80);
    }

    inline static std::vector<char> mapChar32ToChar(char32_t c, bool isCallerMainWindow = true)
    {
      size_t charBytesTaken = getCharacterBytesTakenLight(c, isCallerMainWindow);
      std::vector<char> result(charBytesTaken, 0);
      char *cPtr = reinterpret_cast<char *>(&c);
      for (size_t currByte = 0; currByte < charBytesTaken; ++currByte)
      {
        result[currByte] = *cPtr++;
      }
      return result;
    }

    inline static bool isValidUtf8Character(char32_t c)
    {
      if (0 == c)
      {
        return false;
      }
      const unsigned char *bytes = reinterpret_cast<unsigned char *>(&c);
      unsigned int cp;
      int num;

      if ((*bytes & 0x80) == 0x00)
      {
        // U+0000 to U+007F
        cp = (*bytes & 0x7F);
        num = 1;
      }
      else if ((*bytes & 0xE0) == 0xC0)
      {
        // U+0080 to U+07FF
        cp = (*bytes & 0x1F);
        num = 2;
      }
      else if ((*bytes & 0xF0) == 0xE0)
      {
        // U+0800 to U+FFFF
        cp = (*bytes & 0x0F);
        num = 3;
      }
      else if ((*bytes & 0xF8) == 0xF0)
      {
        // U+10000 to U+10FFFF
        cp = (*bytes & 0x07);
        num = 4;
      }
      else
        return false;

      // Middle check if all other bytes are zero
      unsigned char const *byte_ptr = bytes + num;
      unsigned short zeroBytes = sizeof(char32_t) - num;
      while (zeroBytes--)
      {
        if (*byte_ptr++)
        {
          return false;
        }
      }

      bytes += 1;
      for (int i = 1; i < num; ++i)
      {
        if ((*bytes & 0xC0) != 0x80)
          return false;
        cp = (cp << 6) | (*bytes & 0x3F);
        bytes += 1;
      }

      if ((cp > 0x10FFFF) ||
          ((cp >= 0xD800) && (cp <= 0xDFFF)) ||
          ((cp <= 0x007F) && (num != 1)) ||
          ((cp >= 0x0080) && (cp <= 0x07FF) && (num != 2)) ||
          ((cp >= 0x0800) && (cp <= 0xFFFF) && (num != 3)) ||
          ((cp >= 0x10000) && (cp <= 0x1FFFFF) && (num != 4)))
      {
        return false;
      }
      return true;
    }

    inline static char32_t getUtf8Character(std::string const &buffer, size_t startIdx, bool isCallerMainWindow = true)
    {
      char32_t utf8Character = 0;
      char *cPtr = reinterpret_cast<char *>(&utf8Character);
      size_t charBytesTaken = getCharacterBytesTaken(utils::helpers::Lambda::getSubVectorFromString(buffer, startIdx, 4UL), isCallerMainWindow);
      while (charBytesTaken--)
      {
        *cPtr++ = buffer[startIdx++];
      }
      return utf8Character;
    }

    inline static char32_t codepointToUtf8(char32_t codepoint)
    {
      std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
      return getUtf8Character(convert.to_bytes(&codepoint, &codepoint + 1), 0);
    }
  };
} // namespace feather::utils
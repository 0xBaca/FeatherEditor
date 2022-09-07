#include <utils/algorithm/RegexSearch.hpp>
#include <utils/Utf8Util.hpp>

namespace feather::utils::algorithm
{
    std::u32string RegexSearch::preprocess(std::u32string const &input)
    {
        std::u32string preprocessed;
        for (size_t idx = 0; idx < input.size(); ++idx)
        {
            if (U'^' == input[idx] && (idx == 0))
            {
                preprocessed += U"(^|\n)";
            }
            else if (U'$' == input[idx] && (idx == 0 || input[idx - 1] != U'\\'))
            {
                preprocessed += U"(?=\n|$)";
            }
            else
            {
                preprocessed += input[idx];
            }
        }
        return preprocessed;
    }

    std::string RegexSearch::preprocess(std::string const &input)
    {
        std::string preprocessed;
        for (size_t idx = 0; idx < input.size(); ++idx)
        {
            if (U'^' == input[idx] && (idx == 0 || input[idx - 1] != U'\\'))
            {
                preprocessed += "(^|\n)";
            }
            else if (U'$' == input[idx] && (idx == 0 || input[idx - 1] != U'\\'))
            {
                preprocessed += "(?=\n|$)";
            }
            else
            {
                preprocessed += input[idx];
            }
        }
        return preprocessed;
    }

    RegexSearch::RegexSearch(std::u32string const &searchBufferArg)
    {
        std::transform(searchBufferArg.begin(), searchBufferArg.end(), std::back_inserter(searchRegexBuffer), [&](char32_t const &c)
                       { return utils::Utf8Util::getCodePoint(c, true); });
    }

    std::set<std::u32string> RegexSearch::search(std::u32string const &searchStringArg)
    {
        std::set<std::u32string> result;
        try
        {
            std::wstring searchString;
            auto preprocessedString = preprocess(searchStringArg);
            std::transform(preprocessedString.begin(), preprocessedString.end(), std::back_inserter(searchString), [&](char32_t const &c)
                           { return utils::Utf8Util::getCodePoint(c, true); });
            this->searchRegex = std::wregex(searchString);

            std::set<std::wstring> toReturn;
            auto results_begin = std::wsregex_iterator(searchRegexBuffer.begin(), searchRegexBuffer.end(), searchRegex, std::regex_constants::match_not_eol | std::regex_constants::match_not_bol);
            auto results_end = std::wsregex_iterator();

            for (std::wsregex_iterator i = results_begin; i != results_end; ++i)
            {
                result.insert(utils::helpers::Lambda::mapStringToWideString(utils::Utf8Util::getBytesFromWideString(i->str())));
            }
        }
        catch (const std::exception &e)
        {
            return {};
        }
        return result;
    }

    std::map<pair, size_t> RegexSearch::search(std::basic_string<char> &where, std::u32string const &what, size_t startPos, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator, utils::datatypes::Uuid const &windowUUID)
    {
        std::map<pair, size_t> result;
        try
        {
            auto preprocessedString = preprocess(what);
            std::wstring searchString;
            std::transform(preprocessedString.begin(), preprocessedString.end(), std::back_inserter(searchString), [&](char32_t const &c)
                           { return utils::Utf8Util::getCodePoint(c, true); });
            auto preprocessedWhere = utils::helpers::Lambda::charToWchar(where);

            auto searchRegex = std::wregex(searchString);
            auto results_begin = std::wsregex_iterator(preprocessedWhere.first.begin(), preprocessedWhere.first.end(), searchRegex, std::regex_constants::match_not_eol | std::regex_constants::match_not_bol);
            auto results_end = std::wsregex_iterator();

            for (std::wsregex_iterator i = results_begin; i != results_end; ++i)
            {
                size_t offset = startPos + preprocessedWhere.second[i->position(0)];
                result.insert({{offset, printingOrchestrator->getNumberOfChangesBytesAtPos(offset, windowUUID)}, i->str().size()});
            }
        }
        catch (const std::exception &e)
        {
            return {};
        }
        return result;
    }

    std::map<pair, size_t> RegexSearch::search(std::basic_string<char> &where, std::u32string const &what, pair startPos, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator, std::shared_ptr<utils::storage::AbstractStorage> storage, std::basic_string<char> const &chunk, utils::datatypes::Uuid const &windowUUID)
    {
        std::map<pair, size_t> result;
        try
        {
            auto preprocessedString = preprocess(what);
            std::wstring searchString;
            std::transform(preprocessedString.begin(), preprocessedString.end(), std::back_inserter(searchString), [&](char32_t const &c)
                           { return utils::Utf8Util::getCodePoint(c, true); });
            auto preprocessedWhere = utils::helpers::Lambda::charToWchar(where);

            auto searchRegex = std::wregex(searchString);
            auto results_begin = std::wsregex_iterator(preprocessedWhere.first.begin(), preprocessedWhere.first.end(), searchRegex, std::regex_constants::match_not_eol | std::regex_constants::match_not_bol);
            auto results_end = std::wsregex_iterator();

            for (std::wsregex_iterator i = results_begin; i != results_end; ++i)
            {
                result.insert({utils::helpers::Lambda::convertBufferByteOffsetToPosition(startPos, preprocessedWhere.second[i->position(0)], printingOrchestrator, windowUUID, storage, chunk), i->str().size()});
            }
        }
        catch (const std::exception &e)
        {
            return {};
        }
        return result;
    }

    std::map<pair, size_t> RegexSearch::searchHex(std::basic_string<char> const &where, std::string const &what, size_t startPos, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator, utils::datatypes::Uuid const &windowUUID)
    {
        std::map<pair, size_t> result;
        try
        {
            auto searchRegex = std::regex(preprocess(what));
            auto results_begin = std::sregex_iterator(where.begin(), where.end(), searchRegex, std::regex_constants::match_not_eol | std::regex_constants::match_not_bol);
            auto results_end = std::sregex_iterator();

            for (std::regex_iterator i = results_begin; i != results_end; ++i)
            {
                size_t offset = startPos + i->position(0);
                result.insert({{offset, printingOrchestrator->getNumberOfChangesBytesAtPos(offset, windowUUID)}, i->str().size()});
            }
        }
        catch (const std::exception &e)
        {
            return {};
        }
        return result;
    }

    std::map<pair, size_t> RegexSearch::searchHex(std::basic_string<char> const &where, std::string const &what, pair startPos, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator, std::shared_ptr<utils::storage::AbstractStorage> storage, std::basic_string<char> const &chunk, utils::datatypes::Uuid const &windowUUID)
    {
        std::map<pair, size_t> result;
        try
        {
            auto searchRegex = std::regex(preprocess(what));
            auto results_begin = std::sregex_iterator(where.begin(), where.end(), searchRegex, std::regex_constants::match_not_eol | std::regex_constants::match_not_bol);
            auto results_end = std::sregex_iterator();

            for (std::regex_iterator i = results_begin; i != results_end; ++i)
            {
                result.insert({utils::helpers::Lambda::convertBufferByteOffsetToPosition(startPos, i->position(0), printingOrchestrator, windowUUID, storage, chunk), i->str().size()});
            }
        }
        catch (const std::exception &e)
        {
            return {};
        }
        return result;
    }

    bool RegexSearch::isValidRegex(std::u32string const &searchStringArg)
    {
        std::wstring searchString;
        std::transform(searchStringArg.begin(), searchStringArg.end(), std::back_inserter(searchString), [&](char32_t const &c)
                       { return utils::Utf8Util::getCodePoint(c, true); });

        try
        {
            auto r = std::wregex(searchString);
        }
        catch (const std::exception &e)
        {
            return false;
        }
        return true;
    }
}
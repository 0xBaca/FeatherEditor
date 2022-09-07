#pragma once

#include <printer/PrintingOrchestrator.hpp>
#include <utils/datatypes/Global.hpp>

#include <regex>
#include <set>

namespace feather::utils::algorithm
{
    class RegexSearch
    {
    private:
        std::wregex searchRegex;
        std::wstring searchRegexBuffer;
        std::set<std::u32string> searchResults;
        static std::u32string preprocess(std::u32string const &);
        static std::string preprocess(std::string const &);

    public:
        RegexSearch(std::u32string const &);
        std::set<std::u32string> search(std::u32string const &);
        static std::map<pair, size_t> search(std::basic_string<char> &, std::u32string const &, size_t, std::shared_ptr<printer::PrintingOrchestratorInterface>, utils::datatypes::Uuid const &);
        static std::map<pair, size_t> search(std::basic_string<char> &, std::u32string const &, pair, std::shared_ptr<printer::PrintingOrchestratorInterface>, std::shared_ptr<utils::storage::AbstractStorage>, std::basic_string<char> const &, utils::datatypes::Uuid const &);
        static std::map<pair, size_t> searchHex(std::basic_string<char> const &, std::string const &, size_t, std::shared_ptr<printer::PrintingOrchestratorInterface>, utils::datatypes::Uuid const &);
        static std::map<pair, size_t> searchHex(std::basic_string<char> const &, std::string const &, pair, std::shared_ptr<printer::PrintingOrchestratorInterface>, std::shared_ptr<utils::storage::AbstractStorage>, std::basic_string<char> const &, utils::datatypes::Uuid const &);
        static bool isValidRegex(std::u32string const &);
    };
}
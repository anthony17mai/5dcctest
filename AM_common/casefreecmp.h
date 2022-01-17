#pragma once

#include <string>

namespace AM_common
{
    //casefree less
    struct casefreeless {
        typedef std::string first_argument_type;
        typedef std::string second_argument_type;
        typedef bool result_type;

        bool operator()(const first_argument_type& _Left, const second_argument_type& _Right) const;
    };

    //casefree equal
    struct casefreeeqq
    {
        typedef std::string first_argument_type;
        typedef std::string second_argument_type;
        typedef bool result_type;

        bool operator()(const first_argument_type& _Left, const second_argument_type& _Right) const;
    };

    //casefree hash
    template<typename hasher = std::hash<std::string>>
    struct casefreehash
    {
        inline size_t operator()(const std::string& str)
        {
            std::string lower(str.size(), 0);
            for (size_t i = 0; i < str.size(); i++)
            {
                lower[i] = (char)tolower(str[i]);
            }
            return hasher()(lower);
        }
    };
}
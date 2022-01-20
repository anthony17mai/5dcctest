#include "casefreecmp.h"

namespace AM_common
{
    bool casefreeless::operator()(const first_argument_type& _Left, const second_argument_type& _Right) const {
        const char* l = _Left.c_str();
        const char* r = _Right.c_str();
        char lc = '\0';
        int i = 0;
        for (; (lc = l[i]) != '\0'; i++)
        {
            char rc = r[i];
            if (isupper(lc)) lc = (char)tolower(lc);
            if (isupper(rc)) rc = (char)tolower(rc);
            if (lc != rc)
            {
                if (lc < rc)
                    return true;
                else
                    return false;
            }
        }
        if ('\0' < r[i])
            return true;
        else
            return false;
    }

    bool casefreeeqq::operator()(const first_argument_type& _Left, const second_argument_type& _Right) const
    {
        const char* l = _Left.c_str();
        const char* r = _Right.c_str();

        if (_Left.size() != _Right.size()) return false;

        bool eq = true;
        for (size_t i = 0; i < _Left.size(); i++)
        {
            char lc = l[i];
            char rc = r[i];
            lc = (char)tolower(lc);
            rc = (char)tolower(rc);

            if (lc != rc)
            {
                eq = false;
                break;
            }
            else
            {
                //equals
                continue;
            }
        }
        return eq;
    }
}


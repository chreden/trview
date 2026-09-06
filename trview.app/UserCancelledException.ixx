export module trview.app:UserCancelledException;

import std;

namespace trview
{
    export struct UserCancelledException final : public std::exception
    {
    };
}

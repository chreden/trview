#pragma once

namespace trview
{
    struct IUpdateChecker
    {
        virtual ~IUpdateChecker() = 0;
        /// Check github for updates to trview.
        virtual void check_for_updates() = 0;
    };
}

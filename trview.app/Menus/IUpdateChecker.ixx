export module trview.app:IUpdateChecker;

namespace trview
{
    export struct IUpdateChecker
    {
        virtual ~IUpdateChecker() = 0;
        /// Check github for updates to trview.
        virtual void check_for_updates() = 0;
    };
}

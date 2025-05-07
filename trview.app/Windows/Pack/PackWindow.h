#pragma once

#include "IPackWindow.h"

namespace trview
{
    class PackWindow final : public IPackWindow
    {
    public:
        explicit PackWindow(const std::shared_ptr<IFiles>& files, const std::shared_ptr<IDialogs>& dialogs);
        virtual ~PackWindow() = default;
        void render() override;
        void set_number(int32_t number) override;
        void set_pack(const std::weak_ptr<trlevel::IPack>& pack) override;
    private:
        bool render_pack_window();

        std::weak_ptr<trlevel::IPack> _pack;
        std::string _id{ "Pack 0" };
        int32_t _index{ 0u };
        std::shared_ptr<IFiles> _files;
        std::shared_ptr<IDialogs> _dialogs;
    };
}

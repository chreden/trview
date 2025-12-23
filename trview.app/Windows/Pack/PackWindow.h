#pragma once

#include "IPackWindow.h"
#include <trview.common/Messages/IMessageSystem.h>

namespace trview
{
    class PackWindow final : public IPackWindow, public IRecipient, public std::enable_shared_from_this<IRecipient>
    {
    public:
        explicit PackWindow(const std::shared_ptr<IFiles>& files, const std::shared_ptr<IDialogs>& dialogs, const std::weak_ptr<IMessageSystem>& messaging);
        virtual ~PackWindow() = default;
        void render() override;
        void set_number(int32_t number) override;
        void set_pack(const std::weak_ptr<trlevel::IPack>& pack);
        void initialise();
        void receive_message(const Message& message) override;
    private:
        bool render_pack_window();

        std::weak_ptr<trlevel::IPack> _pack;
        std::string _id{ "Pack 0" };
        int32_t _index{ 0u };
        std::shared_ptr<IFiles> _files;
        std::shared_ptr<IDialogs> _dialogs;
        std::weak_ptr<IMessageSystem> _messaging;
    };
}

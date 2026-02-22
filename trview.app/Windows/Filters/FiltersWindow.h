#pragma once

#include <trview.common/Windows/IShell.h>
#include <trview.common/Windows/IDialogs.h>
#include <trview.common/Messages/IMessageSystem.h>

#include "../IWindow.h"
#include "../../Filters/Filters.h"
#include "../../Filters/IFilterStore.h"
#include "../../Settings/UserSettings.h"

namespace trview
{
    class FiltersWindow final : public IWindow, public std::enable_shared_from_this<IRecipient>
    {
    public:
        struct Names
        {
            static inline const std::string filters_list = "Filters";
        };

        explicit FiltersWindow(const std::weak_ptr<IFilterStore>& filter_store,
            const std::shared_ptr<IShell>& shell,
            const std::shared_ptr<IDialogs>& dialogs,
            const std::weak_ptr<IMessageSystem>& messaging);
        virtual ~FiltersWindow() = default;
        void initialise();
        void render() override;
        void set_number(int32_t number) override;
        void update(float dt) override;
        void receive_message(const Message& message) override;
        std::string type() const override;
        std::string title() const override;
    private:
        bool render_filters_window();

        std::string _id{ "Plugins 0" };
        std::weak_ptr<IFilterStore> _filter_store;
        std::shared_ptr<IShell> _shell;
        std::shared_ptr<IDialogs> _dialogs;
        std::optional<UserSettings> _settings;
        std::weak_ptr<IMessageSystem> _messaging;
        int32_t _selected_filter{ 0 };
        Filters _filters;
    };
}

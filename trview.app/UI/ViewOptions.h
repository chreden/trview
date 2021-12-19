#pragma once

#include <cstdint>
#include <trview.common/Event.h>
#include <set>

#include <trview.common/TokenStore.h>
#include <trview.ui/ILoader.h>
#include "IViewOptions.h"

namespace trview
{
    namespace ui
    {
        class Control;
        class Checkbox;
        class NumericUpDown;
        class Button;
        class Window;
    }

    class ViewOptions final : public IViewOptions
    {
    public:
        struct Names
        {
            static const std::string depth;
            static const std::string group;
            static const std::string tr_1_3_panel;
            static const std::string tr_4_5_panel;
            static const std::string alternate_groups;
        };

        struct Colours
        {
            static const Colour FlipOff;
            static const Colour FlipOn;
        };

        explicit ViewOptions(ui::Control& parent, const std::shared_ptr<ui::ILoader>& source);
        virtual ~ViewOptions() = default;
        virtual void set_alternate_group(uint32_t value, bool enabled) override;
        virtual void set_alternate_groups(const std::set<uint32_t>& groups) override;
        virtual void set_depth(int32_t value) override;
        virtual void set_flip_enabled(bool enabled) override;
        virtual void set_toggle(const std::string& name, bool value) override;
        virtual void set_use_alternate_groups(bool value) override;
        virtual bool toggle(const std::string& name) const override;
    private:
        void find_toggles(ui::Control& options);

        TokenStore _token_store;
        ui::NumericUpDown* _depth;
        ui::Window* _tr1_3_panel;
        ui::Window* _tr4_5_panel;
        ui::Window* _alternate_groups;
        std::unordered_map<uint32_t, bool> _alternate_group_values;
        std::unordered_map<uint32_t, ui::Button*> _alternate_group_buttons;
        std::unordered_map<std::string, ui::Checkbox*> _toggles;
    };
}

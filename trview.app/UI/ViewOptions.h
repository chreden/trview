#pragma once

#include <cstdint>
#include <trview.common/Event.h>
#include <set>

#include <trview.common/TokenStore.h>
#include <trview.app/Graphics/ITextureStorage.h>
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
            static const std::string depth_enabled;
            static const std::string flip;
            static const std::string hidden_geometry;
            static const std::string highlight;
            static const std::string triggers;
            static const std::string show_bounding_boxes;
            static const std::string water;
            static const std::string wireframe;
            static const std::string group;
        };

        struct Colours
        {
            static const Colour FlipOff;
            static const Colour FlipOn;
        };

        explicit ViewOptions(ui::Control& parent, const ITextureStorage& texture_storage);
        virtual ~ViewOptions() = default;
        virtual void set_alternate_group(uint32_t value, bool enabled) override;
        virtual void set_alternate_groups(const std::set<uint32_t>& groups) override;
        virtual void set_depth(int32_t value) override;
        virtual void set_depth_enabled(bool value) override;
        virtual void set_flip(bool flip) override;
        virtual void set_flip_enabled(bool enabled) override;
        virtual void set_highlight(bool highlight) override;
        virtual void set_show_hidden_geometry(bool show) override;
        virtual void set_show_triggers(bool show) override;
        virtual void set_show_water(bool show) override;
        virtual void set_show_wireframe(bool show) override;
        virtual void set_use_alternate_groups(bool value) override;
        virtual void set_show_bounding_boxes(bool value) override;
        virtual bool show_hidden_geometry() const override;
        virtual bool show_triggers() const override;
        virtual bool show_water() const override;
        virtual bool show_wireframe() const override;
        virtual bool show_bounding_boxes() const override;
    private:
        TokenStore _token_store;
        ui::Checkbox* _highlight;
        ui::Checkbox* _flip;
        ui::Checkbox* _triggers;
        ui::Checkbox* _hidden_geometry;
        ui::Checkbox* _water;
        ui::Checkbox* _depth_enabled;
        ui::Checkbox* _wireframe;
        ui::Checkbox* _bounding_boxes;
        ui::NumericUpDown* _depth;
        ui::Window* _tr1_3_panel;
        ui::Window* _tr4_5_panel;
        ui::Window* _alternate_groups;
        std::unordered_map<uint32_t, bool> _alternate_group_values;
        std::unordered_map<uint32_t, ui::Button*> _alternate_group_buttons;
    };
}

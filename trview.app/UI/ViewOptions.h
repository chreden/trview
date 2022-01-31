#pragma once

#include <cstdint>
#include "IViewOptions.h"

namespace trview
{
    class ViewOptions final : public IViewOptions
    {
    public:
        ViewOptions();
        virtual void render() override;
        virtual void set_alternate_group(uint32_t value, bool enabled) override;
        virtual void set_alternate_groups(const std::set<uint32_t>& groups) override;
        virtual void set_flip_enabled(bool enabled) override;
        virtual void set_scalar(const std::string& name, int32_t value) override;
        virtual void set_toggle(const std::string& name, bool value) override;
        virtual void set_use_alternate_groups(bool value) override;
        virtual bool toggle(const std::string& name) const override;
    private:
        std::unordered_map<uint32_t, bool> _alternate_group_values;
        std::unordered_map<std::string, bool> _toggles;
        std::unordered_map<std::string, int32_t> _scalars;
        bool _use_alternate_groups{ false };
        bool _flip_enabled{ false };
    };
}

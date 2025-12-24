#pragma once

#include <cstdint>
#include "IViewOptions.h"

namespace trview
{
    struct IWindows;

    class ViewOptions final : public IViewOptions
    {
    public:
        struct Names
        {
            static const inline std::string flags = "flags";
        };

        explicit ViewOptions(const std::weak_ptr<IWindows>& windows);
        void render() override;
        void set_alternate_group(uint32_t value, bool enabled) override;
        void set_alternate_groups(const std::set<uint32_t>& groups) override;
        void set_flip_enabled(bool enabled) override;
        void set_scalar(const std::string& name, int32_t value) override;
        void set_toggle(const std::string& name, bool value) override;
        void set_use_alternate_groups(bool value) override;
        bool toggle(const std::string& name) const override;
        void set_ng_plus_enabled(bool enabled) override;
    private:
        std::unordered_map<uint32_t, bool> _alternate_group_values;
        std::unordered_map<std::string, bool> _toggles;
        std::unordered_map<std::string, int32_t> _scalars;
        bool _use_alternate_groups{ false };
        bool _flip_enabled{ false };
        bool _ng_plus_enabled{ false };
        std::weak_ptr<IWindows> _windows;
    };
}

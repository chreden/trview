#pragma once

#include <trview.common/Windows/IShell.h>
#include "ILoader.h"

namespace trview
{
    namespace ui
    {
        class JsonLoader final : public ILoader
        {
        public:
            explicit JsonLoader(const std::shared_ptr<IShell>& shell);
            virtual ~JsonLoader() = default;
            virtual std::unique_ptr<Control> load_from_resource(uint32_t resource_id) const;
            std::unique_ptr<Control> load_from_json(const std::string& json) const;
        private:
            std::shared_ptr<IShell> _shell;
        };
    }
}
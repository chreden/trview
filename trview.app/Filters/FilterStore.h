#pragma once

#include <trview.common/IFiles.h>
#include <trview.common/Messages/IRecipient.h>

#include "IFilterStore.h"
#include "Filters.h"
#include "../Settings/UserSettings.h"

namespace trview
{
    class FilterStore final : public IFilterStore, public IRecipient
    {
    public:
        explicit FilterStore(const std::shared_ptr<IFiles>& files, const UserSettings& settings);
        virtual ~FilterStore() = default;
        void add(const std::string& key, const Filters::Filter& filter) override;
        void load() override;
        std::map<std::string, Filters::Filter> filters_for_key(const std::string& key) const override;
        void receive_message(const Message& message) override;
        void remove(const std::string& type_key, const std::string& name) override;
        void save() override;
    private:
        struct StoredFilter
        {
            std::string     name;
            std::string     filename;
            Filters::Filter filter;
        };

        std::shared_ptr<IFiles>     _files;
        std::map<std::string, std::vector<StoredFilter>> _filters;
        UserSettings                _settings;
    };
}

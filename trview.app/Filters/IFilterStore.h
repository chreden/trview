#pragma once

#include <map>
#include <string>

#include "Filters.h"

namespace trview
{
    struct IFilterStore
    {
        virtual ~IFilterStore() = 0;
        virtual void add(const std::string& key, const Filters::Filter& filter) = 0;
        virtual void load() = 0;
        virtual std::map<std::string, Filters::Filter> filters() const = 0;
        virtual std::map<std::string, Filters::Filter> filters_for_key(const std::string& key) const = 0;
        virtual void remove(const std::string& name) = 0;
        virtual void save() = 0;
    };
}

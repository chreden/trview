#pragma once

#include <external/nlohmann/json.hpp>
#include "SizeMode.h"
#include "Layouts/StackLayout.h"
#include "Listbox.h"
#include "TextArea.h"

namespace trview
{
    namespace ui
    {
        void from_json(const nlohmann::json& json, SizeMode& mode);
        void from_json(const nlohmann::json& json, StackLayout::Direction& direction);
        void from_json(const nlohmann::json& json, Listbox::Column::IdentityMode& identity_mode);
        void from_json(const nlohmann::json& json, Listbox::Column::Type& type);
        void from_json(const nlohmann::json& json, TextArea::Mode& mode);
        void from_json(const nlohmann::json& json, Align& align);
        void from_json(const nlohmann::json& json, SizeDimension& dimension);
    }
}

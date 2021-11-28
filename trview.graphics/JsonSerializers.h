#pragma once

#include <external/nlohmann/json.hpp>
#include "TextAlignment.h"
#include "ParagraphAlignment.h"

namespace trview
{
    namespace graphics
    {
        void from_json(const nlohmann::json& json, TextAlignment& text_alignment);
        void from_json(const nlohmann::json& json, ParagraphAlignment& paragraph_alignment);
    }
}

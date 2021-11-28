#include "JsonSerializers.h"

namespace trview
{
    namespace graphics
    {
        void from_json(const nlohmann::json& json, TextAlignment& text_alignment)
        {
            const auto text = json.get<std::string>();
            if (text == "justified")
            {
                text_alignment = TextAlignment::Justified;
            }
            else if (text == "centre")
            {
                text_alignment = TextAlignment::Centre;
            }
            else if (text == "right")
            {
                text_alignment = TextAlignment::Right;
            }
            else
            {
                text_alignment = TextAlignment::Left;
            }
        }

        void from_json(const nlohmann::json& json, ParagraphAlignment& paragraph_alignment)
        {
            const auto text = json.get<std::string>();
            if (text == "near")
            {
                paragraph_alignment = ParagraphAlignment::Near;
            }
            else if (text == "far")
            {
                paragraph_alignment = ParagraphAlignment::Far;
            }
            else
            {
                paragraph_alignment = ParagraphAlignment::Centre;
            }
        }
    }
}

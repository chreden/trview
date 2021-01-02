#include "Serialisation.h"
#include <trview.common/Strings.h>

namespace trview
{
    namespace
    {
        std::string to_base64(const std::vector<uint8_t>& bytes)
        {
            if (bytes.empty())
            {
                return std::string();
            }

            DWORD required_length = 0;
            CryptBinaryToString(&bytes[0], static_cast<DWORD>(bytes.size()), CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, nullptr, &required_length);

            std::vector<wchar_t> output_string(required_length);
            CryptBinaryToString(&bytes[0], static_cast<DWORD>(bytes.size()), CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, &output_string[0], &required_length);

            return to_utf8(std::wstring(&output_string[0]));
        }
    }

    void to_json(nlohmann::json& json, const Waypoint& waypoint)
    {
        std::stringstream pos_string;
        auto pos = waypoint.position();
        pos_string << pos.x << "," << pos.y << "," << pos.z;

        json = nlohmann::json
        {
            { "type", to_utf8(waypoint_type_to_string(waypoint.type())) },
            { "position", pos_string.str() },
            { "room", waypoint.room() },
            { "index", waypoint.index() },
            { "notes", to_utf8(waypoint.notes()) }
        };
        if (waypoint.has_save())
        {
            json["save"] = to_base64(waypoint.save_file());
        }
    }
}

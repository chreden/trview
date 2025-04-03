#include "Level.h"
#include "Level_common.h"
#include "Level_psx.h"

#include <ranges>

namespace trlevel
{
    namespace
    {
        struct tr4_psx_level_info
        {
            int32_t  version;
            uint32_t sound_offsets;
            uint32_t sound_data_offset;
            uint32_t textiles_offset;
            uint32_t frames_offset;
            uint32_t room_data_offset;
            uint32_t models_offset;
            uint32_t unknown[2];
            uint32_t num_sounds;
            uint32_t sound_data_length;
            uint16_t clut_start;
            uint16_t num_rooms;
            char     unknown_2[2];
            uint16_t num_items;
            char     unknown_3[4];
            uint32_t room_data_size;
            uint32_t floor_data_size;
            uint32_t outside_room_size;
            uint32_t bounding_boxes_size;
            char     unknown_4[0x4];
            uint32_t mesh_data_size;
            uint32_t mesh_pointer_size;
            uint32_t animations_size;
            uint32_t state_changes_size;
            uint32_t dispatches_size;
            uint32_t commands_size;
            uint32_t meshtree_size;
            uint32_t frames_size;
            uint32_t texture_info_length;
            uint32_t sprite_info_length;
            uint32_t texture_info_length2;
            uint32_t animated_texture_length;
            uint32_t sfx_info_length;
            uint32_t sample_info_length;
            char     unknown_5[12];
            uint32_t unknown_offsets[7];
            uint32_t num_cameras;
            char     unknown_5a[4];
            int      camera_length;
            char     unknown_6[4];
            uint16_t num_ai_objects;
            char     unknown_7[38];
        };
        static_assert(sizeof(tr4_psx_level_info) == 228);
    }

    void Level::load_tr5_psx(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        file.seekg(313344);
        tr4_psx_level_info convert_room_info = read<tr4_psx_level_info>(file);

        activity;
        callbacks;
    }
}
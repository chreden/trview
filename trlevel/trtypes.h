#pragma once

#include <cstdint>
#include <vector>
#include <SimpleMath.h>

namespace trlevel
{
#pragma pack(push, 1)

    constexpr float Scale_X { 1024.0f };
    constexpr float Scale_Y { 1024.0f };
    constexpr float Scale_Z { 1024.0f };

    struct tr_colour
    {
        uint8_t Red;
        uint8_t Green;
        uint8_t Blue;
    };

    struct tr_colour4
    {
        uint8_t Red;
        uint8_t Green;
        uint8_t Blue;
        uint8_t Unused;
    };

    struct tr_textile8
    {
        uint8_t Tile[256 * 256];
    };

    struct tr_textile16
    {
        uint16_t Tile[256 * 256];
    };

    struct tr_textile32
    {
        uint32_t Tile[256 * 256];
    };

    struct tr_room_info
    {
        int32_t x;
        int32_t y;
        int32_t z;
        int32_t yBottom;
        int32_t yTop;
    };

    struct tr1_4_room_info
    {
        int32_t x;
        int32_t z;
        int32_t yBottom;
        int32_t yTop;
    };

    struct tr_vertex
    {
        int16_t x;
        int16_t y;
        int16_t z;
    };

    struct tr5_vertex
    {
        float x;
        float y;
        float z;
    };

    // Four vertices(the values are indices into the appropriate vertex list) and a texture(an index into 
    // the object - texture list) or colour(index into 8 - bit palette or 16 - bit palette).If the rectangle 
    // is a coloured polygon(not textured), the.Texture element contains two indices : the low byte(Texture & 0xFF)
    // is an index into the 256 - colour palette, while the high byte(Texture >> 8) is in index into the 16 - bit palette, 
    // when present. A textured rectangle will have its vertices mapped onto all 4 vertices of an object texture, 
    // in appropriate correspondence.
    // Texture field can have the bit 15 set: when it is, the face is double - sided(i.e.visible from both sides).
    // 1, 2, 3: If the rectangle is a coloured polygon(not textured), the.Texture element contains two indices : 
    // the low byte(Texture & 0xFF) is an index into the 256 - colour palette, while the high byte(Texture >> 8) is 
    // in index into the 16 - bit palette, when present.
    struct tr_face4
    {
        uint16_t vertices[4];
        uint16_t texture;
    };

    struct tr_face3
    {
        uint16_t vertices[3];
        uint16_t texture;
    };

    struct tr4_mesh_face3
    {
        uint16_t vertices[3];
        uint16_t texture;
        uint16_t effects;
    };

    struct tr4_mesh_face4
    {
        uint16_t vertices[4];
        uint16_t texture;
        uint16_t effects;
    };

    struct tr_room_sprite
    {
        int16_t vertex;
        int16_t texture;
    };

    struct tr_animation // 32 bytes
    {
        uint32_t FrameOffset; // Byte offset into Frames[] (divide by 2 for Frames[i])
        uint8_t FrameRate;   // Engine ticks per frame
        uint8_t FrameSize;   // Number of int16_t's in Frames[] used by this animation

        uint16_t State_ID;

        uint32_t Speed;     // fixed
        uint32_t Accel;     // fixed

        uint16_t FrameStart;  // First frame in this animation
        uint16_t FrameEnd;    // Last frame in this animation
        uint16_t NextAnimation;
        uint16_t NextFrame;

        uint16_t NumStateChanges;
        uint16_t StateChangeOffset; // Offset into StateChanges[]

        uint16_t NumAnimCommands;   // How many of them to use.
        uint16_t AnimCommand;       // Offset into AnimCommand[]
    };

    struct tr4_animation
    {
        uint32_t FrameOffset; // Byte offset into Frames[] (divide by 2 for Frames[i])
        uint8_t FrameRate;   // Engine ticks per frame
        uint8_t FrameSize;   // Number of int16_t's in Frames[] used by this animation

        uint16_t State_ID;

        uint32_t Speed;     // fixed
        uint32_t Accel;     // fixed
        uint32_t SpeedLateral; // fixed
        uint32_t AccelLateral; // fixed

        uint16_t FrameStart;  // First frame in this animation
        uint16_t FrameEnd;    // Last frame in this animation
        uint16_t NextAnimation;
        uint16_t NextFrame;

        uint16_t NumStateChanges;
        uint16_t StateChangeOffset; // Offset into StateChanges[]

        uint16_t NumAnimCommands;   // How many of them to use.
        uint16_t AnimCommand;       // Offset into AnimCommand[]
    };

    struct tr_anim_command // 2 bytes
    {
        int16_t Value;
    };

    struct tr_state_change // 6 bytes
    {
        uint16_t StateID;
        uint16_t NumAnimDispatches; // number of ranges (seems to always be 1..5)
        uint16_t AnimDispatch;      // Offset into AnimDispatches[]
    };

    struct tr_anim_dispatch    // 8 bytes
    {
        int16_t Low;           // Lowest frame that uses this range
        int16_t High;          // Highest frame that uses this range
        int16_t NextAnimation; // Animation to dispatch to
        int16_t NextFrame;     // Frame offset to dispatch to
    };

    struct tr2_meshtree
    {
        int32_t Coord;
    };

    struct tr_meshtree_node // 4 bytes
    {
        uint32_t Flags;
        int32_t Offset_X;
        int32_t Offset_Y;
        int32_t Offset_Z;

        DirectX::SimpleMath::Vector3 position() const
        {
            return DirectX::SimpleMath::Vector3(Offset_X / Scale_X, Offset_Y / Scale_Y, Offset_Z / Scale_Z);
        }
    };

    struct tr_model  // 18 bytes
    {
        uint32_t ID;           // Type Identifier (matched in Entities[])
        uint16_t NumMeshes;    // Number of meshes in this object
        uint16_t StartingMesh; // Starting mesh (offset into MeshPointers[])
        uint32_t MeshTree;     // Offset into MeshTree[]
        uint32_t FrameOffset;  // Byte offset into Frames[] (divide by 2 for Frames[i])
        uint16_t Animation;    // Offset into Animations[]
    };

    struct tr5_model
    {
        tr_model model;
        uint16_t filler;
    };


    struct tr_bounding_box // 12 bytes
    {
        int16_t MinX, MaxX, MinY, MaxY, MinZ, MaxZ;
    };

    struct tr_staticmesh   // 32 bytes
    {
        uint32_t ID;   // Static Mesh Identifier
        uint16_t Mesh; // Mesh (offset into MeshPointers[])
        tr_bounding_box VisibilityBox;
        tr_bounding_box CollisionBox;
        uint16_t Flags;
    };
    
    struct tr_sprite_texture   // 16 bytes
    {
        uint16_t Tile;
        uint8_t x;
        uint8_t y;
        uint16_t Width;        // (ActualWidth  * 256) + 255
        uint16_t Height;       // (ActualHeight * 256) + 255
        int16_t LeftSide;
        int16_t TopSide;
        int16_t RightSide;
        int16_t BottomSide;
    };

    struct tr_sprite_sequence  // 8 bytes
    {
        int32_t SpriteID;       // Sprite identifier
        int16_t NegativeLength; // Negative of ``how many sprites are in this sequence''
        int16_t Offset;         // Where (in sprite texture list) this sequence starts
    };

    struct tr_camera // 16 bytes
    {
        int32_t  x;
        int32_t  y;
        int32_t  z;
        int16_t  Room;
        uint16_t Flag;
    };

    struct tr4_flyby_camera
    {
        int32_t x;
        int32_t y;
        int32_t z;
        int32_t dx;
        int32_t dy;
        int32_t dz;
        uint8_t sequence;
        uint8_t index;
        uint16_t fov;
        int16_t  roll;
        uint16_t timer;
        uint16_t speed;
        uint16_t flags;
        uint32_t room_id;
    };

    struct tr_sound_source // 16 bytes
    {
        int32_t x;         // absolute X position of sound source (world coordinates)
        int32_t y;         // absolute Y position of sound source (world coordinates)
        int32_t z;         // absolute Z position of sound source (world coordinates)
        uint16_t SoundID;   // internal sound index
        uint16_t Flags;     // 0x40, 0x80, or 0xC0
    };

    struct tr_object_texture_vert // 4 bytes
    {
        int8_t Xcoordinate; // 1 if Xpixel is the low value, 255 if Xpixel is the high value in the object texture
        uint8_t Xpixel;
        int8_t Ycoordinate; // 1 if Ypixel is the low value, 255 if Ypixel is the high value in the object texture
        uint8_t Ypixel;
    };

    // Version of box used in TR1/UB.
    struct tr_box
    {
        uint32_t Zmin;
        uint32_t Zmax;
        uint32_t Xmin;
        uint32_t Xmax;
        int16_t TrueFloor;
        uint16_t OverlapIndex;
    };

    struct tr2_box   // 8 bytes
    {
        uint8_t Zmin;          // Horizontal dimensions in sectors
        uint8_t Zmax;
        uint8_t Xmin;
        uint8_t Xmax;
        int16_t TrueFloor;     // Height value in global units
        int16_t OverlapIndex;  // Bits 0-13 is the index into Overlaps[]
    };

    struct tr_entity
    {
        int16_t TypeID;
        int16_t Room;
        int32_t x;
        int32_t y;
        int32_t z;
        int16_t Angle;
        int16_t Intensity1;
        uint16_t Flags;
    };

    struct tr2_entity // 24 bytes
    {
        int16_t TypeID;
        int16_t Room;
        int32_t x;
        int32_t y;
        int32_t z;
        int16_t Angle;
        int16_t Intensity1;
        int16_t Intensity2; // Like Intensity1, and almost always with the same value.
        uint16_t Flags;

        DirectX::SimpleMath::Vector3 position() const
        {
            return DirectX::SimpleMath::Vector3(x / Scale_X, y / Scale_Y, z / Scale_Z);
        }
    };

    struct tr_sound_details // 8 bytes
    {
        uint16_t Sample; // (index into SampleIndices)
        uint16_t Volume;
        uint16_t Chance; // If !=0 and ((rand()&0x7fff) > Chance), this sound is not played
        uint16_t Characteristics;
    };

    struct tr3_sound_details  // 8 bytes
    {
        uint16_t Sample; // (index into SampleIndices)
        uint8_t Volume;
        uint8_t Range;
        uint8_t Chance;
        uint8_t Pitch;
        int16_t Characteristics;
    };

    struct tr_cinematic_frame // 16 bytes
    {
        int16_t targetX; // Camera look at position about X axis, 
        int16_t targetY; // Camera look at position about Y axis
        int16_t target2; // Camera look at position about Z axis
        int16_t posZ;    // Camera position about Z axis
        int16_t posY;    // Camera position relative to something (see posZ)
        int16_t posX;    // Camera position relative to something (see posZ)
        int16_t fov;
        int16_t roll;    // Rotation about X axis
    };

    struct tr_object_texture  // 20 bytes
    {
        uint16_t               Attribute;
        uint16_t               TileAndFlag;
        tr_object_texture_vert Vertices[4]; // The four corners of the texture
    };

    struct tr4_object_texture // 38 bytes
    {
        uint16_t               Attribute;
        uint16_t               TileAndFlag;
        uint16_t               NewFlags;
        tr_object_texture_vert Vertices[4]; // The four corners of the texture
        uint32_t               OriginalU;
        uint32_t               OriginalV;
        uint32_t               Width;     // Actually width-1
        uint32_t               Height;    // Actually height-1
    };

    struct tr5_object_texture
    {
        tr4_object_texture tr4_texture;
        uint16_t           filler;
    };

    // Room vertex for Tomb Raider 1/Unfinished Business.
    struct tr_room_vertex
    {
        tr_vertex vertex;
        int16_t   lighting;
    };

    struct tr3_room_vertex
    {
        tr_vertex   vertex;
        int16_t     lighting;
        uint16_t    attributes;
        uint16_t    colour;
    };

    struct tr5_room_vertex
    {
        tr5_vertex vertex;
        tr5_vertex normal;
        uint32_t   colour;
    };

    struct tr5_room_layer
    {
        uint16_t num_vertices;
        uint16_t _1[2];
        uint16_t num_rectangles;
        uint16_t num_triangles;
        uint16_t _2[3];
        tr5_vertex bounding_box_min;
        tr5_vertex bounding_box_max;
        uint32_t _3[4];
    };

    struct tr_room_portal
    {
        uint16_t  adjoining_room;
        tr_vertex normal;
        tr_vertex vertices[4];
    };

    struct tr_room_sector
    {
        uint16_t floordata_index;    // Index into FloorData[]
        uint16_t box_index;   // Index into Boxes[] (-1 if none)
        uint8_t  room_below;  // 255 is none
        int8_t   floor;      // Absolute height of floor
        uint8_t  room_above;  // 255 if none
        int8_t   ceiling;    // Absolute height of ceiling
    };

    // Version of the room_light structure used in Tomb Raider I/UB.
    struct tr_room_light
    {
        int32_t  x;
        int32_t  y;
        int32_t  z;
        uint16_t intensity;
        uint32_t fade;
    };

    struct tr3_room_light   // 24 bytes
    {
        int32_t x;
        int32_t y;
        int32_t z;
        tr_colour4 colour;        // Colour of the light
        uint32_t   intensity;
        uint32_t   fade;          // Falloff value
    };

    struct tr4_room_light
    {
        int32_t x;
        int32_t y;
        int32_t z;
        tr_colour colour;
        uint8_t light_type;
        uint8_t unknown;
        uint8_t intensity;
        float in;
        float out;
        float length;
        float cutoff;
        float dx;
        float dy;
        float dz;
    };

    struct tr5_room_light
    {
        tr5_vertex position;
        float r, g, b;
        uint32_t separator;
        float in;
        float out;
        float rad_in;
        float rad_out;
        float range;
        tr5_vertex direction;
        int32_t x2, y2, z2;
        int32_t dx2, dy2, dz2;
        uint8_t light_type;
        uint8_t filler[3];
    };

    // Version of tr_room_staticmesh used in TR1/UB.
    struct tr_room_staticmesh
    {
        int32_t x;
        int32_t y;
        int32_t z;
        uint16_t rotation;
        uint16_t intensity;
        uint16_t mesh_id;
    };

    struct tr3_room_staticmesh 
    {
        int32_t x;
        int32_t y;
        int32_t z;
        uint16_t rotation;
        uint16_t colour;     // 15-bit colour
        uint16_t unused;     // Not used!
        uint16_t mesh_id;     // Which StaticMesh item to draw

        DirectX::SimpleMath::Vector3 position() const
        {
            return DirectX::SimpleMath::Vector3(x / Scale_X, y / Scale_Y, z / Scale_Z);
        }
    };

    struct tr2_frame_rotation
    {
        float x{ 0.0f }, y{ 0.0f }, z{ 0.0f };
    };

    struct tr4_ai_object
    {
        uint16_t type_id;
        uint16_t room;
        int32_t x;
        int32_t y;
        int32_t z;
        int16_t ocb;
        uint16_t flags;     // Activation mask, bitwise-shifted left by 1
        int32_t angle;
    };

#pragma pack(pop)

    struct tr3_room_data
    {
        std::vector<tr3_room_vertex> vertices;
        std::vector<tr4_mesh_face4> rectangles;
        std::vector<tr4_mesh_face3> triangles;
        std::vector<tr_room_sprite> sprites;
    };

    struct tr3_room
    {
        tr_room_info info;
        tr3_room_data data;

        std::vector<tr_room_portal> portals;

        uint16_t num_z_sectors;
        uint16_t num_x_sectors;
        std::vector<tr_room_sector> sector_list;

        uint32_t colour{ 0xffffffff };
        int16_t ambient_intensity_1;
        int16_t ambient_intensity_2;
        int16_t light_mode;

        std::vector<tr3_room_light> lights;

        std::vector<tr3_room_staticmesh> static_meshes;

        int16_t alternate_room;
        int16_t flags;

        uint16_t water_scheme;
        uint8_t reverb_info;
        uint8_t alternate_group;

        uint32_t room_colour;
    };

    struct tr2_frame
    {
        int16_t bb1x, bb1y, bb1z;
        int16_t bb2x, bb2y, bb2z;
        int16_t offsetx, offsety, offsetz;
        std::vector<tr2_frame_rotation> values;

        DirectX::SimpleMath::Vector3 position() const
        {
            return DirectX::SimpleMath::Vector3(offsetx / Scale_X, offsety / Scale_Y, offsetz / Scale_Z);
        }
    };

    struct tr_mesh
    {
        tr_vertex              centre;
        int32_t                coll_radius;
        std::vector<tr_vertex> vertices;
        std::vector<tr_vertex> normals;
        std::vector<int16_t>   lights;
        std::vector<tr4_mesh_face4>  textured_rectangles;
        std::vector<tr4_mesh_face3>  textured_triangles;
        std::vector<tr_face4>  coloured_rectangles;
        std::vector<tr_face3>  coloured_triangles;
    };

    struct tr4_sample
    {
        std::vector<uint8_t> sound_data;
    };

    struct tr5_room_header
    {
        uint8_t _1[4];
        uint32_t end_sd_offset;
        uint32_t start_sd_offset;
        uint8_t _2[4];
        uint32_t end_portal_offset;
        tr_room_info info;
        uint16_t num_z_sectors;
        uint16_t num_x_sectors;
        uint32_t colour;
        uint16_t num_lights;
        uint16_t num_static_meshes;
        uint8_t reverb_info;
        uint8_t alternate_group;
        uint16_t water_scheme;
        uint8_t _3[20];
        uint16_t alternate_room;
        uint16_t flags;
        uint8_t _4[20];
        float room_x;
        float room_y;
        float room_z;
        uint8_t _5[24];
        uint32_t num_room_triangles;
        uint32_t num_room_rectangles;
        uint8_t _6[4];
        uint32_t light_data_size;
        uint32_t num_lights2;
        uint8_t _7[4];
        float room_y_top;
        float room_y_bottom;
        uint32_t num_layers;
        uint32_t layer_offset;
        uint32_t vertices_offset;
        uint32_t poly_offset;
        uint32_t poly_offset2;
        uint32_t vertices_size;
        uint8_t _8[16];
    };

    // Convert a 32 bit textile into a 32 bit argb value.
    uint32_t convert_textile32(uint32_t t);

    // Convert a 16 bit textile into a 32 bit argb value.
    uint32_t convert_textile16(uint16_t t);

    // Convert a set of Tomb Raider I vertices into a vertex format compatible
    // with Tomb Raider III (what the viewer is currently using).
    std::vector<tr3_room_vertex> convert_vertices(std::vector<tr_room_vertex> vertices);

    /// Convert a set of Tomb Raider 5 vertices into a vertex format compatible
    /// with Tomb Raider III (what the viewer is currently using).
    /// @param vertices The vertices to convert.
    /// @return The converted vertices.
    std::vector<tr3_room_vertex> convert_vertices(std::vector<tr5_room_vertex> vertices);

    // Convert a set of Tomb Raider I lights into a light format compatible
    // with Tomb Raider III (what the viewer is currently using).
    std::vector<tr3_room_light> convert_lights(std::vector<tr_room_light> lights);

    // Convert a set of Tomb Raider I static meshes into a format compatible
    // with Tomb Raider III (what the viewer is currently using).
    std::vector<tr3_room_staticmesh> convert_room_static_meshes(std::vector<tr_room_staticmesh> meshes);

    // Convert a set of Tomb Raider I entities into a format compatible
    // with Tomb Raider III (what the viewer is currently using).
    std::vector<tr2_entity> convert_entities(std::vector<tr_entity> entities);

    // Convert the tr_colour to a tr_colour4 value.
    tr_colour4 convert_to_colour4(const tr_colour& colour);

    // Convert a set of Tomb Raider IV object textures into a format compatible
    // with Tomb Raider III (what the viewer is currently using).
    std::vector<tr_object_texture> convert_object_textures(std::vector<tr4_object_texture> object_textures);

    /// Convert a set of TR5 object textures into a format compatible with TR3 (what the viewer is currently using).
    /// @param object_textures The textures to convert.
    /// @returns The converted texture.
    std::vector<tr_object_texture> convert_object_textures(std::vector<tr5_object_texture> object_textures);

    // Convert a set of Tomb Raider I-III triangles to TRIV triangles.
    std::vector<tr4_mesh_face3> convert_triangles(std::vector<tr_face3> triangles);

    // Convert a set of Tomb Raider I-III rectangles to TRIV rectangles.
    std::vector<tr4_mesh_face4> convert_rectangles(std::vector<tr_face4> rectangles);

    /// Convert a pre TR5 room info into a TR5 room info.
    /// @param room_info The room info to convert.
    /// @returns The converted room info.
    tr_room_info convert_room_info(const tr1_4_room_info& room_info);

    std::vector<tr_model> convert_models(std::vector<tr5_model> models);
}
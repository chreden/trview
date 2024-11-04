#pragma once

#include <memory>
#include <wrl/client.h>
#include <d3d11.h>
#include <vector>
#include <set>

#include <trlevel/ILevel.h>
#include <trview.app/Elements/ILevel.h>
#include <trview.app/Geometry/ITransparencyBuffer.h>
#include <trview.app/Graphics/ISelectionRenderer.h>
#include <trview.app/Graphics/IMeshStorage.h>
#include <trview.graphics/IBuffer.h>
#include <trview.common/TokenStore.h>

#include "SoundSource/ISoundSource.h"

namespace trview
{
    struct ILevelTextureStorage;
    struct ICamera;
    struct ISoundStorage;

    namespace graphics
    {
        struct IShaderStorage;
        struct IShader;
    }

    class Level final : public ILevel, public std::enable_shared_from_this<ILevel>
    {
    public:
        Level(const std::shared_ptr<graphics::IDevice>& device,
            const std::shared_ptr<graphics::IShaderStorage>& shader_storage,
            std::shared_ptr<ILevelTextureStorage> level_texture_storage,
            std::unique_ptr<ITransparencyBuffer> transparency_buffer,
            std::unique_ptr<ISelectionRenderer> selection_renderer,
            const std::shared_ptr<ILog>& log,
            const graphics::IBuffer::ConstantSource& buffer_source,
            std::shared_ptr<ISoundStorage> sound_storage);
        virtual ~Level() = default;
        virtual std::vector<graphics::Texture> level_textures() const override;
        std::weak_ptr<IItem> selected_item() const override;
        std::weak_ptr<IRoom> selected_room() const override;
        virtual std::weak_ptr<IItem> item(uint32_t index) const override;
        virtual std::vector<std::weak_ptr<IItem>> items() const override;
        std::string name() const override;
        virtual uint32_t neighbour_depth() const override;
        virtual uint32_t number_of_rooms() const override;
        virtual std::vector<std::weak_ptr<IRoom>> rooms() const override;
        virtual std::weak_ptr<ITrigger> trigger(uint32_t index) const override;
        virtual std::vector<std::weak_ptr<ITrigger>> triggers() const override;
        virtual PickResult pick(const ICamera& camera, const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const override;
        virtual trlevel::Platform platform() const override;
        virtual void render(const ICamera& camera, bool render_selection) override;
        virtual void render_transparency(const ICamera& camera) override;
        virtual void set_highlight_mode(RoomHighlightMode mode, bool enabled) override;
        virtual bool highlight_mode_enabled(RoomHighlightMode mode) const override;
        std::vector<std::weak_ptr<IScriptable>> scriptables() const override;
        void set_selected_room(const std::weak_ptr<IRoom>& room) override;
        void set_selected_item(const std::weak_ptr<IItem>& item) override;
        virtual void set_neighbour_depth(uint32_t depth) override;
        virtual void on_camera_moved() override;
        virtual void set_alternate_mode(bool enabled) override;
        virtual void set_alternate_group(uint32_t group, bool enabled) override;
        virtual bool alternate_group(uint32_t group) const override;
        virtual std::weak_ptr<IRoom> room(uint32_t id) const override;
        virtual bool alternate_mode() const override;
        virtual bool any_alternates() const override;
        virtual void set_show_triggers(bool show) override;
        virtual void set_show_geometry(bool show) override;
        virtual bool show_geometry() const override;
        virtual void set_show_water(bool show) override;
        virtual void set_show_wireframe(bool show) override;
        virtual void set_show_bounding_boxes(bool show) override;
        void set_show_lighting(bool show) override;
        virtual void set_show_lights(bool show) override;
        virtual void set_show_items(bool show) override;
        virtual void set_show_rooms(bool show) override;
        bool show_lighting() const override;
        virtual bool show_lights() const override;
        virtual bool show_triggers() const override;
        virtual bool show_items() const override;
        virtual void set_selected_trigger(uint32_t number) override;
        virtual void set_selected_light(uint32_t number) override;
        virtual void set_selected_camera_sink(uint32_t number) override;
        virtual std::shared_ptr<ILevelTextureStorage> texture_storage() const override;
        virtual std::set<uint32_t> alternate_groups() const override;
        virtual trlevel::LevelVersion version() const override;
        virtual std::string filename() const override;
        virtual void set_filename(const std::string& filename) override;
        virtual std::vector<uint16_t> floor_data() const override;
        virtual std::weak_ptr<ILight> light(uint32_t index) const override;
        virtual std::vector<std::weak_ptr<ILight>> lights() const override;
        virtual MapColours map_colours() const override;
        virtual void set_map_colours(const MapColours& map_colours) override;
        virtual std::optional<uint32_t> selected_light() const override;
        virtual std::optional<uint32_t> selected_trigger() const override;
        virtual bool has_model(uint32_t type_id) const override;
        virtual std::weak_ptr<ICameraSink> camera_sink(uint32_t index) const override;
        virtual std::vector<std::weak_ptr<ICameraSink>> camera_sinks() const override;
        virtual void set_show_camera_sinks(bool show) override;
        virtual std::optional<uint32_t> selected_camera_sink() const override;
        virtual bool show_camera_sinks() const override;
        void initialise(
            std::shared_ptr<trlevel::ILevel> level,
            std::unique_ptr<IMeshStorage> mesh_storage,
            const IItem::EntitySource& entity_source,
            const IItem::AiSource& ai_source,
            const IRoom::Source& room_source,
            const ITrigger::Source& trigger_source,
            const ILight::Source& light_source,
            const ICameraSink::Source& camera_sink_source,
            const ISoundSource::Source& sound_source_source,
            const trlevel::ILevel::LoadCallbacks callbacks);
        std::vector<std::weak_ptr<IStaticMesh>> static_meshes() const override;
        std::weak_ptr<IStaticMesh> static_mesh(uint32_t index) const override;
        void add_scriptable(const std::weak_ptr<IScriptable>& scriptable) override;
        std::weak_ptr<ISoundStorage> sound_storage() const override;
        std::vector<std::weak_ptr<ISoundSource>> sound_sources() const override;
        void set_show_sound_sources(bool show) override;
        bool show_sound_sources() const override;
    private:
        void generate_rooms(const trlevel::ILevel& level, const IRoom::Source& room_source, const IMeshStorage& mesh_storage);
        void generate_triggers(const ITrigger::Source& trigger_source);
        void generate_entities(const trlevel::ILevel& level, const IItem::EntitySource& entity_source, const IItem::AiSource& ai_source, const IMeshStorage& mesh_storage);
        void regenerate_neighbours();
        void generate_neighbours(std::set<uint16_t>& results, uint16_t selected_room, int32_t max_depth);
        void generate_lights(const trlevel::ILevel& level, const ILight::Source& light_source);
        void generate_camera_sinks(const trlevel::ILevel& level, const ICameraSink::Source& camera_sink_source);
        void generate_sound_sources(const trlevel::ILevel& level, const ISoundSource::Source& sound_source_source);

        // Render the rooms in the level.
        // context: The device context.
        // camera: The current camera to render the level with.
        void render_rooms(const ICamera& camera);

        void render_selected_item(const ICamera& camera);

        struct RoomToRender
        {
            RoomToRender(IRoom& room, IRoom::SelectionMode selection_mode, uint16_t number)
                : room(room), selection_mode(selection_mode), number(number)
            {
            }

            IRoom&               room;
            IRoom::SelectionMode selection_mode;
            uint16_t            number;
        };

        // Get the collection of rooms that need to be renderered depending on the current view mode.
        // Returns: The rooms to render and their selection mode.
        std::vector<RoomToRender> get_rooms_to_render(const ICamera& camera) const;

        // Determines whether the room is currently being rendered.
        // room: The room index.
        // Returns: True if the room is visible.
        bool room_visible(uint32_t room) const;

        // Determines whether the alternate mode specified is a mismatch with the current setting of 
        // the alternate mode flag.
        bool is_alternate_mismatch(const IRoom& room) const;

        bool is_alternate_group_set(uint16_t group) const;
        void apply_ocb_adjustment();
        void deduplicate_triangles();
        void record_models(const trlevel::ILevel& level);
        void record_static_meshes();
        void content_changed();

        std::shared_ptr<graphics::IDevice> _device;
        std::vector<std::shared_ptr<IRoom>>   _rooms;
        std::vector<std::shared_ptr<ITrigger>> _triggers;
        std::vector<std::shared_ptr<IItem>> _entities;
        std::vector<std::shared_ptr<ILight>> _lights;
        std::vector<std::shared_ptr<ICameraSink>> _camera_sinks;
        std::vector<std::weak_ptr<IStaticMesh>> _static_meshes;

        graphics::IShader*          _vertex_shader;
        graphics::IShader*          _pixel_shader;
        Microsoft::WRL::ComPtr<ID3D11SamplerState> _sampler_state;
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> _default_rasterizer;
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> _wireframe_rasterizer;
        std::unique_ptr<graphics::IBuffer> _pixel_shader_data;

        std::set<RoomHighlightMode> _room_highlight_modes;
        
        std::weak_ptr<IRoom> _selected_room;
        std::weak_ptr<IItem> _selected_item;
        std::weak_ptr<ITrigger> _selected_trigger;
        std::weak_ptr<ILight> _selected_light;
        std::weak_ptr<ICameraSink> _selected_camera_sink;
        uint32_t           _neighbour_depth{ 1 };
        std::set<uint16_t> _neighbours;

        std::shared_ptr<ILevelTextureStorage> _texture_storage;
        std::unique_ptr<ITransparencyBuffer> _transparency;

        MapColours _map_colours;

        bool _regenerate_transparency{ true };
        bool _alternate_mode{ false };
        bool _show_wireframe{ false };
        RenderFilter _render_filters{ RenderFilter::Default };

        std::unique_ptr<ISelectionRenderer> _selection_renderer;
        std::set<uint32_t> _alternate_groups;
        trlevel::LevelVersion _version;
        trlevel::Platform _platform;
        std::string _filename;
        std::shared_ptr<ILog> _log;
        std::vector<uint16_t> _floor_data;
        std::set<uint32_t> _models;
        TokenStore _token_store;
        std::string _name;

        std::vector<std::weak_ptr<IScriptable>> _scriptables;
        std::shared_ptr<ISoundStorage> _sound_storage;
        std::vector<std::shared_ptr<ISoundSource>> _sound_sources;
    };

    /// Find the first item with the type id specified.
    /// @param level The level to search.
    /// @param type_id The type id to search for.
    /// @param output_item The item to output the result into.
    /// @returns True if the item was found.
    bool find_item_by_type_id(const ILevel& level, uint32_t type_id, std::weak_ptr<IItem>& output_item);

    /// <summary>
    /// Find the last item with the type id specified.
    /// </summary>
    /// <param name="level">The level to search.</param>
    /// <param name="type_id">The type id to search for.</param>
    /// <param name="output_item">The item to output the result into.</param>
    /// <returns>True if the item was found.</returns>
    bool find_last_item_by_type_id(const ILevel& level, uint32_t type_id, std::weak_ptr<IItem>& output_item);
}

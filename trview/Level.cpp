#include "stdafx.h"
#include "Level.h"

#include <trview.common/FileLoader.h>

namespace trview
{
    Level::Level(CComPtr<ID3D11Device> device, const trlevel::ILevel* level)
        : _device(device), _level(level)
    {
        std::vector<char> vs_data = load_file(L"level_vertex_shader.cso");

        D3D11_INPUT_ELEMENT_DESC input_desc[3];
        memset(&input_desc, 0, sizeof(input_desc));
        input_desc[0].SemanticName = "Position";
        input_desc[0].SemanticIndex = 0;
        input_desc[0].InstanceDataStepRate = 0;
        input_desc[0].InputSlot = 0;
        input_desc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        input_desc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;

        input_desc[1].SemanticName = "Texcoord";
        input_desc[1].SemanticIndex = 0;
        input_desc[1].InstanceDataStepRate = 0;
        input_desc[1].InputSlot = 0;
        input_desc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        input_desc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        input_desc[1].Format = DXGI_FORMAT_R32G32_FLOAT;

        input_desc[2].SemanticName = "Texcoord";
        input_desc[2].SemanticIndex = 1;
        input_desc[2].InstanceDataStepRate = 0;
        input_desc[2].InputSlot = 0;
        input_desc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        input_desc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        input_desc[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

        HRESULT hr = _device->CreateInputLayout(input_desc, 3, &vs_data[0], vs_data.size(), &_input_layout);

        hr = _device->CreateVertexShader(&vs_data[0], vs_data.size(), nullptr, &_vertex_shader);

        std::vector<char> ps_data = load_file(L"level_pixel_shader.cso");
        hr = _device->CreatePixelShader(&ps_data[0], ps_data.size(), nullptr, &_pixel_shader);

        // Create a texture sampler state description.
        D3D11_SAMPLER_DESC sampler_desc;
        memset(&sampler_desc, 0, sizeof(sampler_desc));
        sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampler_desc.MaxAnisotropy = 1;
        sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

        // Create the texture sampler state.
        _device->CreateSamplerState(&sampler_desc, &_sampler_state);

        generate_textures();
        generate_rooms();
        generate_entities();
    }

    std::vector<RoomInfo> Level::room_info() const
    {
        std::vector<RoomInfo> room_infos;
        for (const auto& r : _rooms)
        {
            room_infos.push_back(r->info());
        }
        return room_infos;
    }

    std::vector<Texture> Level::level_textures() const
    {
        return _textures;
    }

    Level::RoomHighlightMode Level::highlight_mode() const
    {
        return _room_highlight_mode;
    }

    void Level::set_highlight_mode(RoomHighlightMode mode)
    {
        if (_room_highlight_mode == mode)
        {
            return;
        }
        
        _room_highlight_mode = mode;
        if (_room_highlight_mode == RoomHighlightMode::Neighbours)
        {
            regenerate_neighbours();
        }
    }

    void Level::set_selected_room(uint16_t index)
    {
        _selected_room = index;
        regenerate_neighbours();
    }

    void Level::set_neighbour_depth(uint32_t depth)
    {
        _neighbour_depth = depth;
        regenerate_neighbours();
    }

    void Level::render(CComPtr<ID3D11DeviceContext> context, DirectX::XMMATRIX view_projection)
    {
        using namespace DirectX;

        context->PSSetSamplers(0, 1, &_sampler_state.p);
        context->IASetInputLayout(_input_layout);
        context->VSSetShader(_vertex_shader, nullptr, 0);
        context->PSSetShader(_pixel_shader, nullptr, 0);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        switch (_room_highlight_mode)
        {
            case RoomHighlightMode::None:
            {
                for (std::size_t i = 0; i < _rooms.size(); ++i)
                {
                    _rooms[i]->render(context, view_projection, _textures, Room::SelectionMode::Selected);
                }
                break;
            }
            case RoomHighlightMode::Highlight:
            {
                for (std::size_t i = 0; i < _rooms.size(); ++i)
                {
                    _rooms[i]->render(context, view_projection, _textures, _selected_room == i ? Room::SelectionMode::Selected : Room::SelectionMode::NotSelected);
                }
                break;
            }
            case RoomHighlightMode::Neighbours:
            {
                for (uint16_t i : _neighbours)
                {
                    _rooms[i]->render(context, view_projection, _textures, i == _selected_room ? Room::SelectionMode::Selected : Room::SelectionMode::Neighbour);
                }
                break;
            }
        }
    }

    void Level::generate_textures()
    {
        _textures.clear();

        // Load the textures from the level and then allow to cycle through them?
        for (uint32_t i = 0; i < _level->num_textiles(); ++i)
        {
            auto t16 = _level->get_textile16(i);

            std::vector<uint32_t> data(256 * 256, 0u);

            uint32_t index = 0;
            for (auto t : t16.Tile)
            {
                data[index++] = trlevel::convert_textile16(t);
            }

            D3D11_SUBRESOURCE_DATA srd;
            memset(&srd, 0, sizeof(srd));
            srd.pSysMem = &data[0];
            srd.SysMemPitch = sizeof(uint32_t) * 256;

            D3D11_TEXTURE2D_DESC desc;
            memset(&desc, 0, sizeof(desc));
            desc.Width = 256;
            desc.Height = 256;
            desc.MipLevels = desc.ArraySize = 1;
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            desc.SampleDesc.Count = 1;
            desc.Usage = D3D11_USAGE_DYNAMIC;
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            desc.MiscFlags = 0;

            Texture tex;
            _device->CreateTexture2D(&desc, &srd, &tex.texture);
            _device->CreateShaderResourceView(tex.texture, nullptr, &tex.view);
            _textures.push_back(tex);
        }
    }

    void Level::generate_rooms()
    {
        const uint16_t num_rooms = _level->num_rooms();
        for (uint16_t i = 0; i < num_rooms; ++i)
        {
            auto room = _level->get_room(i);

            // Convert that room into a Room that we can use in the UI.
            _rooms.push_back(std::make_unique<Room>(_device, *_level, room));
        }
    }

    void Level::generate_entities()
    {
        const uint32_t num_entities = _level->num_entities();
        for (uint32_t i = 0; i < num_entities; ++i)
        {
            auto entity = _level->get_entity(i);
            auto model = _level->get_model(entity.TypeID);
            _entities.push_back(std::make_unique<Entity>());
        }
    }

    void Level::regenerate_neighbours()
    {
        _neighbours = std::set<uint16_t>{ _selected_room };
        if (_selected_room < _level->num_rooms())
        {
            generate_neighbours(_neighbours, _selected_room, _selected_room, 1, _neighbour_depth);
        }
    }

    void Level::generate_neighbours(std::set<uint16_t>& all_rooms, uint16_t previous_room, uint16_t selected_room, int32_t current_depth, int32_t max_depth)
    {
        if (current_depth > max_depth)
        {
            return;
        }

        const auto neighbours = _rooms[selected_room]->neighbours();
        for (auto room = neighbours.begin(); room != neighbours.end(); ++room)
        {
            if (*room != previous_room)
            {
                all_rooms.insert(*room);
                generate_neighbours(all_rooms, selected_room, *room, current_depth + 1, max_depth);
            }
        }
    }
}

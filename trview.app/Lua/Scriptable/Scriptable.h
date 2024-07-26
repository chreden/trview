#pragma once

#include "IScriptable.h"

namespace trview
{
    class Scriptable : public IScriptable, public std::enable_shared_from_this<IScriptable>
    {
    public:
        explicit Scriptable(lua_State* L, const std::shared_ptr<IMesh>& mesh, const graphics::Texture& texture);
        virtual ~Scriptable() = default;
        void click() override;
        int data() const override;
        std::shared_ptr<IMesh> mesh() const override;
        std::string notes() const override;
        DirectX::SimpleMath::Vector3 position() const override;
        void render(const ICamera& camera) override;
        DirectX::SimpleMath::Vector3 screen_position() const override;
        void set_data(int ref) override;
        void set_notes(const std::string& notes) override;
        void set_on_click(int ref) override;
        void set_on_tooltip(int ref) override;
        void set_position(const DirectX::SimpleMath::Vector3& position) override;
        void set_screen_position(const DirectX::SimpleMath::Vector3& position) override;
        std::string tooltip() const override;
    private:
        lua_State* _state;
        int _on_click{ -2 };
        int _on_tooltip{ -2 };
        int _data{ -2 };
        DirectX::SimpleMath::Vector3 _position;
        std::shared_ptr<IMesh> _mesh;
        std::string _notes;
        DirectX::SimpleMath::Vector3 _screen_position;
        graphics::Texture _texture;
    };
}

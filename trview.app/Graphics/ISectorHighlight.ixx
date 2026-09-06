export module trview.app:ISectorHighlight;

import std;

import trview.graphics;

import :ISector;
import :ICamera;

namespace trview
{
    export struct ISectorHighlight
    {
        virtual ~ISectorHighlight() = 0;
        virtual void set_sector(const std::shared_ptr<ISector>& sector, const DirectX::SimpleMath::Matrix& room_offset) = 0;
        virtual void render(const ICamera& camera) = 0;
    };
}

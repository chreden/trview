#pragma once

#include <string>
#include <memory>

#include <trview.common/IFiles.h>
#include "../Settings/RandomizerSettings.h"

#include "IRoute.h"

namespace trview
{
    struct IWaypoint;
    struct IRandomizerRoute : public IRoute
    {
        using Source = std::function<std::shared_ptr<IRandomizerRoute>()>;
        virtual ~IRandomizerRoute() = 0;
        virtual std::shared_ptr<IWaypoint> add(const std::string& level_name, const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room_number) = 0;
    };

    std::shared_ptr<IRoute> import_randomizer_route(const IRandomizerRoute::Source& route_source, const std::shared_ptr<IFiles>& files, const std::string& route_filename, const RandomizerSettings& randomizer_settings);
}
#pragma once

#include <external/boost/di.hpp>
#include "Route.h"

namespace trview
{
    auto register_app_routing_module() noexcept
    {
        using namespace boost;
        return di::make_injector(
            di::bind<IRoute>.to<Route>(),
            di::bind<IRoute::Source>.to(
                [](const auto& injector) -> IRoute::Source
                {
                    return [&]()
                    {
                        return injector.create<std::unique_ptr<IRoute>>();
                    };
                })
        );
    }
}


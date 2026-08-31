export module trview.app:IMeshStorage;

import std;
import std.compat;

import trlevel;

import :ILevelTextureStorage;

namespace trview
{
    struct IMesh;
    export struct IMeshStorage
    {
        using Source = std::function<std::unique_ptr<IMeshStorage>(const trlevel::ILevel&, const ILevelTextureStorage&)>;
        virtual ~IMeshStorage() = 0;
        virtual std::shared_ptr<IMesh> mesh(uint32_t mesh_pointer) const = 0;
    };
}

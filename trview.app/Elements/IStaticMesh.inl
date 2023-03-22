#pragma once

namespace trview
{
    constexpr std::string to_string(IStaticMesh::Type type) noexcept
    {
        switch (type)
        {
        case IStaticMesh::Type::Mesh:
            return "Mesh";
        case IStaticMesh::Type::Sprite:
            return "Sprite";
        }
        return "Unknown";
    }
}

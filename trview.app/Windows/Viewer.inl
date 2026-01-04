#pragma once

namespace trview
{
    template <typename T>
    std::shared_ptr<T> Viewer::get_entity_and_sync_level(const std::weak_ptr<T>& entity)
    {
        if (const auto entity_ptr = entity.lock())
        {
            const auto level_ptr = entity_ptr->level().lock();
            open(level_ptr, ILevel::OpenMode::Reload);
            return entity_ptr;
        }
        return nullptr;
    }
}

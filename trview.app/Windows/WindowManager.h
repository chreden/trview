#pragma once

#include <trview.common/TokenStore.h>

namespace trview
{
    /// <summary>
    /// Handles common functions for window managers.
    /// </summary>
    /// <typeparam name="T">The type of window to manage.</typeparam>
    template <typename T>
    class WindowManager
    {
    protected:
        /// <summary>
        /// Renders all managed windows.
        /// </summary>
        void render();
        /// <summary>
        /// Updates all managed windows.
        /// </summary>
        /// <param name="delta">The time elapsed since the previous update.</param>
        void update(float delta);
        /// <summary>
        /// Adds and numbers a window.
        /// </summary>
        /// <param name="window">The new window to add.</param>
        /// <returns>The added window.</returns>
        std::weak_ptr<T> add_window(const std::shared_ptr<T>& window);
        TokenStore _token_store;
        std::unordered_map<std::string, std::unordered_map<int32_t, std::shared_ptr<T>>> _windows;
    private:
        /// <summary>
        /// Find the next ID.
        /// </summary>
        /// <returns>The next available window number.</returns>
        int32_t next_id(const std::string& type) const;
        std::vector<std::pair<std::string, int32_t>> _closing_windows;
    };
}

#include "WindowManager.hpp"

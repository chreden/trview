#pragma once

struct lua_State;

namespace trview
{
    struct ILevel;
    class Item;

    namespace lua
    {
        /// <summary>
        /// Create a new Item and push it on to the stack.
        /// </summary>
        /// <param name="L">The Lua state</param>
        /// <param name="item">The item to bind</param>
        void create_item(lua_State* L, const Item& item);

        /// <summary>
        /// Set the current level.
        /// </summary>
        /// <param name="level">The current level instance.</param>
        void item_set_current_level(ILevel* level);
    }
}

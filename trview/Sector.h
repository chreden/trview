#pragma once

#include <memory>
#include <vector>
#include "MapTypes.h" 

namespace trview
{
    struct Sector
    {
    public:
        std::int8_t         floor, ceiling;
        int                 row, column;
        ui::Point           origin, last; // Origin point, last point (bottom right corner 

        // Constructs sector object 
        Sector(int p_row, int p_column); 

        // Returns the first floordata that has the specified function 
        // Returns nullptr if no such floordata
        std::unique_ptr<Floor> at(const FunctionType& func) const; 

        // Returns the floordata at the specified index 
        // Returns nullptr if no such floordata
        std::unique_ptr<Floor> at(int index) const;

        // Adds a new floordata entry to this sector 
        void add(const Floor& floor); 

        // Returns the number of floordata(s) this sector has 
        std::size_t size() const; 

        // Returns true if function is a wall, false otherwise 
        bool is_wall() const; 

        // Returns true if sector has the specified function, false otherwise 
        bool has_function(const FunctionType& func) const; 

        // Quick lookups rather than iterating _floor_data 
        bool is_portal = false; 
        bool is_trigger = false; 
        bool is_death = false; 

    private: 
        std::vector<Floor>  _floor_data;
    };
}
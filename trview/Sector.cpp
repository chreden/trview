#include "stdafx.h"
#include "Sector.h"

namespace trview
{
    // Constructs sector object 
    Sector::Sector(int p_row, int p_column)
        : row(p_row), column(p_column)
    {
    }

    // Returns the first floordata that has the specified function 
    // Returns nullptr if no such floordata
    std::unique_ptr<Floor> 
    Sector::at(const FunctionType& func) const
    {
        auto floor = std::find_if(_floor_data.begin(), _floor_data.end(), [&func] (const Floor& floor) {
            return floor.function == func; 
        }); 

        if (floor == _floor_data.end())
            return nullptr;
        else
            return std::make_unique <Floor> (*floor);
    }

    // Returns the floordata at the specified index 
    // Returns nullptr if no such floordata
    std::unique_ptr<Floor> 
    Sector::at(int index) const
    {
        if (index < _floor_data.size())
            return std::make_unique<Floor>(_floor_data[index]); 
        else
            return nullptr;
    }

    // Adds a new floordata entry to this sector 
    void 
    Sector::add(const Floor& floor)
    {
        switch (floor.function)
        {
            case FunctionType::PORTAL: is_portal = true; break;
            case FunctionType::TRIGGER: is_trigger = true; break; 
            case FunctionType::KILL: is_death = true; break; 
        }; 

        _floor_data.push_back(floor); 
    }

    // Returns the number of floordata(s) this sector has 
    std::size_t 
    Sector::size() const
    {
        return _floor_data.size(); 
    }

    // Returns true if function is a wall, false otherwise 
    bool 
    Sector::is_wall() const
    {
        return floor == -127 && ceiling == -127; 
    }

    // Returns true if sector has the specified function, false otherwise 
    bool 
    Sector::has_function(const FunctionType& func) const
    {
        auto& floor = at(func); 
        return (floor != nullptr); 
    }
}


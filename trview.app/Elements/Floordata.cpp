#include "Floordata.h"
#include "ITrigger.h"

namespace trview
{
    namespace
    {
        Floordata::Command::Function extract_function(uint16_t floor, std::optional<trlevel::PlatformAndVersion> version)
        {
            Floordata::Command::Function function = static_cast<Floordata::Command::Function>(floor & 0x1f);
            if (!version.has_value() || !is_tr1_may_1996(version.value()))
            {
                return function;
            }

            using namespace trlevel;
            using Function = Floordata::Command::Function;
            switch (function)
            {
            case Function::Portal:
                return Function::Death;
            case Function::Death:
                return Function::CeilingSlant;
                break;
            case Function::Trigger:
                return Function::FloorSlant;
                break;
            case Function::CeilingSlant:
                return Function::Trigger;
                break;
            case Function::FloorSlant:
                return Function::Portal;
                break;
            }
            return function;
        }
    }

    Floordata::Command::Command(Function type, const std::vector<uint16_t>& data, FloordataMeanings meanings, const std::vector<std::weak_ptr<IItem>>& items, bool trng)
        : type(type), data(data)
    {
        if (meanings == FloordataMeanings::Generate)
        {
            create_meanings(items, trng);
        }
    }

    void Floordata::Command::create_meanings(const std::vector<std::weak_ptr<IItem>>& items, bool trng)
    {
        // Parse the data to create the meanings.
        const uint16_t subfunction = (data[0] & 0x7F00) >> 8;
        const std::string name = to_string(type);

        switch (type)
        {
            case Function::None:
            case Function::Death:
            case Function::MonkeySwing:
            case Function::ClimbableWall:
            case Function::MinecartLeft_DeferredTrigger:
            case Function::MinecartRight_Mapper:
            {
                meanings.push_back(name);
                break;
            }
            case Function::Triangulation_Floor_NWSE:
            case Function::Triangulation_Floor_NESW:
            case Function::Triangulation_Floor_Collision_SW:
            case Function::Triangulation_Floor_Collision_NE:
            case Function::Triangulation_Floor_Collision_SE:
            case Function::Triangulation_Floor_Collision_NW:
            {
                const auto tri = parse_triangulation(data[0], data[1]);
                meanings.push_back(name);
                meanings.push_back(std::format("  NW: {}, NE: {}, SE: {}, SW: {}", tri.c01, tri.c11, tri.c10, tri.c00));
                break;
            }
            case Function::Triangulation_Ceiling_NW:
            case Function::Triangulation_Ceiling_NE:
            case Function::Triangulation_Ceiling_Collision_SW:
            case Function::Triangulation_Ceiling_Collision_NE:
            case Function::Triangulation_Ceiling_Collision_NW:
            case Function::Triangulation_Ceiling_Collision_SE:
            {
                const auto tri = parse_triangulation(data[0], data[1]);
                meanings.push_back(name);
                meanings.push_back(std::format("  NW: {}, NE: {}, SE: {}, SW: {}", tri.c00, tri.c10, tri.c11, tri.c01));
                break;
            }
            case Function::Portal:
            {
                meanings.push_back(name);
                meanings.push_back("  Room " + std::to_string(data[1] & 0xff));
                break;
            }
            case Function::FloorSlant:
            {
                meanings.push_back(name);

                uint16_t floor_slant = data[1];
                const int8_t x_slope = floor_slant & 0x00ff;
                const int8_t z_slope = floor_slant >> 8;
                meanings.push_back("  X:" + std::to_string(x_slope) + ", Z:" + std::to_string(z_slope));
                break;
            }
            case Function::CeilingSlant:
            {
                meanings.push_back(name);

                uint16_t ceiling_slant = data[1];
                const int8_t x_slope = ceiling_slant & 0x00ff;
                const int8_t z_slope = ceiling_slant >> 8;
                meanings.push_back("  X:" + std::to_string(x_slope) + ", Z:" + std::to_string(z_slope));
                break;
            }
            case Function::Trigger:
            {
                uint32_t i = 0;
                std::uint16_t command = 0;
                std::uint16_t setup = data[++i];

                auto trigger_type = (TriggerType)subfunction;
                meanings.push_back(to_string(trigger_type));
                meanings.push_back("  Timer:" + std::to_string(setup & 0xff) + ", Only Once:" + std::to_string((setup & 0x100) >> 8) + ", Mask:" + format_binary((setup & 0x3e00) >> 9));

                bool continue_processing = true;
                if (trigger_type == TriggerType::Key || trigger_type == TriggerType::Switch)
                {
                    auto reference = data[++i];
                    continue_processing = (reference & 0x8000) == 0;
                    meanings.push_back("  Lock/Switch " + std::to_string(reference));
                }

                if (continue_processing)
                {
                    do
                    {
                        if (++i < data.size())
                        {
                            command = data[i];
                            auto action = static_cast<TriggerCommandType>((command & 0x7C00) >> 10);
                            auto index = static_cast<uint16_t>(command & 0x3FF);

                            std::string meaning = "  " + command_type_name(action);
                            if (command_has_index(action))
                            {
                                meaning += " " + std::to_string(index);
                                if (command_is_item(action) && index < items.size())
                                {
                                    if (const auto item = items[index].lock())
                                    {
                                        meaning += " - " + item->type();
                                    }
                                }
                            }

                            meanings.push_back(meaning);
                            if (action == TriggerCommandType::Camera || action == TriggerCommandType::Flyby)
                            {
                                command = data[++i];
                                meanings.push_back(std::format("    Once: {}, Timer: {}, MoveTimer: {}, ContBit: {}", 
                                    (command & 0x100) != 0,
                                    command & 0xff,
                                    (command & 0x3e00) >> 9,
                                    (command & 0x8000) >> 15));
                            }
                            else if (trng && action == TriggerCommandType::Flipeffect)
                            {
                                command = data[++i];
                                meanings.push_back(std::format("    Extra TRNG value: {}", command & 0x7fff));
                            }
                        }

                    } while (i < data.size() && !(command & 0x8000));
                }

                break;
            }
        }
    }

    uint32_t Floordata::size() const
    {
        uint32_t sum = 0;
        for (const auto& command : commands)
        {
            sum += static_cast<uint32_t>(command.data.size());
        }
        return sum;
    }

    Floordata parse_floordata(const std::vector<uint16_t>& floordata, uint32_t index, FloordataMeanings meanings, bool trng, std::optional<trlevel::PlatformAndVersion> version)
    {
        return parse_floordata(floordata, index, meanings, {}, trng, version);
    }


    Floordata parse_floordata(const std::vector<uint16_t>& floordata, uint32_t index, FloordataMeanings meanings, const std::vector<std::weak_ptr<IItem>>& items, bool trng, std::optional<trlevel::PlatformAndVersion> version)
    {
        Floordata result;

        if (index == 0)
        {
            result.commands.push_back(Floordata::Command(Floordata::Command::Function::None, { floordata[index] }, meanings, items, trng));
            return result;
        }
        
        while (true)
        {
            // Parse the floordata here.
            const uint16_t floor = floordata[index];
            Floordata::Command::Function function = extract_function(floor, version);
            uint16_t subfunction = (floor & 0x7F00) >> 8;

            using Function = Floordata::Command::Function;

            std::vector<uint16_t> data;
            data.push_back(floor);

            switch (function)
            {
                case Function::Trigger:
                {
                    std::uint16_t trigger_command = 0;
                    std::uint16_t setup = floordata[++index];
                    data.push_back(setup);

                    auto type = (TriggerType)subfunction;

                    bool continue_processing = true;
                    if (type == TriggerType::Key || type == TriggerType::Switch)
                    {
                        auto reference = floordata[++index];
                        data.push_back(reference);
                        continue_processing = (reference & 0x8000) == 0;
                    }

                    if (continue_processing)
                    {
                        do
                        {
                            if (++index < floordata.size())
                            {
                                trigger_command = floordata[index];
                                data.push_back(trigger_command);
                                auto action = static_cast<TriggerCommandType>((trigger_command & 0x7C00) >> 10);
                                if (action == TriggerCommandType::Camera ||
                                    action == TriggerCommandType::Flyby ||
                                    (trng && action == TriggerCommandType::Flipeffect))
                                {
                                    // Camera has another uint16_t - skip for now.
                                    trigger_command = floordata[++index];
                                    data.push_back(trigger_command);
                                }
                            }

                        } while (index < floordata.size() && !(trigger_command & 0x8000));
                    }

                    break;
                }
                case Function::Portal:
                case Function::FloorSlant:
                case Function::CeilingSlant:
                case Function::Triangulation_Floor_NWSE:
                case Function::Triangulation_Floor_NESW:
                case Function::Triangulation_Floor_Collision_SW:
                case Function::Triangulation_Floor_Collision_NE:
                case Function::Triangulation_Floor_Collision_SE:
                case Function::Triangulation_Floor_Collision_NW:
                case Function::Triangulation_Ceiling_NW:
                case Function::Triangulation_Ceiling_NE:
                case Function::Triangulation_Ceiling_Collision_SW:
                case Function::Triangulation_Ceiling_Collision_NE:
                case Function::Triangulation_Ceiling_Collision_NW:
                case Function::Triangulation_Ceiling_Collision_SE:
                {
                    data.push_back(floordata[++index]);
                    break;
                }
                case Function::MonkeySwing:
                case Function::MinecartLeft_DeferredTrigger:
                case Function::MinecartRight_Mapper:
                case Function::Death:
                case Function::ClimbableWall:
                {
                    break;
                }
            }

            result.commands.push_back(Floordata::Command(function, data, meanings, items, trng));

            if ((floor >> 15) || index == 0)
            {
                break;
            }
            else
            {
                ++index;
            }
        }

        return result;
    }

    std::string to_string(Floordata::Command::Function function)
    {
        switch (function)
        {
            case Floordata::Command::Function::None:
                return "None";
            case Floordata::Command::Function::Portal:
                return "Portal";
            case Floordata::Command::Function::FloorSlant:
                return "Floor Slant";
            case Floordata::Command::Function::CeilingSlant:
                return "Ceiling Slant";
            case Floordata::Command::Function::Trigger:
                return "Trigger";
            case Floordata::Command::Function::Death:
                return "Death";
            case Floordata::Command::Function::ClimbableWall:
                return "Climbable Wall";
            case Floordata::Command::Function::Triangulation_Floor_NWSE:
                return "Floor Triangulation (NWSE)";
            case Floordata::Command::Function::Triangulation_Floor_NESW:
                return "Floor Triangulation (NESW)";
            case Floordata::Command::Function::Triangulation_Ceiling_NW:
                return "Ceiling Triangulation (NW)";
            case Floordata::Command::Function::Triangulation_Ceiling_NE:
                return "Ceiling Triangulation (NE)";
            case Floordata::Command::Function::Triangulation_Floor_Collision_SW:
                return "Floor Triangulation Collision (SW)";
            case Floordata::Command::Function::Triangulation_Floor_Collision_NE:
                return "Floor Triangulation Collision (NE)";
            case Floordata::Command::Function::Triangulation_Floor_Collision_SE:
                return "Floor Triangulation Collision (SE)";
            case Floordata::Command::Function::Triangulation_Floor_Collision_NW:
                return "Floor Triangulation Collision (NW)";
            case Floordata::Command::Function::Triangulation_Ceiling_Collision_SW:
                return "Ceiling Triangulation Collision (SW)";
            case Floordata::Command::Function::Triangulation_Ceiling_Collision_NE:
                return "Ceiling Triangulation Collision (NE)";
            case Floordata::Command::Function::Triangulation_Ceiling_Collision_NW:
                return "Ceiling Triangulation Collision (NW)";
            case Floordata::Command::Function::Triangulation_Ceiling_Collision_SE:
                return "Ceiling Triangulation Collision (SE)";
            case Floordata::Command::Function::MonkeySwing:
                return "Monkey Swing";
            case Floordata::Command::Function::MinecartLeft_DeferredTrigger:
                return "Minecart Left/Deferred Trigger";
            case Floordata::Command::Function::MinecartRight_Mapper:
                return "Minecart Right/Mapper";
        }
        return "";
    }

    Triangulation parse_triangulation(uint16_t floor, uint16_t data)
    {
        // Not sure what to do with h1 and h2 values yet.
        // const int16_t h2 = (floor & 0x7C00) >> 10;
        // const int16_t h1 = (floor & 0x03E0) >> 5;
        const int16_t function = (floor & 0x001F);

        TriangulationDirection direction{ TriangulationDirection::None };
        switch (function)
        {
            // Floor
        case 0x07:
        case 0x0B:
        case 0x0C:
            // Ceiling:
        case 0x09:
        case 0x0f:
        case 0x10:
            direction = TriangulationDirection::NwSe;
            break;
            // Floor
        case 0x08:
        case 0x0D:
        case 0x0E:
            // Ceiling:
        case 0x0A:
        case 0x11:
        case 0x12:
            direction = TriangulationDirection::NeSw;
            break;
        }

        const uint16_t c00 = (data & 0x00F0) >> 4;
        const uint16_t c01 = (data & 0x0F00) >> 8;
        const uint16_t c10 = (data & 0x000F);
        const uint16_t c11 = (data & 0xF000) >> 12;
        const auto max_corner = std::max({ c00, c01, c10, c11 });
        return Triangulation{ function, direction, (max_corner - c00) * 0.25f, (max_corner - c01) * 0.25f, (max_corner - c10) * 0.25f, (max_corner - c11) * 0.25f };
    }
}
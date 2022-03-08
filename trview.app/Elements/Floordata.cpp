#include "Floordata.h"
#include "ITrigger.h"

namespace trview
{
    Floordata::Command::Command(Function type, const std::vector<uint16_t>& data, FloordataMeanings meanings, const std::vector<Item>& items)
        : type(type), data(data)
    {
        if (meanings == FloordataMeanings::Generate)
        {
            create_meanings(items);
        }
    }

    void Floordata::Command::create_meanings(const std::vector<Item>& items)
    {
        // Parse the data to create the meanings.
        const uint16_t subfunction = (data[0] & 0x7F00) >> 8;

        switch (type)
        {
            case Function::None:
            {
                meanings.push_back("None");
                break;
            }
            case Function::Portal:
            {
                meanings.push_back("Portal");
                meanings.push_back("  Room " + std::to_string(data[1] & 0xff));
                break;
            }
            case Function::FloorSlant:
            {
                meanings.push_back("Floor Slant");

                uint16_t floor_slant = data[1];
                const int8_t x_slope = floor_slant & 0x00ff;
                const int8_t z_slope = floor_slant >> 8;
                meanings.push_back("  X:" + std::to_string(x_slope) + ", Z:" + std::to_string(z_slope));
                break;
            }
            case Function::CeilingSlant:
            {
                meanings.push_back("Ceiling Slant");

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
                meanings.push_back(trigger_type_name(trigger_type));
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
                                    const auto item = items[index];
                                    meaning += " - " + item.type();
                                }
                            }

                            meanings.push_back(meaning);
                            if (action == TriggerCommandType::Camera || action == TriggerCommandType::Flyby)
                            {
                                // Camera has another uint16_t - skip for now.
                                command = data[++i];
                                meanings.push_back("    " + std::to_string(command));
                            }
                        }

                    } while (i < data.size() && !(command & 0x8000));
                }

                break;
            }
            case Function::Death:
            {
                meanings.push_back("Death");
                break;
            }
            case Function::ClimbableWall:
            {
                meanings.push_back("Climbable wall");
                break;
            }
            case Function::Triangulation_Floor_NWSE:
            {
                meanings.push_back("Floor triangulation");
                meanings.push_back("");
                break;
            }
            case Function::Triangulation_Floor_NESW:
            {
                meanings.push_back("Floor triangulation");
                meanings.push_back("");
                break;
            }
            case Function::Triangulation_Ceiling_NW:
            {
                meanings.push_back("Ceiling triangulation");
                meanings.push_back("");
                break;
            }
            case Function::Triangulation_Ceiling_NE:
            {
                meanings.push_back("Ceiling triangulation");
                meanings.push_back("");
                break;
            }
            case Function::Triangulation_Floor_Collision_SW:
            {
                meanings.push_back("Floor triangulation");
                meanings.push_back("");
                break;
            }
            case Function::Triangulation_Floor_Collision_NE:
            {
                meanings.push_back("Floor triangulation");
                meanings.push_back("");
                break;
            }
            case Function::Triangulation_Floor_Collision_SE:
            {
                meanings.push_back("Floor triangulation");
                meanings.push_back("");
                break;
            }
            case Function::Triangulation_Floor_Collision_NW:
            {
                meanings.push_back("Floor triangulation");
                meanings.push_back("");
                break;
            }
            case Function::Triangulation_Ceiling_Collision_SW:
            {
                meanings.push_back("Ceiling triangulation");
                meanings.push_back("");
                break;
            }
            case Function::Triangulation_Ceiling_Collision_NE:
            {
                meanings.push_back("Ceiling triangulation");
                meanings.push_back("");
                break;
            }
            case Function::Triangulation_Ceiling_Collision_NW:
            {
                meanings.push_back("Ceiling triangulation");
                meanings.push_back("");
                break;
            }
            case Function::Triangulation_Ceiling_Collision_SE:
            {
                meanings.push_back("Ceiling triangulation");
                meanings.push_back("");
                break;
            }
            case Function::MonkeySwing:
            {
                meanings.push_back("Monkey swing");
                break;
            }
            case Function::MinecartLeft_DeferredTrigger:
            {
                meanings.push_back("Minecart left");
                break;
            }
            case Function::MinecartRight_Mapper:
            {
                meanings.push_back("Minecart right");
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

    Floordata parse_floordata(const std::vector<uint16_t>& floordata, uint32_t index, FloordataMeanings meanings)
    {
        return parse_floordata(floordata, index, meanings, std::vector<Item>{});
    }


    Floordata parse_floordata(const std::vector<uint16_t>& floordata, uint32_t index, FloordataMeanings meanings, const std::vector<Item>& items)
    {
        Floordata result;

        if (index == 0)
        {
            result.commands.push_back(Floordata::Command(Floordata::Command::Function::None, { floordata[index] }, FloordataMeanings::None, items));
            return result;
        }
        
        while (true)
        {
            // Parse the floordata here.
            const uint16_t floor = floordata[index];
            Floordata::Command::Function function = static_cast<Floordata::Command::Function>(floor & 0x1f);
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
                                if (action == TriggerCommandType::Camera || action == TriggerCommandType::Flyby)
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

            result.commands.push_back(Floordata::Command(function, data, meanings, items));

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
}
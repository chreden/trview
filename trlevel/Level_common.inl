#pragma once

namespace trlevel
{
    template <typename T>
    T peek(std::basic_ispanstream<uint8_t>& file)
    {
        T value;
        const auto at = file.tellg();
        read<T>(file, value);
        file.seekg(at);
        return value;
    }

    template <typename T>
    void read(std::basic_ispanstream<uint8_t>& file, T& value)
    {
        file.read(reinterpret_cast<uint8_t*>(&value), sizeof(value));
    }

    template <typename T>
    T read(std::basic_ispanstream<uint8_t>& file)
    {
        T value;
        read<T>(file, value);
        return value;
    }

    template < typename DataType, typename SizeType >
    std::vector<DataType> read_vector(std::basic_ispanstream<uint8_t>& file, SizeType size)
    {
        std::vector<DataType> data(size);
        for (SizeType i = 0; i < size; ++i)
        {
            read<DataType>(file, data[i]);
        }
        return data;
    }

    template < typename SizeType, typename DataType >
    std::vector<DataType> read_vector(std::basic_ispanstream<uint8_t>& file)
    {
        auto size = read<SizeType>(file);
        return read_vector<DataType, SizeType>(file, size);
    }

    template < typename DataType >
    std::vector<DataType> read_vector_compressed(std::basic_ispanstream<uint8_t>& file, uint32_t elements)
    {
        const auto uncompressed_data = read_compressed(file);
        std::basic_ispanstream<uint8_t> data_stream{ { uncompressed_data } };
        data_stream.exceptions(std::ios::failbit | std::ios::badbit | std::ios::eofbit);
        return read_vector<DataType>(data_stream, elements);
    }

    template <typename size_type>
    std::vector<tr3_room> read_rooms(
        trview::Activity& activity,
        std::basic_ispanstream<uint8_t>& file,
        const ILevel::LoadCallbacks& callbacks,
        std::function<void(trview::Activity& activity, std::basic_ispanstream<uint8_t>&, tr3_room&)> load_function)
    {
        std::vector<tr3_room> rooms;

        log_file(activity, file, "Reading number of rooms");
        const size_type num_rooms = read<size_type>(file);

        callbacks.on_progress(std::format("Reading {} rooms", num_rooms));
        log_file(activity, file, std::format("Reading {} rooms", num_rooms));
        for (auto i = 0u; i < num_rooms; ++i)
        {
            trview::Activity room_activity(activity, std::format("Room {}", i));
            callbacks.on_progress(std::format("Reading room {}", i));
            log_file(room_activity, file, std::format("Reading room {}", i));
            tr3_room room;
            load_function(room_activity, file, room);

            log_file(room_activity, file, std::format("Read room {}", i));
            rooms.push_back(room);
        }

        return rooms;
    }
}

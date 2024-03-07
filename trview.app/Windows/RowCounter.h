#pragma once

namespace trview
{
    class RowCounter
    {
    public:
        RowCounter(const std::string& entry_name, std::size_t maximum_size);
        float height() const;
        void count();
        void set_count(std::size_t value);
        void render();
    private:
        std::string _entry_name;
        std::size_t _maximum_size;
        std::size_t _count;
    };
}

#pragma once

#include "IFiles.h"

namespace trview
{
    class Files final : public IFiles
    {
    public:
        virtual ~Files() = default;
        virtual void save_file(const std::string& filename, const std::vector<uint8_t>& bytes) const override;
    private:

    };
}

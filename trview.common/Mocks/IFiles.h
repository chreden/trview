#include "../IFiles.h"

namespace trview
{
    struct MockFiles final : public IFiles
    {
        virtual ~MockFiles() = default;
        MOCK_METHOD(void, save_file, (const std::string&, const std::vector<uint8_t>&), (const, override));
    };
}

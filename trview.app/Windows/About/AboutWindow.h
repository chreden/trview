#pragma once

#include "../IWindow.h"

namespace trview
{
    class AboutWindow final : public IWindow
    {
    public:
        virtual ~AboutWindow() = default;
        void update(float delta) override;
        void render() override;
        void set_number(int32_t number) override;
        std::string type() const override;
        void receive_message(const Message&) override;
    private:
        std::string _id{ "About 0" };
    };
}

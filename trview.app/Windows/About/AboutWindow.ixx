export module trview.app:AboutWindow;

import :IWindow;

namespace trview
{
    export class AboutWindow final : public IWindow
    {
    public:
        virtual ~AboutWindow() = default;
        void update(float delta) override;
        void render() override;
        void set_number(int32_t number) override;
        std::string type() const override;
        std::string title() const override;
        void receive_message(const Message&) override;
    private:
        std::string _id{ "About 0" };
    };
}

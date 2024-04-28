#pragma once

#include <trview.app/UI/IImGuiBackend.h>

namespace trview
{
    namespace tests
    {
        class NullImGuiBackend final : public IImGuiBackend
        {
        public:
            virtual ~NullImGuiBackend() = default;
            void initialise() override;
            bool is_setup() const override;
            void new_frame() override;
            void rebuild_fonts() override;
            void render() override;
            void reset_layout() override;
            void shutdown() override;
            bool window_procedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
        private:
            bool _active { false };
        };
    }
}
#pragma once

#include <trview.app/UI/IImGuiBackend.h>

namespace trview
{
    namespace tests
    {
        class NullImGuiBackend : public IImGuiBackend
        {
        public:
            virtual ~NullImGuiBackend() = default;
            virtual void initialise() override;
            virtual void new_frame() override;
            virtual void render() override;
            virtual void shutdown() override;
            virtual bool window_procedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
        };
    }
}
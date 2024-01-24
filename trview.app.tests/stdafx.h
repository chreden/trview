#pragma once

#include <optional>
#include <set>

#include <Windows.h>
#include <shellapi.h>
#include <ShlObj.h>

#include <trview.common/TokenStore.h>
#include <trview.graphics/IShaderStorage.h>
#include <trview.graphics/IShader.h>
#include <trview.tests.common/Window.h>
#include <trview.tests.common/Mocks.h>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

// Fix for printing ComPtr - removes value printing:
namespace testing
{
    namespace internal
    {
        template <typename T>
        class UniversalPrinter<const Microsoft::WRL::ComPtr<T>&> {
        public:
            // MSVC warns about adding const to a function type, so we want to
            // disable the warning.
            GTEST_DISABLE_MSC_WARNINGS_PUSH_(4180)

                static void Print(const Microsoft::WRL::ComPtr<T>& value, ::std::ostream* os) {
                UniversalPrint(value, os);
            }

            GTEST_DISABLE_MSC_WARNINGS_POP_()
        };
    }
}

// Mock headers
#include <trlevel/Mocks/ILevel.h>
#include <trview.app/Mocks/Camera/ICamera.h>
#include <trview.app/Mocks/Graphics/ILevelTextureStorage.h>
#include <trview.app/Mocks/Geometry/IMesh.h>
#include <trview.app/Mocks/Geometry/IPicking.h>
#include <trview.app/Mocks/Graphics/IMeshStorage.h>
#include <trview.app/Mocks/Graphics/ISectorHighlight.h>
#include <trview.app/Mocks/Graphics/ISelectionRenderer.h>
#include <trview.app/Mocks/Graphics/ITextureStorage.h>
#include <trview.app/Mocks/Geometry/ITransparencyBuffer.h>
#include <trview.app/Mocks/Elements/IItem.h>
#include <trview.app/Mocks/Elements/ILevel.h>
#include <trview.app/Mocks/Elements/IStaticMesh.h>
#include <trview.app/Mocks/Elements/ITrigger.h>
#include <trview.app/Mocks/Elements/ITypeInfoLookup.h>
#include <trview.app/Mocks/Menus/IUpdateChecker.h>
#include <trview.app/Mocks/Routing/IRoute.h>
#include <trview.app/Mocks/Routing/IWaypoint.h>
#include <trview.app/Mocks/Settings/ISettingsLoader.h>
#include <trview.app/Mocks/Settings/IStartupOptions.h>
#include <trview.app/Mocks/Tools/ICompass.h>
#include <trview.app/Mocks/Tools/IMeasure.h>
#include <trview.app/Mocks/UI/ICameraControls.h>
#include <trview.app/Mocks/UI/IContextMenu.h>
#include <trview.app/Mocks/UI/ISettingsWindow.h>
#include <trview.app/Mocks/UI/IViewOptions.h>
#include <trview.app/Mocks/UI/IViewerUI.h>
#include <trview.app/Mocks/Windows/IItemsWindow.h>
#include <trview.app/Mocks/Windows/IItemsWindowManager.h>
#include <trview.app/Mocks/Windows/IRoomsWindow.h>
#include <trview.app/Mocks/Windows/IRoomsWindowManager.h>
#include <trview.app/Mocks/Windows/IRouteWindow.h>
#include <trview.app/Mocks/Windows/IRouteWindowManager.h>
#include <trview.app/Mocks/Windows/ITriggersWindow.h>
#include <trview.app/Mocks/Windows/ITriggersWindowManager.h>
#include <trview.app/Mocks/Windows/IViewer.h>
#include <trview.common/Mocks/IFiles.h>
#include <trview.common/Mocks/Windows/IClipboard.h>
#include <trview.common/Mocks/Windows/IDialogs.h>
#include <trview.common/Mocks/Windows/IShell.h>
#include <trview.common/Mocks/Windows/IShortcuts.h>
#include <trview.graphics/mocks/D3D/ID3D11DeviceContext.h>
#include <trview.graphics/mocks/IDevice.h>
#include <trview.graphics/mocks/IDeviceWindow.h>
#include <trview.graphics/mocks/IRenderTarget.h>
#include <trview.graphics/mocks/IShader.h>
#include <trview.graphics/mocks/IShaderStorage.h>
#include <trview.graphics/mocks/ISprite.h>
#include <trview.input/Mocks/IMouse.h>
#include <trview.app/Mocks/UI/IMapRenderer.h>

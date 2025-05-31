#include "pch.h"
#include <gmock/gmock.h>

#include "Camera/ICamera.h"
#include "Elements/IItem.h"
#include "Elements/ILevel.h"
#include "Elements/ILight.h"
#include "Elements/INgPlusSwitcher.h"
#include "Elements/IRoom.h"
#include "Elements/ISector.h"
#include "Elements/IStaticMesh.h"
#include "Elements/ISoundSource.h"
#include "Elements/ITrigger.h"
#include "Elements/ITypeInfoLookup.h"
#include "Geometry/IMesh.h"
#include "Geometry/IPicking.h"
#include "Geometry/ITransparencyBuffer.h"
#include "Geometry/IModelStorage.h"
#include "Graphics/ILevelTextureStorage.h"
#include "Graphics/IMeshStorage.h"
#include "Graphics/ISectorHighlight.h"
#include "Graphics/ISelectionRenderer.h"
#include "Graphics/ITextureStorage.h"
#include "Lua/IScriptable.h"
#include "Menus/IFileMenu.h"
#include "Menus/IUpdateChecker.h"
#include "Routing/IRoute.h"
#include "Routing/IWaypoint.h"
#include "Routing/IRandomizerRoute.h"
#include "Settings/ISettingsLoader.h"
#include "Settings/IStartupOptions.h"
#include "Sound/ISound.h"
#include "Sound/ISoundStorage.h"
#include "Tools/ICompass.h"
#include "Tools/IMeasure.h"
#include "UI/ICameraControls.h"
#include "UI/IContextMenu.h"
#include "UI/IFonts.h"
#include "UI/IImGuiBackend.h"
#include "UI/IMapRenderer.h"
#include "UI/ISettingsWindow.h"
#include "UI/IViewerUI.h"
#include "UI/IViewOptions.h"
#include "Windows/IItemsWindow.h"
#include "Windows/IItemsWindowManager.h"
#include "Windows/ILightsWindow.h"
#include "Windows/ILightsWindowManager.h"
#include "Windows/ILogWindow.h"
#include "Windows/ILogWindowManager.h"
#include "Windows/IRoomsWindow.h"
#include "Windows/IRoomsWindowManager.h"
#include "Windows/IRouteWindow.h"
#include "Windows/IRouteWindowManager.h"
#include "Windows/ITriggersWindow.h"
#include "Windows/ITriggersWindowManager.h"
#include "Windows/IViewer.h"
#include "Windows/ITexturesWindowManager.h"
#include "Windows/ITexturesWindow.h"
#include "Windows/ICameraSinkWindow.h"
#include "Windows/ICameraSinkWindowManager.h"
#include "Windows/IConsoleManager.h"
#include "Windows/IConsole.h"
#include "IApplication.h"
#include "Lua/ILua.h"
#include "Plugins/IPlugins.h"
#include "Plugins/IPlugin.h"
#include "Windows/IPluginsWindow.h"
#include "Windows/IPluginsWindowManager.h"
#include "Tools/IToolbar.h"
#include "Windows/IStaticsWindow.h"
#include "Windows/IStaticsWindowManager.h"
#include "Windows/IWindows.h"
#include "Windows/ISoundsWindow.h"
#include "Windows/ISoundsWindowManager.h"
#include "Windows/IAboutWindow.h"
#include "Windows/IAboutWindowManager.h"
#include "Windows/IDiffWindowManager.h"
#include "Windows/IDiffWindow.h"
#include "Windows/IPackWindowManager.h"
#include "Windows/IPackWindow.h"

namespace trview
{
    namespace mocks
    {
        MockCamera::MockCamera() {}
        MockCamera::~MockCamera() {}

        MockItem::MockItem() {}
        MockItem::~MockItem() {}

        MockLevel::MockLevel() {}
        MockLevel::~MockLevel() {}

        MockLight::MockLight() {}
        MockLight::~MockLight() {}

        MockRoom::MockRoom() {}
        MockRoom::~MockRoom() {}

        MockSector::MockSector() {}
        MockSector::~MockSector() {}

        MockStaticMesh::MockStaticMesh() {}
        MockStaticMesh::~MockStaticMesh() {}

        MockTrigger::MockTrigger() {}
        MockTrigger::~MockTrigger() {}

        MockTypeInfoLookup::MockTypeInfoLookup() {}
        MockTypeInfoLookup::~MockTypeInfoLookup() {}

        MockMesh::MockMesh() {}
        MockMesh::~MockMesh() {}

        MockPicking::MockPicking() {}
        MockPicking::~MockPicking() {}

        MockTransparencyBuffer::MockTransparencyBuffer() {}
        MockTransparencyBuffer::~MockTransparencyBuffer() {}

        MockLevelTextureStorage::MockLevelTextureStorage() {}
        MockLevelTextureStorage::~MockLevelTextureStorage() {}

        MockMeshStorage::MockMeshStorage() {}
        MockMeshStorage::~MockMeshStorage() {}

        MockSectorHighlight::MockSectorHighlight() {}
        MockSectorHighlight::~MockSectorHighlight() {}

        MockSelectionRenderer::MockSelectionRenderer() {}
        MockSelectionRenderer::~MockSelectionRenderer() {}

        MockTextureStorage::MockTextureStorage() {}
        MockTextureStorage::~MockTextureStorage() {}

        MockFileMenu::MockFileMenu() {}
        MockFileMenu::~MockFileMenu() {}

        MockUpdateChecker::MockUpdateChecker() {}
        MockUpdateChecker::~MockUpdateChecker() {}

        MockRoute::MockRoute() {}
        MockRoute::~MockRoute() {}

        MockRandomizerRoute::MockRandomizerRoute() {}
        MockRandomizerRoute::~MockRandomizerRoute() {}

        MockWaypoint::MockWaypoint() {}
        MockWaypoint::~MockWaypoint() {}

        MockSettingsLoader::MockSettingsLoader() {}
        MockSettingsLoader::~MockSettingsLoader() {}

        MockStartupOptions::MockStartupOptions() {}
        MockStartupOptions::~MockStartupOptions() {}

        MockCompass::MockCompass() {}
        MockCompass::~MockCompass() {}

        MockMeasure::MockMeasure() {}
        MockMeasure::~MockMeasure() {}

        MockCameraControls::MockCameraControls() {}
        MockCameraControls::~MockCameraControls() {}

        MockContextMenu::MockContextMenu() {}
        MockContextMenu::~MockContextMenu() {}

        MockFonts::MockFonts() {}
        MockFonts::~MockFonts() {}

        MockImGuiBackend::MockImGuiBackend() {}
        MockImGuiBackend::~MockImGuiBackend() {}

        MockMapRenderer::MockMapRenderer() {}
        MockMapRenderer::~MockMapRenderer() {}

        MockSettingsWindow::MockSettingsWindow() {}
        MockSettingsWindow::~MockSettingsWindow() {}

        MockViewerUI::MockViewerUI() {}
        MockViewerUI::~MockViewerUI() {}

        MockViewOptions::MockViewOptions() {}
        MockViewOptions::~MockViewOptions() {}

        MockItemsWindow::MockItemsWindow() {}
        MockItemsWindow::~MockItemsWindow() {}

        MockItemsWindowManager::MockItemsWindowManager() {}
        MockItemsWindowManager::~MockItemsWindowManager() {}

        MockLightsWindow::MockLightsWindow() {}
        MockLightsWindow::~MockLightsWindow() {}

        MockLightsWindowManager::MockLightsWindowManager() {}
        MockLightsWindowManager::~MockLightsWindowManager() {}

        MockLogWindow::MockLogWindow() {}
        MockLogWindow::~MockLogWindow() {}

        MockLogWindowManager::MockLogWindowManager() {}
        MockLogWindowManager::~MockLogWindowManager() {}

        MockPackWindow::MockPackWindow() {}
        MockPackWindow::~MockPackWindow() {}

        MockPackWindowManager::MockPackWindowManager() {}
        MockPackWindowManager::~MockPackWindowManager() {}

        MockRoomsWindow::MockRoomsWindow() {}
        MockRoomsWindow::~MockRoomsWindow() {}

        MockRoomsWindowManager::MockRoomsWindowManager() {}
        MockRoomsWindowManager::~MockRoomsWindowManager() {}

        MockRouteWindow::MockRouteWindow() {}
        MockRouteWindow::~MockRouteWindow() {}

        MockRouteWindowManager::MockRouteWindowManager() {}
        MockRouteWindowManager::~MockRouteWindowManager() {}

        MockTriggersWindow::MockTriggersWindow() {}
        MockTriggersWindow::~MockTriggersWindow() {}

        MockTriggersWindowManager::MockTriggersWindowManager() {}
        MockTriggersWindowManager::~MockTriggersWindowManager() {}

        MockViewer::MockViewer() {}
        MockViewer::~MockViewer() {}

        MockTexturesWindowManager::MockTexturesWindowManager() {}
        MockTexturesWindowManager::~MockTexturesWindowManager() {}

        MockTexturesWindow::MockTexturesWindow() {}
        MockTexturesWindow::~MockTexturesWindow() {}

        MockCameraSinkWindowManager::MockCameraSinkWindowManager() {}
        MockCameraSinkWindowManager::~MockCameraSinkWindowManager() {}

        MockCameraSinkWindow::MockCameraSinkWindow() {}
        MockCameraSinkWindow::~MockCameraSinkWindow() {}

        MockConsoleManager::MockConsoleManager() {}
        MockConsoleManager::~MockConsoleManager() {}

        MockConsole::MockConsole() {}
        MockConsole::~MockConsole() {}

        MockApplication::MockApplication() {}
        MockApplication::~MockApplication() {}

        MockLua::MockLua() {}
        MockLua::~MockLua() {}

        MockPlugins::MockPlugins() {}
        MockPlugins::~MockPlugins() {}

        MockPlugin::MockPlugin() {}
        MockPlugin::~MockPlugin() {}

        MockPluginsWindow::MockPluginsWindow() {}
        MockPluginsWindow::~MockPluginsWindow() {}

        MockPluginsWindowManager::MockPluginsWindowManager() {}
        MockPluginsWindowManager::~MockPluginsWindowManager() {}

        MockStaticsWindow::MockStaticsWindow() {}
        MockStaticsWindow::~MockStaticsWindow() {}

        MockStaticsWindowManager::MockStaticsWindowManager() {}
        MockStaticsWindowManager::~MockStaticsWindowManager() {}

        MockToolbar::MockToolbar() {}
        MockToolbar::~MockToolbar() {}

        MockWindows::MockWindows() {}
        MockWindows::~MockWindows() {}

        MockScriptable::MockScriptable() {}
        MockScriptable::~MockScriptable() {}

        MockSoundsWindow::MockSoundsWindow() {}
        MockSoundsWindow::~MockSoundsWindow() {}

        MockSoundsWindowManager::MockSoundsWindowManager() {}
        MockSoundsWindowManager::~MockSoundsWindowManager() {}

        MockSoundSource::MockSoundSource() {};
        MockSoundSource::~MockSoundSource() {};

        MockSoundStorage::MockSoundStorage() {};
        MockSoundStorage::~MockSoundStorage() {};

        MockSound::MockSound() {};
        MockSound::~MockSound() {};

        MockNgPlusSwitcher::MockNgPlusSwitcher() {};
        MockNgPlusSwitcher::~MockNgPlusSwitcher() {};

        MockAboutWindowManager::MockAboutWindowManager() {};
        MockAboutWindowManager::~MockAboutWindowManager() {};

        MockAboutWindow::MockAboutWindow() {};
        MockAboutWindow::~MockAboutWindow() {};

        MockDiffWindow::MockDiffWindow() {};
        MockDiffWindow::~MockDiffWindow() {};

        MockDiffWindowManager::MockDiffWindowManager() {};
        MockDiffWindowManager::~MockDiffWindowManager() {};

        MockModelStorage::MockModelStorage() {};
        MockModelStorage::~MockModelStorage() {};
    }
}
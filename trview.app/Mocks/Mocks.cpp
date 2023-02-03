#include "pch.h"
#include <gmock/gmock.h>

#include "Camera/ICamera.h"
#include "Elements/IItem.h"
#include "Elements/ILevel.h"
#include "Elements/ILight.h"
#include "Elements/IRoom.h"
#include "Elements/ISector.h"
#include "Elements/IStaticMesh.h"
#include "Elements/ITrigger.h"
#include "Elements/ITypeNameLookup.h"
#include "Geometry/IMesh.h"
#include "Geometry/IPicking.h"
#include "Geometry/ITransparencyBuffer.h"
#include "Graphics/ILevelTextureStorage.h"
#include "Graphics/IMeshStorage.h"
#include "Graphics/ISectorHighlight.h"
#include "Graphics/ISelectionRenderer.h"
#include "Graphics/ITextureStorage.h"
#include "Menus/IFileMenu.h"
#include "Menus/IUpdateChecker.h"
#include "Routing/IRoute.h"
#include "Routing/IWaypoint.h"
#include "Settings/ISettingsLoader.h"
#include "Settings/IStartupOptions.h"
#include "Tools/ICompass.h"
#include "Tools/IMeasure.h"
#include "UI/ICameraControls.h"
#include "UI/IContextMenu.h"
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

        MockTypeNameLookup::MockTypeNameLookup() {}
        MockTypeNameLookup::~MockTypeNameLookup() {}

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
    }
}
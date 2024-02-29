#pragma once

#include <windows.h>
#undef Yield

#define IMGUI_TEST_ENGINE_ENABLE_COROUTINE_STDTHREAD_IMPL 1

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_app.h>
#include <imgui_te_context.h>
#include <imgui_te_engine.h>
#include <imgui_te_internal.h>
#include <imgui_te_ui.h>

#include <trview.tests.common/Mocks.h>

#include "TestHelpers.h"

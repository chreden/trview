// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <algorithm>
#include <sstream>
#include <vector>
#include <cstdint>
#include <string>
#include <iomanip>
#include <memory>

#include <external/DirectXTK/Inc/WICTextureLoader.h>
#include <directxmath.h>
#include <commdlg.h>
#include <shellapi.h>
#include <Shlwapi.h>

#include <trview.graphics/FontFactory.h>
#include <trview.graphics/Device.h>
#include <trview.graphics/IShaderStorage.h>
#include <trview.graphics/VertexShader.h>
#include <trview.graphics/PixelShader.h>
#include <trview.app/Graphics/ITextureStorage.h>
#include <trview.common/Strings.h>
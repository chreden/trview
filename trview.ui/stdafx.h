#pragma once

#define NOMINMAX
#include <algorithm>
#include <iterator>
#include <memory>
#include <optional>
#include <queue>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <Windows.h>

#include <external/nlohmann/json.hpp>
#include <external/boost/di.hpp>

#include <trview.common/Colour.h>
#include <trview.common/Event.h>
#include <trview.common/Json.h>
#include <trview.common/JsonSerializers.h>
#include <trview.common/Point.h>
#include <trview.common/Resources.h>
#include <trview.common/Strings.h>
#include <trview.common/Size.h>
#include <trview.common/TokenStore.h>
#include <trview.common/Window.h>
#include <trview.common/Windows/IClipboard.h>
#include <trview.common/Windows/IShortcuts.h>
#include <trview.common/Windows/IShell.h>
#include <trview.graphics/ParagraphAlignment.h>
#include <trview.graphics/TextAlignment.h>
#include <trview.graphics/Texture.h>
#include <trview.input/Keyboard.h>
#include <trview.input/IMouse.h>


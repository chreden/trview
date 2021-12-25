#pragma once

#define NOMINMAX
#include <Windows.h>

#include <optional>

#include <gmock/gmock.h>
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include <trview.common/Mocks/Windows/IShell.h>
#include <trview.ui/Mocks/IFontMeasurer.h>
#include <trview.ui/Mocks/Input/IInput.h>

#include <trview.ui/Button.h>
#include <trview.ui/Checkbox.h>
#include <trview.ui/Dropdown.h>
#include <trview.ui/GroupBox.h>
#include <trview.ui/Image.h>
#include <trview.ui/JsonLoader.h>
#include <trview.ui/Label.h>
#include <trview.ui/Listbox.h>
#include <trview.ui/Layouts/GridLayout.h>
#include <trview.ui/Layouts/StackLayout.h>
#include <trview.ui/NumericUpDown.h>
#include <trview.ui/Scrollbar.h>
#include <trview.ui/Slider.h>
#include <trview.ui/TextArea.h>

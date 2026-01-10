#include "stdafx.h"

#include <gmock/gmock.h>

#include "Logs/ILog.h"
#include "Windows/IClipboard.h"
#include "Windows/IDialogs.h"
#include "Windows/IShell.h"
#include "Windows/IShortcuts.h"
#include "IFiles.h"
#include "Messages/IMessageSystem.h"
#include "Messages/IRecipient.h"

namespace trview
{
    namespace mocks
    {
        MockLog::MockLog() {}
        MockLog::~MockLog() {}

        MockClipboard::MockClipboard() {}
        MockClipboard::~MockClipboard() {}

        MockDialogs::MockDialogs() {}
        MockDialogs::~MockDialogs() {}

        MockShell::MockShell() {}
        MockShell::~MockShell() {}

        MockShortcuts::MockShortcuts() {}
        MockShortcuts::~MockShortcuts() {}

        MockFiles::MockFiles() {}
        MockFiles::~MockFiles() {}

        MockMessageSystem::MockMessageSystem() {}
        MockMessageSystem::~MockMessageSystem() {}

        MockRecipient::MockRecipient() {}
        MockRecipient::~MockRecipient() {}
    }
}
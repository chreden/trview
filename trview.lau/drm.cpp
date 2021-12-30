#include "drm.h"
#include <fstream>
#include <unordered_set>

namespace trview
{
    namespace lau
    {
        std::wstring section_type_to_string(const SectionType& type)
        {
            switch (type)
            {
            case SectionType::Audio:
                return L"Audio";
            case SectionType::Section:
                return L"Section";
            case SectionType::Texture:
                return L"Texture";
            }
            return L"Unknown";
        }
    }
}

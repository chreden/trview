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
            case SectionType::Section:
                return L"Section";
            case SectionType::Empty:
                return L"Empty";
            case SectionType::Animation:
                return L"Animation";
            case SectionType::PushBuffer_Wc:
                return L"PushBuffer WC";
            case SectionType::PushBuffer:
                return L"PushBuffer";
            case SectionType::Texture:
                return L"Texture";
            case SectionType::Audio:
                return L"Audio";
            case SectionType::DtpData:
                return L"DTP Data";
            case SectionType::Script:
                return L"Script";
            case SectionType::ShaderLib:
                return L"Shader Library";
            case SectionType::CollisionMesh:
                return L"Collision Mesh";
            }
            return L"Unknown";
        }
    }
}

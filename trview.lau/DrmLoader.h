#pragma once

#include <memory>
#include <trview.common/IFiles.h>

#include "IDrmLoader.h"

namespace trview
{
    namespace lau
    {
        class DrmLoader final : public IDrmLoader
        {
        public:
            explicit DrmLoader(const std::shared_ptr<IFiles>& files);
            virtual ~DrmLoader() = default;
            virtual std::unique_ptr<Drm> load(const std::wstring filename) const;
        private:
            void read_file_header(Drm& drm) const;
            void generate_textures(Drm& drm) const;
            void load_vertex_data(Drm& drm, const Section& section) const;
            void load_world_vertices(Drm& drm, const Section& section) const;
            void load_world_mesh(Drm& drm, const Section& section) const;
            void resolve_world_mesh_textures(Drm& drm, const Section& section) const;
            void process_relocation(Drm& drm) const;

            std::shared_ptr<IFiles> _files;
        };
    }
}
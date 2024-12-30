#include "DiffWindow.h"
#include <format>
#include <trlevel/LevelEncryptedException.h>
#include "../../UserCancelledException.h"

namespace trview
{
    IDiffWindow::~IDiffWindow()
    {
    }

    DiffWindow::DiffWindow(const std::shared_ptr<IDialogs>& dialogs, const ILevel::Source& level_source)
        : _dialogs(dialogs), _level_source(level_source)
    {
    }

    void DiffWindow::render()
    {
        if (!render_diff_window())
        {
            on_window_closed();
            return;
        }
    }

    bool DiffWindow::render_diff_window()
    {
        bool stay_open = true;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(520, 400));
        if (ImGui::Begin(_id.c_str(), &stay_open, ImGuiWindowFlags_MenuBar))
        {
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("Open"))
                    {
                        const auto filename = _dialogs->open_file(L"Open level", { { L"All Tomb Raider Files", { L"*.tr2", L"*.tr4", L"*.trc", L"*.phd", L"*.psx" } } }, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST);
                        if (filename.has_value())
                        {
                            start_load(filename->filename);
                        }
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            ImGui::Text(std::format("Load Progress: {}", _progress).c_str());
        }
        ImGui::End();
        ImGui::PopStyleVar();
        return stay_open;
    }

    void DiffWindow::set_number(int32_t number)
    {
        _id = std::format("Diff {}", number);
    }

    void DiffWindow::start_load(const std::string& filename)
    {
        _load = std::async(std::launch::async, [=]() -> LoadOperation
            {
                LoadOperation operation
                {
                    .filename = filename,
                };

                try
                {
                    operation.level = load_level(filename);
                }
                catch (trlevel::LevelEncryptedException&)
                {
                    operation.error = "Level is encrypted and cannot be loaded";
                }
                catch (UserCancelledException&)
                {
                }
                catch (std::exception& e)
                {
                    operation.error = std::format("Failed to load level : {}", e.what());
                }

                return operation;
            });
    }

    std::shared_ptr<ILevel> DiffWindow::load_level(const std::string& filename)
    {
        _progress = std::format("Loading {}", filename);
        auto level = _level_source(filename,
            {
                .on_progress_callback = [&](auto&& p) { _progress = p; }
            });
        level->set_filename(filename);
        return level;
    }
}

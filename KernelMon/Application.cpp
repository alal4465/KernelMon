#include "Application.h"
#include <iostream>
#include <intrin.h>

Gui::Application::Application() : driver_conn_(DEVICE_SYM_NAME)
{
    monitored_drivers_.reserve(MAX_MONITORED_DRIVERS);
    driver_conn_.reset_state();
}

void Gui::Application::render_tick() {
    show_log_window();
    show_drivers_window();
}

void Gui::Application::show_drivers_window() {
    ImGui::Begin("Driver Settings");

    ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp;
    if (ImGui::BeginTable("Monitored Drivers", 1, flags, ImVec2(0, 0)))
    {
        ImGui::TableSetupColumn("driver names");
        ImGui::TableHeadersRow();

        for (const auto& driver : monitored_drivers_) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text(driver.c_str());
        }

        ImGui::EndTable();
    }

    static char buf1[64] = "";
    ImGui::InputText("", buf1, 64);

    if (ImGui::Button("add driver") && buf1[0] != '\0' && monitored_drivers_.size() < MAX_MONITORED_DRIVERS) {
        auto driver_name = utf8ToUtf16(buf1);
        driver_conn_.add_driver(driver_name);

        monitored_drivers_.push_back(buf1);
        buf1[0] = '\0';
    }

    else if (ImGui::Button("remove driver") && buf1[0] != '\0' && !monitored_drivers_.empty()) {
        auto driver_name = utf8ToUtf16(buf1);
        driver_conn_.remove_driver(driver_name);

        monitored_drivers_.erase(std::remove(monitored_drivers_.begin(), monitored_drivers_.end(), buf1), monitored_drivers_.end());
        buf1[0] = '\0';
    }

    ImGui::End();
}

void Gui::Application::show_log_window() {
    ImGui::Begin("Logs");

    ImGuiTableFlags flags = ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp;
    if (ImGui::BeginTable("table_scrollx", 5, flags, ImVec2(0, 0)))
    {
        ImGui::TableSetupColumn("Driver");
        ImGui::TableSetupColumn("Function");
        ImGui::TableSetupColumn("Path");
        ImGui::TableSetupColumn("Details");
        ImGui::TableSetupColumn("Result");
        ImGui::TableHeadersRow();
        for (const auto& log : this->logs_) {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::Text(log.driver.data());

            ImGui::TableSetColumnIndex(1);
            ImGui::Text(monitored_functions_map.at(log.function).data());

            ImGui::TableSetColumnIndex(2);
            ImGui::Text(log.path.data());

            ImGui::TableSetColumnIndex(3);
            ImGui::Text(log.details.data());

            ImGui::TableSetColumnIndex(4);
            auto result_text = std::to_string(log.result);
            ImGui::Text(result_text.data());
        }
        ImGui::EndTable();

        ImGui::End();
    }
}

void Gui::Application::add_log(AppLog&& log) {
    logs_.push_back(std::move(log));
}
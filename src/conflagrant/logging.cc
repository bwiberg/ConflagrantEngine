#include "logging.hh"

namespace cfl {
std::vector<Log::Entry> Log::Entries;
ImVec4 Log::InfoColor{.8f, 1.f, .8f, 1.f};
ImVec4 Log::ErrorColor{1.f, .5f, .5f, 1.f};

void Log::Append(Entry::Type type, std::string &&identifier, std::string &&message) {
    Entries.emplace_back(type, std::move(identifier), std::move(message));
}

void Log::DrawImGuiWindow() {
    ImGui::Begin("Log");

    for (auto const &e : Entries) {
        ImVec4 const &color = (e.type == Entry::Type::ERROR) ? ErrorColor : InfoColor;
        ImGui::TextColored(color, "%s: %s", e.identifier.c_str(), e.message.c_str());
    }

    ImGui::End();
}
} // namespace cfl

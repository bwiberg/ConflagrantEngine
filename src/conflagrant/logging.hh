#pragma once

#include <iostream>
#include <sstream>
#include <vector>
#include <imgui.h>

#ifndef LOG_ERROR
#define LOG_ERROR(identifier) (LogEntry(cfl::Log::Entry::Type::ERROR, #identifier).ss)
#endif // LOG_ERROR

#ifndef LOG_INFO
#define LOG_INFO(identifier) (LogEntry(cfl::Log::Entry::Type::INFO, #identifier).ss)
#endif // LOG_INFO

namespace cfl {
struct Log {
    struct Entry {
        enum Type {
            ERROR,
            INFO
        };

        Type type;
        std::string const identifier, message;

        Entry(Type type, std::string &&identifier, std::string &&message);
    };

    static std::vector<Entry> Entries;

    static ImVec4 InfoColor, ErrorColor;

    static void Append(Entry::Type type, std::string &&identifier, std::string &&message);

    static void DrawImGuiWindow();
};

struct LogEntry {
    std::stringstream ss;
    std::string identifier;
    Log::Entry::Type type;

    inline LogEntry(Log::Entry::Type type, std::string &&identifier)
            : identifier(identifier), type(type) {}

    inline ~LogEntry() {
        if (type == Log::Entry::Type::ERROR) {
            std::cerr << identifier << ": " << ss.str() << std::endl;
        } else {
            std::cout << identifier << ": " << ss.str() << std::endl;
        }

        Log::Append(type, std::move(identifier), std::move(ss.str()));
    }
};

inline Log::Entry::Entry(Log::Entry::Type type, std::string &&identifier, std::string &&message)
        : type(type), identifier(identifier), message(message) {}
}

#include "gherkin-catch-module.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>
#include <locale>
#include <codecvt>  // нужно для конвертации wchar_t <-> UTF-8

namespace gherkin_bridge {

    GherkinValidator& GherkinValidator::instance() {
        static GherkinValidator validator;
        return validator;
    }

    void GherkinValidator::init(const std::string& path) {
        loadFeatures(path);
    }

    void GherkinValidator::printReport() {
        std::cout << "\n=== Gherkin Coverage Report ===\n";
        std::cout << "Total steps: " << steps_.size() << "\n";
        std::cout << "Used steps:  " << used_count_ << "\n";
        std::cout << "Coverage:    " << (used_count_ * 100.0 / steps_.size()) << "%\n";

        if (used_count_ < steps_.size()) {
            std::cout << "\nUnused steps:\n";
            for (const auto& [step, used] : steps_) {
                if (!used) std::cout << step << "\n";
            }
        }
        std::cout << "==============================\n";
    }

    bool GherkinValidator::allStepsUsed() const {
        return used_count_ == steps_.size();
    }

    // === Private ===

    void GherkinValidator::loadFeatures(const std::string& path) {
        namespace fs = std::filesystem;
        for (const auto& entry : fs::recursive_directory_iterator(path)) {
            if (entry.path().extension() == ".feature") {
                parseFeatureFile(entry.path().string());
            }
        }
    }

    std::wstring readFileUtf8ToWstring(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Cannot open file: " << filename << std::endl;
            return {};
        }
        std::ostringstream ss;
        ss << file.rdbuf();
        std::string content = ss.str();

        // Конвертируем UTF-8 -> std::wstring
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        return converter.from_bytes(content);
    }

    void GherkinValidator::parseFeatureFile(const std::string& filename) {
        std::wstring wcontent = readFileUtf8ToWstring(filename);
        if (wcontent.empty()) return;

        gherkin::v1::parser parser;
        auto doc = parser.parse(wcontent); // теперь передаем std::wstring
        if (!doc) return;

        const GherkinDocument* document = doc.get();
        if (!document || !document->feature) return;

        const Feature* feature = document->feature;
        if (!feature->scenario_definitions) return;

        const ScenarioDefinitions* defs = feature->scenario_definitions;

        for (int i = 0; i < defs->scenario_definition_count; ++i) {
            const ScenarioDefinition* def = defs->scenario_definitions[i];

            switch (def->type) {
            case Gherkin_Background: {
                const Background* bg = (const Background*)def;
                processSteps(bg->steps);
                break;
            }
            case Gherkin_Scenario: {
                const Scenario* sc = (const Scenario*)def;
                processSteps(sc->steps);
                break;
            }
            case Gherkin_ScenarioOutline: {
                const Scenario* so = (const Scenario*)def;
                processSteps(so->steps);
                break;
            }
            default: break;
            }
        }
    }

    void GherkinValidator::processSteps(const Steps* steps) {
        if (!steps) return;
        for (int i = 0; i < steps->step_count; ++i)
            addStep(&steps->steps[i]);
    }

    std::string trim(const std::string& s) {
        size_t start = s.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = s.find_last_not_of(" \t\r\n");
        return s.substr(start, end - start + 1);
    }

    void GherkinValidator::addStep(const Step* step) {
        if (!step) return;
        std::wstring keyword_w = step->keyword ? step->keyword : L"";
        std::wstring text_w = step->text ? step->text : L"";

        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::string keyword = trim(converter.to_bytes(keyword_w));
        std::string text = trim(converter.to_bytes(text_w));

        steps_.insert({ keyword + " " + text, false });
    }

    bool GherkinValidator::validateStep(const std::string& keyword, const std::string& text) {
        std::string key = trim(keyword) + " " + trim(text);
        auto it = steps_.find(key);
        if (it != steps_.end() && !it->second) {
            it->second = true;
            used_count_++;
            return true;
        }
        return false;
    }

} // namespace gherkin_bridge
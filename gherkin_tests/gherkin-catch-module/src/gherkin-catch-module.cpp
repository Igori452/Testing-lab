#include "gherkin-catch-module.hpp"
#include "gherkin_macros.hpp"

#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>
#include <locale>
#include <codecvt>
#include <regex>

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

    void GherkinValidator::loadFeatures(const std::string& path) {
        parseFeatureFile(path);
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

        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        return converter.from_bytes(content);
    }

    // Функция для удаления строк с Rule: (работает с UTF-8 string)
    std::wstring removeRuleLines(const std::wstring& content) {
        std::wstring result;
        std::wistringstream stream(content);
        std::wstring line;
        
        while (std::getline(stream, line)) {
            size_t firstChar = line.find_first_not_of(L" \t");
            if (firstChar != std::wstring::npos) {
                if (line.substr(firstChar, 5) == L"Rule:") {
                    continue;  // пропускаем строки с Rule:
                }
            }
            result += line + L"\n";
        }
        
        return result;
    }

    void GherkinValidator::parseFeatureFile(const std::string& filename) {
        try {
            std::wstring wcontent = readFileUtf8ToWstring(filename);
            if (wcontent.empty()) return;

            wcontent = removeRuleLines(wcontent);

            gherkin::v1::parser parser;
            auto doc = parser.parse(wcontent);
            if (!doc) return;

            const GherkinDocument* document = doc.get();
            if (!document || !document->feature) return;

            const Feature* feature = document->feature;
            if (!feature->scenario_definitions) return;

            const ScenarioDefinitions* defs = feature->scenario_definitions;

            for (int i = 0; i < defs->scenario_definition_count; ++i) {
                const ScenarioDefinition* def = defs->scenario_definitions[i];
                
                if (!def) continue;

                // Пробуем обработать каждый элемент
                switch (def->type) {
                case Gherkin_Background: {
                    const Background* bg = (const Background*)def;
                    if (bg->steps) processSteps(bg->steps);
                    break;
                }
                case Gherkin_Scenario: {
                    const Scenario* sc = (const Scenario*)def;
                    if (sc->steps) processSteps(sc->steps);
                    break;
                }
                case Gherkin_ScenarioOutline: {
                    const Scenario* so = (const Scenario*)def;
                    if (so->steps) processSteps(so->steps);
                    break;
                }
                default:
                    break;
                }
            }
        }
        catch (const std::exception& e) {
            // Игнорируем ошибки
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

        // Удаляем комментарии из текста (всё от # до конца строки)
        size_t commentPos = text.find(" #");
        if (commentPos != std::string::npos) {
            text = text.substr(0, commentPos);
        }
        
        text = trim(text);

        const std::string stepText = keyword + " " + text;
        steps_.insert({ stepText, false });
    }

    bool GherkinValidator::validateStep(const std::string& keyword, const std::string& text) {
        std::string key = keyword + " " + text;

        // 1. Точное совпадение
        auto it = steps_.find(key);
        if (it != steps_.end()) {
            if (!it->second) {
                it->second = true;
                used_count_++;
            }
            return true;
        }

        // 2. УНИВЕРСАЛЬНЫЙ шаблон: любой текст в < > заменяем на (.*)
        for (auto& [step, used] : steps_) {
            if (step.find(keyword) != 0) continue;

            std::string stepText = step.substr(keyword.length() + 1);
            std::string userText = text;

            // Проверяем, есть ли в шаблоне <...>
            if (stepText.find('<') != std::string::npos && stepText.find('>') != std::string::npos) {

                // Создаем regex: все что в < > заменяем на (.*)
                std::string pattern = stepText;

                // Экранируем спецсимволы
                pattern = std::regex_replace(pattern, std::regex("\\."), "\\.");
                pattern = std::regex_replace(pattern, std::regex("\\?"), "\\?");
                pattern = std::regex_replace(pattern, std::regex("\\*"), "\\*");
                pattern = std::regex_replace(pattern, std::regex("\\+"), "\\+");
                pattern = std::regex_replace(pattern, std::regex("\\["), "\\[");
                pattern = std::regex_replace(pattern, std::regex("\\]"), "\\]");
                pattern = std::regex_replace(pattern, std::regex("\\{"), "\\{");
                pattern = std::regex_replace(pattern, std::regex("\\}"), "\\}");
                pattern = std::regex_replace(pattern, std::regex("\\("), "\\(");
                pattern = std::regex_replace(pattern, std::regex("\\)"), "\\)");
                pattern = std::regex_replace(pattern, std::regex("\\|"), "\\|");

                // Заменяем <...> на (.*) — любые символы
                pattern = std::regex_replace(pattern, std::regex("<[^>]+>"), "(.*)");

                std::regex stepRegex("^" + pattern + "$");
                std::smatch matches;

                if (std::regex_match(userText, matches, stepRegex)) {
                    if (!used) {
                        steps_[step] = true;
                        used_count_++;

                        // Опционально: выводим захваченные значения
                        for (size_t i = 1; i < matches.size(); ++i) {
                            std::cout << "     captured: '" << matches[i].str() << "'\n";
                        }
                    }
                    return true;
                }
            }
        }

        return false;
    }

    GherkinValidator::~GherkinValidator() {
        // Выводим отчёт в самом конце, после всех тестов
        std::cout << "\n";
        std::cout << "╔════════════════════════════════════════╗\n";
        std::cout << "║     GLOBAL BDD COVERAGE REPORT         ║\n";
        std::cout << "╚════════════════════════════════════════╝\n";
        printReport();
    }

} // namespace gherkin_bridge
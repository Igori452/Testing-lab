#pragma once

#include <gherkin.hpp>

extern "C" {
    #include <gherkin_document.h>
    #include <feature.h>
    #include <background.h>
    #include <scenario.h>
    #include <scenario_outline.h>
    #include <step.h>
}

#include <catch2/catch_test_macros.hpp>
#include <string>
#include <map>

namespace gherkin_bridge {

    class GherkinValidator {

        private:
            GherkinValidator() = default;

            // Вспомогательные функции
            void loadFeatures(const std::string& path);
            void parseFeatureFile(const std::string& filename);
            void processSteps(const Steps* steps);
            void addStep(const Step* step);

            // Хранилище шагов
            std::map<std::string, bool> steps_;
            int used_count_ = 0;

        public:
            // Синглтон
            static GherkinValidator& instance();

            // Инициализация
            void init(const std::string& path);

            // Валидация шага
            bool validateStep(const std::string& keyword, const std::string& text);

            // Отчёт
            void printReport();

            // Проверка полного покрытия
            bool allStepsUsed() const;
    };
} // namespace gherkin_bridge

// ==================== МАКРОСЫ ====================
#define INIT_GHERKIN(path) \
    static struct GherkinInit { \
        GherkinInit() { \
            ::gherkin_bridge::GherkinValidator::instance().init(path); \
        } \
    } GHERKIN_INIT_##__LINE__

#define GHERKIN_GIVEN(text) \
    do { \
        if (!::gherkin_bridge::GherkinValidator::instance().validateStep("Given", text)) { \
            FAIL("Step not defined: Given " + std::string(text)); \
        } \
    } while(0)

#define GHERKIN_WHEN(text) \
    do { \
        if (!::gherkin_bridge::GherkinValidator::instance().validateStep("When", text)) { \
            FAIL("Step not defined: When " + std::string(text)); \
        } \
    } while(0)

#define GHERKIN_THEN(text) \
    do { \
        if (!::gherkin_bridge::GherkinValidator::instance().validateStep("Then", text)) { \
            FAIL("Step not defined: Then " + std::string(text)); \
        } \
    } while(0)

#define GHERKIN_AND(text) \
    do { \
        if (!::gherkin_bridge::GherkinValidator::instance().validateStep("And", text)) { \
            FAIL("Step not defined: And " + std::string(text)); \
        } \
    } while(0)

#define GHERKIN_REPORT() \
    ::gherkin_bridge::GherkinValidator::instance().printReport()

#define REQUIRE_GHERKIN_COVERAGE() \
    do { \
        ::gherkin_bridge::GherkinValidator::instance().printReport(); \
        REQUIRE(::gherkin_bridge::GherkinValidator::instance().allStepsUsed()); \
    } while(0)
// обязательно нужна новая строчка
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

            void loadFeatures(const std::string& path);
            void parseFeatureFile(const std::string& filename);
            void processSteps(const Steps* steps);
            void addStep(const Step* step);

            std::map<std::string, bool> steps_;
            int used_count_ = 0;

        public:
            static GherkinValidator& instance();

            void init(const std::string& path);

            bool validateStep(const std::string& keyword, const std::string& text);

            void printReport();

            bool allStepsUsed() const;

            ~GherkinValidator();
    };
} // namespace gherkin_bridge

#define GHERKIN_GIVEN(text) \
    do { \
        ::gherkin_bridge::GherkinValidator::instance().validateStep("Given", text); \
    } while(0)

#define GHERKIN_WHEN(text) \
    do { \
        ::gherkin_bridge::GherkinValidator::instance().validateStep("When", text); \
    } while(0)

#define GHERKIN_THEN(text) \
    do { \
        ::gherkin_bridge::GherkinValidator::instance().validateStep("Then", text); \
    } while(0)

#define GHERKIN_AND(text) \
    do { \
        ::gherkin_bridge::GherkinValidator::instance().validateStep("And", text); \
    } while(0)

#define GHERKIN_BUT(text) \
    do { \
        ::gherkin_bridge::GherkinValidator::instance().validateStep("But", text); \
    } while(0)
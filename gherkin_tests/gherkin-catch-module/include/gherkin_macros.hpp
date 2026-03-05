#pragma once
#include <map>
#include <functional>
#include <string>
#include <vector>
#include <fstream>
#include "gherkin-catch-module.hpp"

class StepRegistry {
public:
    using StepFn = std::function<void()>;

    static StepRegistry& instance() {
        static StepRegistry inst;
        return inst;
    }

    void add(const std::string& keyword,
             const std::string& text,
             StepFn fn)
    {
        steps[keyword + ":" + text] = fn;
        pending_steps.push_back({keyword, text});
    }

    StepFn get(const std::string& keyword,
               const std::string& text)
    {
        auto key = keyword + ":" + text;
        auto it = steps.find(key);
        if(it == steps.end())
            throw std::runtime_error("Step not registered: " + key);
        return it->second;
    }
    
    void markAllSteps() {
        for (const auto& [keyword, text] : pending_steps) {
            if (keyword == "Given") GHERKIN_GIVEN(text.c_str());
            else if (keyword == "When") GHERKIN_WHEN(text.c_str());
            else if (keyword == "Then") GHERKIN_THEN(text.c_str());
            else if (keyword == "And") GHERKIN_AND(text.c_str());
            else if (keyword == "But") GHERKIN_BUT(text.c_str());
        }
        pending_steps.clear();
    }
    
    // Автоматическая инициализация при первом вызове
    static void ensureInitialized() {
        static bool initialized = false;  // Локальная статическая переменная
        if (!initialized) {
            const char* featureFile = std::getenv("GHERKIN_FEATURE_FILE");
            if (!featureFile) {
                throw std::runtime_error(
                    "ERROR: GHERKIN_FEATURE_FILE environment variable not set!\n"
                    "Usage: export GHERKIN_FEATURE_FILE=jarvis_hull.feature"
                );
            }

            const char* featuresPath = std::getenv("GHERKIN_FEATURES_PATH");
            if (!featuresPath) featuresPath = "features";

            std::string fullPath = std::string(featuresPath) + "/" + featureFile;

            std::ifstream file(fullPath);
            if (!file.good()) {
                std::string errorMsg = "ERROR: Feature file not found: " + fullPath;
                throw std::runtime_error(errorMsg);
            }

            gherkin_bridge::GherkinValidator::instance().init(fullPath);
            instance().markAllSteps();
            initialized = true;
        }
    }

private:
    std::map<std::string, StepFn> steps;
    std::vector<std::pair<std::string, std::string>> pending_steps;
};

#define BDD_JOIN(a,b) a##b
#define BDD_JOIN2(a,b) BDD_JOIN(a,b)
#define BDD_REG_VAR(line) BDD_JOIN2(_bdd_reg_,line)

#define BDD_REGISTER_STEP(keyword,text,...) \
static bool BDD_REG_VAR(__LINE__) = [](){ \
    StepRegistry::instance().add(keyword,text,__VA_ARGS__); \
    return true; \
}()

#define BDD_GIVEN(text,...) BDD_REGISTER_STEP("Given",text,__VA_ARGS__)
#define BDD_WHEN(text,...)  BDD_REGISTER_STEP("When",text,__VA_ARGS__)
#define BDD_THEN(text,...)  BDD_REGISTER_STEP("Then",text,__VA_ARGS__)
#define BDD_AND(text,...)   BDD_REGISTER_STEP("And",text,__VA_ARGS__)
#define BDD_BUT(text,...)   BDD_REGISTER_STEP("But",text,__VA_ARGS__)

// Макросы для вызова шагов (с автоматической инициализацией)
#define CALL_GIVEN(text) \
    do { \
        StepRegistry::ensureInitialized(); \
        auto fn = StepRegistry::instance().get("Given", text); \
        fn(); \
    } while(0)

#define CALL_WHEN(text) \
    do { \
        StepRegistry::ensureInitialized(); \
        auto fn = StepRegistry::instance().get("When", text); \
        fn(); \
    } while(0)

#define CALL_THEN(text) \
    do { \
        StepRegistry::ensureInitialized(); \
        auto fn = StepRegistry::instance().get("Then", text); \
        fn(); \
    } while(0)

#define CALL_AND(text) \
    do { \
        StepRegistry::ensureInitialized(); \
        auto fn = StepRegistry::instance().get("And", text); \
        fn(); \
    } while(0)

#define CALL_BUT(text) \
    do { \
        StepRegistry::ensureInitialized(); \
        auto fn = StepRegistry::instance().get("But", text); \
        fn(); \
    } while(0)
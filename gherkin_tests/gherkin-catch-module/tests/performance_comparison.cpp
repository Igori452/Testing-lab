#include <catch2/catch_test_macros.hpp>

#include "gherkin_macros.hpp"
#include "context/commonFunction.hpp"

#include <vector>
#include <chrono>
#include <fstream>
#include <iostream>
#include <cmath>
#include <random>
#include <unistd.h>

//GHERKIN_FEATURES_PATH="features" GHERKIN_FEATURE_FILE="performance_comparison.feature" ./performance_tests

using namespace jarvis_test;
using namespace graham_test;

static std::vector<Point> dataset;
static std::vector<double> graham_times;
static std::vector<double> jarvis_times;
static double last_time_ms = 0;
static double avg_time_ms = 0;
static size_t dataset_size = 0;
static std::string current_dataset_type;
static size_t peak_memory_kb = 0;
static int current_run_count = 5;
static int current_dataset_size = 0;
static std::string current_dataset_name;

struct Metrics {
    double jarvis_time = 0;
    double graham_time = 0;
    size_t jarvis_memory = 0;
    size_t graham_memory = 0;
    size_t jarvis_hull_size = 0;
    size_t graham_hull_size = 0;
};

static Metrics metrics;

size_t getCurrentMemoryUsage() {
    std::ifstream statm("/proc/self/statm");
    if (!statm.is_open()) {
        return 0;
    }
    
    long rss;
    statm >> rss;
    statm >> rss;
    
    long page_size = sysconf(_SC_PAGESIZE);
    if (page_size == -1) {
        page_size = 4096;
    }
    
    return (rss * page_size) / 1024;
}

enum class Property {
    CIRCLE,
    GRID,
};

std::vector<Point> generateDataset(size_t n, Property prop) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::vector<Point> pts;
    pts.reserve(n);
    
    switch (prop) {
        case Property::GRID: {
            int side = static_cast<int>(std::ceil(std::sqrt(n)));
            double step = 600.0 / (side - 1);

            for (int i = 0; i < side; ++i) {
                for (int j = 0; j < side; ++j) {
                    pts.push_back({ i * step, j * step });
                }
            }
            break;
        }

        case Property::CIRCLE: {
            double center = 300;
            double radius = 250;

            for (size_t i = 0; i < n; ++i) {
                double angle = 2 * M_PI * i / n;
                pts.push_back({
                    center + radius * cos(angle),
                    center + radius * sin(angle)
                    });
            }
            break;
        }
    }
    
    return pts;
}

class LoadProfiler {
private:
    struct LoadPoint {
        size_t size;
        double time_sec;
        double memory_mb;
        size_t hull_size;
    };
    
    std::vector<LoadPoint> results;
    std::string algorithm_name;
    std::string dataset_type;
    std::string cache_filename;
    
    void clearCache() {
        sync();
        std::ofstream("/proc/sys/vm/drop_caches") << "3";
    }
    
    void saveDatasetToFile(const std::vector<Point>& dataset, size_t size) {
        std::string filename = "cache_" + dataset_type + "_" + std::to_string(size) + ".bin";
        std::ofstream f(filename, std::ios::binary);
        size_t points_count = dataset.size();
        f.write(reinterpret_cast<const char*>(&points_count), sizeof(size_t));
        f.write(reinterpret_cast<const char*>(dataset.data()), points_count * sizeof(Point));
        f.close();
    }
    
    std::vector<Point> loadDatasetFromFile(size_t size) {
        std::string filename = "cache_" + dataset_type + "_" + std::to_string(size) + ".bin";
        std::vector<Point> dataset;
        
        std::ifstream f(filename, std::ios::binary);
        if (f.is_open()) {
            size_t points_count;
            f.read(reinterpret_cast<char*>(&points_count), sizeof(size_t));
            dataset.resize(points_count);
            f.read(reinterpret_cast<char*>(dataset.data()), points_count * sizeof(Point));
            f.close();
            std::cout << "Загружено из кэша: " << filename << "\n";
        }
        return dataset;
    }
    
    bool datasetExists(size_t size) {
        std::string filename = "cache_" + dataset_type + "_" + std::to_string(size) + ".bin";
        std::ifstream f(filename);
        return f.good();
    }
    
public:
    void setConfig(const std::string& algo, const std::string& data) {
        algorithm_name = algo;
        dataset_type = data;
        results.clear();
    }
    
void runLoadTest(const std::vector<size_t>& sizes, Property prop, bool useCache = true) {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "НАГРУЗОЧНОЕ ТЕСТИРОВАНИЕ: " << algorithm_name 
              << " на " << dataset_type << "\n";
    std::cout << std::string(70, '=') << "\n";
    
    size_t max_size = 0;
    if (useCache) {
        for (size_t size : sizes) {
            if (size > max_size) max_size = size;
        }
        std::cout << "Максимальный размер: " << max_size << " точек\n";
    }
    
    for (size_t size : sizes) {
        std::cout << "\nТест на " << size << " точках...\n";
        
        // Загрузка или генерация данных
        std::vector<Point> dataset;
        if (useCache && datasetExists(size)) {
            dataset = loadDatasetFromFile(size);
        } else {
            dataset = generateDataset(size, prop);
            if (useCache) {
                saveDatasetToFile(dataset, size);
                std::cout << "Сгенерировано и сохранено в кэш\n";
            }
        }
        
        // Вычисляем реальный размер данных в памяти
        size_t data_memory_kb = (dataset.size() * sizeof(Point)) / 1024;
        
        // Очищаем кэш перед каждым тестом
        clearCache();
        
        // Замер памяти до (только ОС)
        size_t mem_before = getCurrentMemoryUsage();
        
        // Замер времени
        auto start = std::chrono::high_resolution_clock::now();
        
        if (algorithm_name == "Graham") {
            GrahamResult result = runGrahamAlgorithm(dataset);
            auto end = std::chrono::high_resolution_clock::now();
        
            // Замер памяти после
            size_t mem_after = getCurrentMemoryUsage();
            
            // Память алгоритма (разница)
            size_t algo_mem_kb = mem_after > mem_before ? mem_after - mem_before : 0;
            
            // ОБЩАЯ память = данные + алгоритм
            size_t total_mem_kb = data_memory_kb + algo_mem_kb;
            
            double time_sec = std::chrono::duration<double>(end - start).count();
            
            // Сохраняем результат (total memory)
            results.push_back({size, time_sec, total_mem_kb / 1024.0, result.hull.size()});
            
            printf("Время: %.2f сек | Данные: %.2f MB | Алгоритм: %.2f MB | Всего: %.2f MB | h: %zu\n", 
                   time_sec, 
                   data_memory_kb/1024.0, 
                   algo_mem_kb/1024.0, 
                   total_mem_kb/1024.0, 
                   result.hull.size());
        } else {
            JarvisResult result = runJarvisAlgorithm(dataset);
            auto end = std::chrono::high_resolution_clock::now();
        
            size_t mem_after = getCurrentMemoryUsage();
            
            size_t algo_mem_kb = mem_after > mem_before ? mem_after - mem_before : 0;
            
            size_t total_mem_kb = data_memory_kb + algo_mem_kb;
            
            double time_sec = std::chrono::duration<double>(end - start).count();
            
            results.push_back({size, time_sec, total_mem_kb / 1024.0, result.hull.size()});
            
            printf("Время: %.2f сек | Данные: %.2f MB | Алгоритм: %.2f MB | Всего: %.2f MB | h: %zu\n", 
                   time_sec, 
                   data_memory_kb/1024.0, 
                   algo_mem_kb/1024.0, 
                   total_mem_kb/1024.0, 
                   result.hull.size());
        }
    }
}
    
    void printTable() {
        std::cout << "\n" << std::string(70, '=') << "\n";
        std::cout << "ИТОГОВАЯ ТАБЛИЦА: " << algorithm_name << " на " << dataset_type << "\n";
        std::cout << std::string(70, '=') << "\n";
        
        printf("%12s | %12s | %12s | %10s\n", 
               "Точек", "Время (сек)", "Память (MB)", "h");
        std::cout << std::string(70, '-') << "\n";
        
        for (const auto& r : results) {
            printf("%12zu | %12.2f | %12.2f | %10zu\n", 
                   r.size, r.time_sec, r.memory_mb, r.hull_size);
        }
        std::cout << std::string(70, '=') << "\n";
    }
    
    void saveToFile(const std::string& filename) {
        std::ofstream f(filename);
        f << algorithm_name << " на " << dataset_type << "\n";
        f << "================================\n";
        f << "Size\tTime(s)\tMemory(MB)\tHull\n";
        
        for (const auto& r : results) {
            f << r.size << "\t" << r.time_sec << "\t" 
              << r.memory_mb << "\t" << r.hull_size << "\n";
        }
        f.close();
        std::cout << "Результаты сохранены в " << filename << "\n";
    }
    
    // Экспорт данных для отчета (Markdown формат)
    void exportForReport(const std::string& filename) {
        std::ofstream f(filename);
        f << "#Результаты нагрузочного тестирования\n\n";
        f << "## " << algorithm_name << " на " << dataset_type << "\n\n";
        f << "| Размер | Время (сек) | Память (MB) | Вершин h |\n";
        f << "|--------|-------------|-------------|----------|\n";
        
        for (const auto& r : results) {
            f << "| " << r.size << " | " << r.time_sec 
              << " | " << r.memory_mb << " | " << r.hull_size << " |\n";
        }
        f.close();
        std::cout << "Отчет сохранен в " << filename << "\n";
    }
};

static LoadProfiler profiler;

BDD_GIVEN("the performance profiler is initialized", [](){});

BDD_AND("test datasets are generated on demand", [](){});

BDD_GIVEN("a GRID dataset for Graham scaling", [](){
    current_dataset_type = "GRID";
    current_dataset_size = 0;
});

BDD_WHEN("I run Graham scaling test on GRID", [](){
    std::vector<size_t> sizes = { 5000, 40000, 80000, 100000 };
    profiler.setConfig("Graham", "GRID");
    profiler.runLoadTest(sizes, Property::GRID);
});

BDD_THEN("the Graham GRID results are saved", [](){
    profiler.printTable();
    profiler.saveToFile("graham_grid_scaling.txt");
});

TEST_CASE("Graham scaling on GRID", "[graham][grid][scaling]") {
    std::cout << "\n=== GRAHAM: GRID dataset (скалирование) ===\n";
    
    CALL_GIVEN("the performance profiler is initialized");
    CALL_GIVEN("a GRID dataset for Graham scaling");
    CALL_WHEN("I run Graham scaling test on GRID");
    CALL_THEN("the Graham GRID results are saved");
}

BDD_GIVEN("a CIRCLE dataset for Graham scaling", []() {
    current_dataset_type = "CIRCLE";
    current_dataset_size = 0;
    });

BDD_WHEN("I run Graham scaling test on CIRCLE", []() {
    std::vector<size_t> sizes = { 700000, 1600000, 2000000, 5000000 };
    profiler.setConfig("Graham", "CIRCLE");
    profiler.runLoadTest(sizes, Property::CIRCLE);
    });

BDD_THEN("the Graham CIRCLE results are saved", []() {
    profiler.printTable();
    profiler.saveToFile("graham_circle_scaling.txt");
    });

TEST_CASE("Graham scaling on CIRCLE", "[graham][circle][scaling]") {
    std::cout << "\n=== GRAHAM: CIRCLE dataset (скалирование) ===\n";

    CALL_GIVEN("the performance profiler is initialized");
    CALL_GIVEN("a CIRCLE dataset for Graham scaling");
    CALL_WHEN("I run Graham scaling test on CIRCLE");
    CALL_THEN("the Graham CIRCLE results are saved");
}

BDD_GIVEN("a GRID dataset for Jarvis scaling", [](){
    current_dataset_type = "GRID";
    current_dataset_size = 0;
});

BDD_WHEN("I run Jarvis scaling test on GRID", [](){
    std::vector<size_t> sizes = {700000, 1600000, 2000000, 5000000};
    profiler.setConfig("Jarvis", "GRID");
    profiler.runLoadTest(sizes, Property::GRID);
});

BDD_THEN("the Jarvis GRID results are saved", [](){
    profiler.printTable();
    profiler.saveToFile("jarvis_grid_scaling.txt");
});

TEST_CASE("Jarvis scaling on GRID", "[jarvis][grid][scaling]") {
    std::cout << "\n=== JARVIS: CLUSTERS dataset (скалирование) ===\n";
    
    CALL_GIVEN("the performance profiler is initialized");
    CALL_GIVEN("a GRID dataset for Jarvis scaling");
    CALL_WHEN("I run Jarvis scaling test on GRID");
    CALL_THEN("the Jarvis GRID results are saved");
}

BDD_GIVEN("a CIRCLE dataset for Jarvis scaling", [](){
    current_dataset_type = "CIRCLE";
    current_dataset_size = 0;
});

BDD_WHEN("I run Jarvis scaling test on CIRCLE", [](){
    std::vector<size_t> sizes = {700000, 1600000, 2000000, 5000000};
    profiler.setConfig("Jarvis", "CIRCLE");
    profiler.runLoadTest(sizes, Property::CIRCLE);
});

BDD_THEN("the Jarvis CIRCLE results are saved", [](){
    profiler.printTable();
    profiler.saveToFile("jarvis_circle_scaling.txt");
});

TEST_CASE("Jarvis scaling on CIRCLE", "[jarvis][circle][scaling]") {
    std::cout << "\n=== JARVIS: CIRCLE dataset (скалирование) ===\n";
    
    CALL_GIVEN("the performance profiler is initialized");
    CALL_GIVEN("a CIRCLE dataset for Jarvis scaling");
    CALL_WHEN("I run Jarvis scaling test on CIRCLE");
    CALL_THEN("the Jarvis CIRCLE results are saved");
}
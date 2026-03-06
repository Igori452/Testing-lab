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

//GHERKIN_FEATURES_PATH="features" GHERKIN_FEATURE_FILE="performance_comparison.feature" ./performance_tests -s -d yes

using namespace jarvis_test;
using namespace graham_test;


namespace jarvis_test {
    bool operator<(const Point& a, const Point& b) {
        // TODO: реализовать оператор сравнения
        if (a.x != b.x) return a.x < b.x;
        return a.y < b.y;
    }
    
    bool operator==(const Point& a, const Point& b) {
        // TODO: реализовать оператор равенства
        return a.x == b.x && a.y == b.y;
    }
}

// Глобальные переменные для передачи данных между шагами
static std::vector<Point> dataset;
static std::vector<double> graham_times;
static std::vector<double> jarvis_times;
static double last_time_ms = 0;
static double avg_time_ms = 0;
static size_t dataset_size = 0;
static std::string current_dataset_type;
static size_t peak_memory_kb = 0;
static int current_run_count = 5; // значение по умолчанию
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
        // Если не Linux, возвращаем 0
        return 0;
    }
    
    long rss;
    statm >> rss; // пропускаем size
    statm >> rss; // читаем resident
    
    long page_size = sysconf(_SC_PAGESIZE);
    if (page_size == -1) {
        page_size = 4096; // значение по умолчанию
    }
    
    return (rss * page_size) / 1024; // в KB
}

enum class Property {
    CLUSTERS,      // кластеры (h маленькое)
    GRID           // решетка (h большое)
};

std::vector<Point> generateDataset(size_t n, Property prop) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::vector<Point> pts;
    pts.reserve(n);
    
    switch(prop) {        
        case Property::CLUSTERS: {
            // Кластеры - h маленькое (~10-20)
            std::vector<std::pair<double, double>> centers = {
                {300, 300}, {500, 500}, {700, 700}
            };
            std::uniform_int_distribution<> center_choice(0, centers.size() - 1);
            std::normal_distribution<> cluster(0, 30);
            
            // Добавляем немного граничных точек для оболочки
            std::uniform_int_distribution<> boundary(0, 1000);
            size_t boundary_count = n / 50;
            
            for (size_t i = 0; i < n; ++i) {
                if (i < boundary_count) {
                    pts.push_back({double(boundary(gen)), double(boundary(gen))});
                } else {
                    int c = center_choice(gen);
                    pts.push_back({
                        centers[c].first + cluster(gen),
                        centers[c].second + cluster(gen)
                    });
                }
            }
            break;
        }
        
        case Property::GRID: {
            // Решетка - h большое (все точки на границе)
            int side = sqrt(n);
            for (int i = 0; i < side; ++i) {
                for (int j = 0; j < side; ++j) {
                    pts.push_back({double(i * 1000/side), double(j * 1000/side)});
                }
            }
            // Если нужно больше точек, добиваем случайными
            std::uniform_real_distribution<> dis(0.0, 1000.0);
            while (pts.size() < n) {
                pts.push_back({dis(gen), dis(gen)});
            }
            break;
        }
    }
    
    return pts;
}

// =======================================================
// ПРОФАЙЛЕР ДЛЯ НАГРУЗОЧНОГО ТЕСТИРОВАНИЯ
// =======================================================
// =======================================================
// ПРОФАЙЛЕР ДЛЯ НАГРУЗОЧНОГО ТЕСТИРОВАНИЯ (с кэшированием)
// =======================================================
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
    
    // Очистка кэша (Linux)
    void clearCache() {
        sync();
        std::ofstream("/proc/sys/vm/drop_caches") << "3";
    }
    
    // Сохранение датасета в файл
    void saveDatasetToFile(const std::vector<Point>& dataset, size_t size) {
        std::string filename = "cache_" + dataset_type + "_" + std::to_string(size) + ".bin";
        std::ofstream f(filename, std::ios::binary);
        size_t points_count = dataset.size();
        f.write(reinterpret_cast<const char*>(&points_count), sizeof(size_t));
        f.write(reinterpret_cast<const char*>(dataset.data()), points_count * sizeof(Point));
        f.close();
    }
    
    // Загрузка датасета из файла
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
            std::cout << "  ✓ Загружено из кэша: " << filename << "\n";
        }
        return dataset;
    }
    
    // Проверка существования файла
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
    std::cout << "📊 НАГРУЗОЧНОЕ ТЕСТИРОВАНИЕ: " << algorithm_name 
              << " на " << dataset_type << "\n";
    std::cout << std::string(70, '=') << "\n";
    
    // Максимальный размер для генерации
    size_t max_size = 0;
    if (useCache) {
        for (size_t size : sizes) {
            if (size > max_size) max_size = size;
        }
        std::cout << "🔧 Максимальный размер: " << max_size << " точек\n";
    }
    
    for (size_t size : sizes) {
        std::cout << "\n▶ Тест на " << size << " точках...\n";
        
        // Загрузка или генерация данных
        std::vector<Point> dataset;
        if (useCache && datasetExists(size)) {
            dataset = loadDatasetFromFile(size);
        } else {
            dataset = generateDataset(size, prop);
            if (useCache) {
                saveDatasetToFile(dataset, size);
                std::cout << "  ✓ Сгенерировано и сохранено в кэш\n";
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
            
            printf("  ✓ Время: %.2f сек | Данные: %.2f MB | Алгоритм: %.2f MB | Всего: %.2f MB | h: %zu\n", 
                   time_sec, 
                   data_memory_kb/1024.0, 
                   algo_mem_kb/1024.0, 
                   total_mem_kb/1024.0, 
                   result.hull.size());
        } else {
            JarvisResult result = runJarvisAlgorithm(dataset);
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
            
            printf("  ✓ Время: %.2f сек | Данные: %.2f MB | Алгоритм: %.2f MB | Всего: %.2f MB | h: %zu\n", 
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
        std::cout << "📊 ИТОГОВАЯ ТАБЛИЦА: " << algorithm_name << " на " << dataset_type << "\n";
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
        std::cout << "✅ Результаты сохранены в " << filename << "\n";
    }
    
    // Экспорт данных для отчета (Markdown формат)
    void exportForReport(const std::string& filename) {
        std::ofstream f(filename);
        f << "# Результаты нагрузочного тестирования\n\n";
        f << "## " << algorithm_name << " на " << dataset_type << "\n\n";
        f << "| Размер | Время (сек) | Память (MB) | Вершин h |\n";
        f << "|--------|-------------|-------------|----------|\n";
        
        for (const auto& r : results) {
            f << "| " << r.size << " | " << r.time_sec 
              << " | " << r.memory_mb << " | " << r.hull_size << " |\n";
        }
        f.close();
        std::cout << "✅ Отчет сохранен в " << filename << "\n";
    }
};

static LoadProfiler profiler;

// =======================================================
// BACKGROUND (уже есть в вашем коде)
// =======================================================
BDD_GIVEN("the performance profiler is initialized", [](){
    // уже есть
});

BDD_AND("test datasets are generated on demand", [](){
    // уже есть
});

// =======================================================
// ТЕСТ 1: GRAHAM на CLUSTERS
// =======================================================
BDD_GIVEN("a CLUSTERS dataset for Graham scaling", [](){
    current_dataset_type = "CLUSTERS";
    current_dataset_size = 0;
});

BDD_WHEN("I run Graham scaling test on CLUSTERS", [](){
    std::vector<size_t> sizes = {100000, 200000, 400000, 700000};
    profiler.setConfig("Graham", "CLUSTERS");
    profiler.runLoadTest(sizes, Property::CLUSTERS);
});

BDD_THEN("the Graham CLUSTERS results are saved", [](){
    profiler.printTable();
    profiler.saveToFile("graham_clusters_scaling.txt");
});

/*
TEST_CASE("Graham scaling on CLUSTERS", "[graham][clusters][scaling]") {
    std::cout << "\n=== GRAHAM: CLUSTERS dataset (скалирование) ===\n";
    
    CALL_GIVEN("the performance profiler is initialized");
    CALL_GIVEN("a CLUSTERS dataset for Graham scaling");
    CALL_WHEN("I run Graham scaling test on CLUSTERS");
    CALL_THEN("the Graham CLUSTERS results are saved");
}*/

// =======================================================
// ТЕСТ 2: GRAHAM на GRID
// =======================================================
BDD_GIVEN("a GRID dataset for Graham scaling", [](){
    current_dataset_type = "GRID";
    current_dataset_size = 0;
});

BDD_WHEN("I run Graham scaling test on GRID", [](){
    std::vector<size_t> sizes = {100000, 200000, 400000, 700000};
    profiler.setConfig("Graham", "GRID");
    profiler.runLoadTest(sizes, Property::GRID);
});

BDD_THEN("the Graham GRID results are saved", [](){
    profiler.printTable();
    profiler.saveToFile("graham_grid_scaling.txt");
});

/*
TEST_CASE("Graham scaling on GRID", "[graham][grid][scaling]") {
    std::cout << "\n=== GRAHAM: GRID dataset (скалирование) ===\n";
    
    CALL_GIVEN("the performance profiler is initialized");
    CALL_GIVEN("a GRID dataset for Graham scaling");
    CALL_WHEN("I run Graham scaling test on GRID");
    CALL_THEN("the Graham GRID results are saved");
}*/

// =======================================================
// ТЕСТ 3: JARVIS на CLUSTERS
// =======================================================
BDD_GIVEN("a CLUSTERS dataset for Jarvis scaling", [](){
    current_dataset_type = "CLUSTERS";
    current_dataset_size = 0;
});

BDD_WHEN("I run Jarvis scaling test on CLUSTERS", [](){
    std::vector<size_t> sizes = {700000, 1100000, 1600000, 2000000};
    profiler.setConfig("Jarvis", "CLUSTERS");
    profiler.runLoadTest(sizes, Property::CLUSTERS);
});

BDD_THEN("the Jarvis CLUSTERS results are saved", [](){
    profiler.printTable();
    profiler.saveToFile("jarvis_clusters_scaling.txt");
});

TEST_CASE("Jarvis scaling on CLUSTERS", "[jarvis][clusters][scaling]") {
    std::cout << "\n=== JARVIS: CLUSTERS dataset (скалирование) ===\n";
    
    CALL_GIVEN("the performance profiler is initialized");
    CALL_GIVEN("a CLUSTERS dataset for Jarvis scaling");
    CALL_WHEN("I run Jarvis scaling test on CLUSTERS");
    CALL_THEN("the Jarvis CLUSTERS results are saved");
}

// =======================================================
// ТЕСТ 4: JARVIS на GRID
// =======================================================
BDD_GIVEN("a GRID dataset for Jarvis scaling", [](){
    current_dataset_type = "GRID";
    current_dataset_size = 0;
});

BDD_WHEN("I run Jarvis scaling test on GRID", [](){
    std::vector<size_t> sizes = {700000, 1100000, 1600000, 2000000};
    profiler.setConfig("Jarvis", "GRID");
    profiler.runLoadTest(sizes, Property::GRID);
});

BDD_THEN("the Jarvis GRID results are saved", [](){
    profiler.printTable();
    profiler.saveToFile("jarvis_grid_scaling.txt");
});

TEST_CASE("Jarvis scaling on GRID", "[jarvis][grid][scaling]") {
    std::cout << "\n=== JARVIS: GRID dataset (скалирование) ===\n";
    
    CALL_GIVEN("the performance profiler is initialized");
    CALL_GIVEN("a GRID dataset for Jarvis scaling");
    CALL_WHEN("I run Jarvis scaling test on GRID");
    CALL_THEN("the Jarvis GRID results are saved");
}
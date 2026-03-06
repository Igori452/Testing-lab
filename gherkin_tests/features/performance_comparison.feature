Feature: Load Testing of Convex Hull Algorithms
  As a GIS analyst
  I need to validate performance hypotheses for Jarvis and Graham algorithms
  To understand their behavior on different dataset types

  Background:
    Given the performance profiler is initialized
    And test datasets are generated on demand

  # ============================================================================
  # ГИПОТЕЗА О СЛОЖНОСТИ
  # ============================================================================
  
  Rule: Graham algorithm complexity should be O(n log n) regardless of hull size

    Scenario Outline: Verification of Graham complexity hypothesis
      Given I have a <type> dataset with <size> points
      When I measure Graham execution time <n> times and calculate average
      Then the time ratio between different sizes approximates theoretical O(n log n)
      And the results are saved for analysis

      Examples:
        | type   | size | n |
        | sparse | 1000 | 5 |
        | sparse | 2000 | 5 |
        | sparse | 4000 | 5 |
        | sparse | 8000 | 5 |
        | dense  | 1000 | 5 |
        | dense  | 2000 | 5 |
        | dense  | 4000 | 5 |
        | dense  | 8000 | 5 |

  Rule: Jarvis algorithm complexity should be O(n*h) where h is hull vertices count

    Scenario Outline: Verification of Jarvis complexity hypothesis
      Given I have a <type> dataset with <size> points and hull size ~<h>
      When I measure Jarvis execution time <n> times and calculate average
      Then the time ratio between different datasets approximates theoretical O(n*h)
      And the results are compared with Graham performance

      Examples:
        | type   | size | h   | n |
        | sparse | 1000 | ~10 | 5 |
        | sparse | 2000 | ~10 | 5 |
        | sparse | 4000 | ~10 | 5 |
        | dense  | 1000 | ~300| 5 |
        | dense  | 2000 | ~600| 5 |
        | dense  | 4000 | ~1200|5 |

  # ============================================================================
  # ДАТАСЕТ 1: ИЗМЕРЕНИЕ ПАМЯТИ (небольшой)
  # ============================================================================
  
  Rule: Memory consumption should be measured on moderate dataset

    Scenario: Memory analysis on 10000 points
      Given a mixed dataset with exactly 10000 points
      When I run both algorithms with memory profiling
      Then peak memory usage is recorded for:
        | algorithm | max_memory_MB |
        | Jarvis    | < 50          |
        | Graham    | < 50          |
      And the hull vertices count is saved for reference
      And both algorithms produce identical hulls

  # ============================================================================
  # ДАТАСЕТ 2: ИЗМЕРЕНИЕ ВРЕМЕНИ (большой, 2-15 минут)
  # ============================================================================
  
  Rule: Execution time should be measured on large dataset (2-15 minutes)

    Scenario: Time analysis on large dense dataset (target: 5-10 minutes)
      Given a dense dataset with 50000 points
      When I run the Graham algorithm and measure execution time
      Then the execution time is between 2 and 15 minutes
      And the exact time is logged: <time> minutes
      And the result is saved to "output/graham_large.txt"

    Scenario: Time analysis on large sparse dataset (target: 2-5 minutes)
      Given a sparse dataset with 100000 points
      When I run the Graham algorithm and measure execution time
      Then the execution time is between 2 and 10 minutes
      And the exact time is logged: <time> minutes
      And the result is saved to "output/graham_sparse_large.txt"

    Scenario: Comparison on maximum dataset
      Given a dense dataset with 100000 points
      When I run the Graham algorithm
      Then the execution time does not exceed 30 minutes
      And memory usage is below 1200 MB
      And results are saved for reference

  # ============================================================================
  # ФИНАЛЬНЫЙ АНАЛИЗ
  # ============================================================================
  
  Rule: All collected data should be aggregated for final report

    Scenario: Generate performance report
      Given all measurements are completed
      When I aggregate results from all test runs
      Then a summary report is generated containing:
        | section                 | content                          |
        | complexity verification | O(n log n) vs O(n*h) confirmation|
        | memory analysis         | peak usage for 10k points        |
        | large dataset timing    | execution times for 50k-100k     |
        | recommendations         | algorithm selection guidelines   |
      And the report is saved as "load_test_report.txt"

  # ============================================================================
  # ПРИМЕЧАНИЯ
  # ============================================================================
  # - Генератор данных должен создавать:
  #   * sparse: точки сгруппированы в центре, мало вершин оболочки (~10-20)
  #   * dense: равномерное распределение, много вершин оболочки (~30-50% точек)
  #   * mixed: комбинация обоих подходов
  # - Для датасета 2 (время) используем dense 50000 и sparse 100000
  # - Ожидаемое время на reference hardware:
  #   * dense 50000: ~5-10 минут
  #   * sparse 100000: ~2-5 минут
  #   * dense 100000: ~15-25 минут
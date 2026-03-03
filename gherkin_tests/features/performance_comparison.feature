Feature: Geographic Data Analysis
  In order to quickly determine boundaries of object clusters
  As a GIS analyst
  I need to compute convex hulls efficiently for large datasets

  Background:
    Given time profiler is running
    And test datasets have been loaded from the "datasets/" folder
    And the "preliminary_experiment.log" shows that datasets > 15000 points take > 2 minutes

  Rule: For datasets up to 1000 points, any algorithm is acceptable

    # Декларативный стиль (описываем ЧТО проверяем)
    Scenario: Jarvis on 500 points
      Given a sparse dataset with 500 points forming 10 hull vertices
      When I run the Jarvis algorithm and measure time
      Then the execution time is less than 100 ms
      And the hull is correctly constructed

    Scenario: Graham on 500 points  
      Given a sparse dataset with 500 points forming 10 hull vertices
      When I run the Graham algorithm and measure time
      Then the execution time is less than 50 ms
      And the hull matches the Jarvis result

    Scenario: Result comparison on 800 points
      Given a mixed dataset with 800 points (dense and sparse regions)
      When I run both algorithms
      Then the sets of hull vertices are identical
      And the order of vertices may differ, but the set is the same  # Перекрестная верификация

  Rule: For datasets from 1000 to 10000 points, Graham is recommended

    # Императивный стиль (подробно описываем процесс измерения)
    Scenario: Graham on 5000 points with performance measurement
      Given I have a dense dataset "dense_5000.txt" where most points are vertices
      When I start the performance profiler
      And I run the Graham algorithm 5 times and calculate average time
      Then the average execution time is less than 2 seconds
      And memory consumption does not exceed 50 MB

    Scenario: Jarvis on 5000 points should be slower
      Given the same dense dataset "dense_5000.txt"
      When I run the Jarvis algorithm with the same profiling settings
      Then the execution time is at least 3 times slower than Graham
      And the theoretical complexity O(n*h) is confirmed (h ≈ 500)

    Scenario: Correctness check on 7500 points
      Given a random dataset "random_7500.txt"
      When I run both algorithms
      Then the Graham result matches the Jarvis result
      And the hull area differs by less than 1e-8  # Проверка точности

  Rule: For datasets over 10,000 points, Graham is required

    Scenario: Graham on 20000 points within time limits
      Given a dense dataset "dense_20000.txt"
      When I run the Graham algorithm
      Then execution time is less than 30 minutes
      And the result is saved to "output/graham_20000.txt"

    Scenario: Jarvis on 20000 points exceeds time limits (preliminary experiment)
      Given the same dense dataset "dense_20000.txt"
      When I run the Jarvis algorithm
      Then execution time exceeds 2 minutes  # t1 from preliminary experiment
      And the time measurement is logged for comparison with unit tests

  Scenario Outline: Performance scaling with different data types
    Given a <type> dataset with <size> points
    When I measure execution time of both algorithms
    Then the Graham to Jarvis time ratio corresponds to theoretical O(n log n) / O(n*h)

    Examples:
      | type       | size  | expected_hull_size |
      | sparse     | 100   | ~10                |
      | sparse     | 1000  | ~10                |
      | sparse     | 10000 | ~10                |
      | dense      | 100   | ~50                |
      | dense      | 1000  | ~300               |
      | dense      | 10000 | ~2000              |

  # Примечание: результаты нагрузочного тестирования сравниваются с unit-тестами
  # TC_JARVIS_STMT_004 и TC_GRAHAM_STMT_005 подтверждают корректность,
  # а производительность проверяется в данном feature-файле
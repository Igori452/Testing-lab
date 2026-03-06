Feature: Load Testing of Convex Hull Algorithms
  As a performance engineer
  I want to measure time and memory consumption of Graham and Jarvis algorithms
  On different dataset types with increasing sizes

  Background:
    Given the performance profiler is initialized
    And test datasets are generated on demand

  # =======================================================
  # GRAHAM ALGORITHM TESTS
  # =======================================================
  
  Scenario: Graham algorithm performance on CLUSTERS dataset
    Given a CLUSTERS dataset for Graham scaling
    When I run Graham scaling test on CLUSTERS
    Then the Graham CLUSTERS results are saved

  Scenario: Graham algorithm performance on GRID dataset
    Given a GRID dataset for Graham scaling
    When I run Graham scaling test on GRID
    Then the Graham GRID results are saved

  # =======================================================
  # JARVIS ALGORITHM TESTS
  # =======================================================
  
  Scenario: Jarvis algorithm performance on CLUSTERS dataset
    Given a CLUSTERS dataset for Jarvis scaling
    When I run Jarvis scaling test on CLUSTERS
    Then the Jarvis CLUSTERS results are saved

  Scenario: Jarvis algorithm performance on GRID dataset
    Given a GRID dataset for Jarvis scaling
    When I run Jarvis scaling test on GRID
    Then the Jarvis GRID results are saved
Feature: Performance comparison of convex hull algorithms
  As a developer
  I want to compare the performance of Graham and Jarvis algorithms
  So that I can understand their scaling characteristics on different datasets

  Background:
    Given the performance profiler is initialized
    And test datasets are generated on demand

  Scenario: Graham scaling on CLUSTERS dataset
    Given a CLUSTERS dataset for Graham scaling
    When I run Graham scaling test on CLUSTERS
    Then the Graham CLUSTERS results are saved

  Scenario: Graham scaling on GRID dataset
    Given a GRID dataset for Graham scaling
    When I run Graham scaling test on GRID
    Then the Graham GRID results are saved

  Scenario: Jarvis scaling on CLUSTERS dataset
    Given a CLUSTERS dataset for Jarvis scaling
    When I run Jarvis scaling test on CLUSTERS
    Then the Jarvis CLUSTERS results are saved

  Scenario: Jarvis scaling on CIRCLE dataset
    Given a CIRCLE dataset for Jarvis scaling
    When I run Jarvis scaling test on CIRCLE
    Then the Jarvis CIRCLE results are saved
Feature: Performance comparison of convex hull algorithms
  As a developer
  I want to compare the performance of Graham and Jarvis algorithms on different dataset types
  So that I can understand their scaling characteristics

  Background:
    Given the performance profiler is initialized
    And test datasets are generated on demand

  Scenario: Graham scaling on GRID dataset
    Given a GRID dataset for Graham scaling
    When I run Graham scaling test on GRID
    Then the Graham GRID results are saved

  Scenario: Graham scaling on CIRCLE dataset
    Given a CIRCLE dataset for Graham scaling
    When I run Graham scaling test on CIRCLE
    Then the Graham CIRCLE results are saved

  Scenario: Jarvis scaling on GRID dataset
    Given a GRID dataset for Jarvis scaling
    When I run Jarvis scaling test on GRID
    Then the Jarvis GRID results are saved

  Scenario: Jarvis scaling on CIRCLE dataset
    Given a CIRCLE dataset for Jarvis scaling
    When I run Jarvis scaling test on CIRCLE
    Then the Jarvis CIRCLE results are saved
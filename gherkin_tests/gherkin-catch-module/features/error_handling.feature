Feature: Handling errors and edge cases
  As a QA-engineer
  I want to check the program's resilience to incorrect input data
  So that I guarantee no crashes and clear messages to the user

  Rule: Validation of input data

    # Императивный стиль
    Scenario: Loading a file with invalid format
      Given I have opened the program
      And I create a file "corrupted.txt" with the content:
        """
        abc def
        100 xyz
        !@# $
        """
      When I press the "Load File" button and select "corrupted.txt"
      Then the program does not terminate abnormally
      And no new points are displayed on the plane
      But an error message "Invalid data format" is printed to the console

    # Декларативный стиль
    Scenario: Loading a file with negative coordinates
      Given a file contains points with negative coordinates:
        | x   | y   |
        | -10 | -20 |
        | 100 | -5  |
        | -30 | 50  |
      When I load this file into the program
      Then negative coordinates are displayed on the plane
      But building a hull is only possible for points in the range [0,600]

    Scenario: File not found
      Given the file "nonexistent.txt" does not exist in the program directory
      When I try to load this file
      Then an error message "File not found!" is displayed
      And the plane remains in its previous state

    Scenario: Boundary value - exactly 3 non-collinear points
      Given a set of exactly 3 non-collinear points:
        | x   | y   |
        | 0   | 0   |
        | 100 | 0   |
        | 0   | 100 |
      When I run any convex hull algorithm
      Then the hull is successfully built as a triangle

    Scenario: Boundary value - exactly 2 points
      Given a set of exactly 2 points:
        | x   | y   |
        | 100 | 100 |
        | 200 | 200 |
      When I run any convex hull algorithm
      Then the algorithm returns an empty hull
      And a message "Cannot build Convex Hull!" is displayed

    Scenario: Points exactly on the drawing boundary
      Given a set of points on the boundaries [0,600]:
        | x   | y   |
        | 0   | 0   |
        | 600 | 0   |
        | 600 | 600 |
        | 0   | 600 |
        | 300 | 300 |
      When I run the Jarvis algorithm
      Then all boundary points are included in the hull
      And the hull has 4 vertices

  Scenario Outline: Testing various incorrect data scenarios
    Given <condition>
    When I try to build a convex hull
    Then the program displays "<error_message>"
    And the application continues running

    Examples:
      | condition                            | error_message                | test_coverage |
      | less than 3 points                   | "Cannot build Convex Hull!"  | TC_BV_002, TC_BV_003 |
      | all points are collinear             | "Cannot build Convex Hull!"  | TC_BV_005 |
      | file with empty lines                | "Invalid data format"        | TC_EP_001 |
      | points with non-numeric characters   | "Invalid data format"        | - |
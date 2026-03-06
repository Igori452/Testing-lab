Feature: Construction of contour processing algorithm of Graham algorithm
  As a data analyst
  I want to construct a convex hull using Graham's algorithm
  So that I can compare the results with Jarvis's algorithm for verification

  Background:
    Given the coordinate plane is cleared
    And test points from the file "points.txt" were loaded

  Rule: Correctness of shell construction

    # Декларативный стиль (описываем ЧТО происходит)
    Scenario: Successful construction for a random set of points
      Given a set of 6 points with different coordinates
      When I run the Graham algorithm
      Then a green contour is displayed on the screen
      And each vertex of the contour is an extreme point of the set
      And all other points lie inside the contour  # TC_GRAHAM_STMT_005

    # Императивный стиль (описываем КАК пользователь это делает)
    Scenario: Building a hull with filtering points outside the range
      Given I have opened the program
      And I load the file "mixed_points.txt" containing:
        """
        100 100
        500 100
        -10 -10
        700 700
        300 400
        """
      When I press the "Load File" button
      And I press the "Find Graham Hull" button
      Then points with coordinates [-10,-10] and [700,700] are filtered out
      And the hull is built from points [100,100], [500,100], [300,400]
      But the program continues working without crashes  # TC_GRAHAM_BRANCH_007

    Scenario: Empty set of points
      Given the file "points.txt" is empty
      When I run the Graham algorithm
      Then the algorithm returns an empty hull
      And a green message "Cannot build Convex Hull!" is displayed  # TC_GRAHAM_STMT_001

    Scenario: All points are collinear
      Given a set of collinear points:
        | x   | y   |
        | 0   | 0   |
        | 50  | 50  |
        | 100 | 100 |
        | 150 | 150 |
      When I run the Graham algorithm
      Then the algorithm returns an empty hull  # TC_GRAHAM_STMT_002

  Scenario Outline: Testing the sorting by polar angle
    Given a set of points with collinear points
    When I run the Graham algorithm
    Then collinear points are removed during sorting
    And only unique extreme points remain in the hull

    Examples:
      | point_set                            | expected_vertices |
      | [(0,0), (1,0), (2,0), (0,1)]         | 3                 |  # TC_GRAHAM_STMT_004
      | [(0,0), (100,0), (50,50), (0,100)]   | 3                 |
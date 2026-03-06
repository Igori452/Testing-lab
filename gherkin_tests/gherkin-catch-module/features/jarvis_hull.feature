Feature: Construction of contour processing algorithm of Jarvis algorithm
  As a data analyst
  I want to construct the convex hull of a set of points using Jarvis's algorithm
  So that I can visually define the boundaries of a point cloud for geographic analysis

  Background:
    Given the coordinate plane is cleared
    And test points from the file "points.txt" were loaded

  Rule: Construction of a shell is only possible with specific input data

    # Декларативный стиль (описываем ЧТО происходит, не вдаваясь в детали)
    Scenario: Successful construction for the minimum set of points
      Given a set of 3 non-collinear points:
        | x   | y   |
        | 0   | 0   |
        | 100 | 0   |
        | 0   | 100 |
      When I run the Jarvis algorithm
      Then a red triangle is displayed on the screen
      And the hull contains exactly 3 vertices
      And all original points are either vertices or lie inside  # TC_EP_004

    # Императивный стиль (описываем КАК пользователь это делает)
    Scenario: Attempt to build for two points
      Given I have opened the program
      And I load the file "two_points.txt" containing:
        """
        100 100
        200 200
        """
      When I press the "Load File" button
      And I press the "Find Jarvis Hull" button
      Then a red message "Cannot build Convex Hull!" appears above the button
      And the hull is not displayed on the plane  # TC_EP_003, TC_BV_003

    Scenario: Empty set of points
      Given the file "points.txt" is empty
      When I run the Jarvis algorithm
      Then the algorithm returns an empty hull
      And the plane remains empty  # TC_EP_001, TC_BV_001

    Scenario: All points are collinear
      Given a set of collinear points:
        | x   | y   |
        | 0   | 0   |
        | 50  | 50  |
        | 100 | 100 |
      When I run the Jarvis algorithm
      Then the algorithm returns an empty hull
      And a message "Cannot build Convex Hull!" is displayed  # TC_EP_005, TC_BV_005

    Scenario: All points have the same coordinates
      Given a set of points with identical coordinates:
        | x   | y   |
        | 100 | 100 |
        | 100 | 100 |
        | 100 | 100 |
        | 100 | 100 |
      When I run the Jarvis algorithm
      Then the algorithm returns an empty hull  # TC_EP_002

  Scenario Outline: Building a hull for different configurations of points
    Given a set of points:
      | x | y |
      | <x1> | <y1> |
      | <x2> | <y2> |
      | <x3> | <y3> |
      | <x4> | <y4> |
    When I run the Jarvis algorithm
    Then the number of vertices in the hull equals <hull_size>
    And all vertices have coordinates from the set of original points

    Examples:
      | x1 | y1 | x2 | y2 | x3 | y3 | x4 | y4 | hull_size | test_coverage      |
      | 0  | 0  | 100| 0  | 100| 100| 0  | 100| 4         | TC_EP_006, TC_BV_006|
      | 0  | 0  | 100| 0  | 50 | 50 | 0  | 100| 3         | TC_JARVIS_STMT_004  |
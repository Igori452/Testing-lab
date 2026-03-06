Feature: Visualisation dots and hull
  As a program user
  I want to see loaded points and the constructed shell on the coordinate plane
  So that I can clearly present the results and use them in presentations

  Background:
    Given coordinate plane of 600x600 pixels
    And coordinate axes are displayed
    And the program window is open

  Rule: Correct display of graphic elements

    # Декларативный стиль (описываем ЧТО должно быть)
    Scenario: Different colors for different algorithms
      Given points are loaded on the plane
      When I run the Jarvis algorithm
      Then the hull is displayed in red color  # Как на рисунке 2.3
      When I run the Graham algorithm
      Then the hull is displayed in green color  # Как на рисунке 2.4

    Scenario: Clearing the plane when loading new data
      Given points and a hull are displayed on the plane
      When I load a new file "new_points.txt"
      Then the previous points and hull disappear
      And only the new points are displayed  # Очистка перед новой загрузкой

    Scenario: Error message display for impossible hull construction
      Given collinear points are loaded:
        | x   | y   |
        | 100 | 100 |
        | 200 | 200 |
        | 300 | 300 |
      When I press the "Find Jarvis Hull" button
      Then a red message "Cannot build Convex Hull!" appears above the button  # Как на рисунке 2.5
      When I press the "Find Graham Hull" button
      Then a green message "Cannot build Convex Hull!" appears above the button  # Как на рисунке 2.6

    Scenario: Displaying points with exact coordinates
      Given I load points with coordinates on the boundaries:
        | x   | y   |
        | 0   | 0   |
        | 600 | 0   |
        | 600 | 600 |
        | 0   | 600 |
      When the points are displayed
      Then they are visible at the corners of the plane
      And no points are cut off  # Проверка границ отрисовки

  Scenario Outline: Visual verification of different point sets
    Given the file "<dataset>" is loaded
    When the points are displayed
    Then the number of visible points equals <count>
    And the minimum coordinate is >= 0
    And the maximum coordinate is <= 600

    Examples:
      | dataset        | count | test_coverage      |
      | points_10.txt  | 10    | Визуальная проверка |
      | points_100.txt | 100   | Визуальная проверка |
      | points_500.txt | 500   | Визуальная проверка |

  Rule: UI elements respond correctly to user actions

    Scenario: Exit button functionality
      Given the program is running
      When I press the "Exit" button
      Then the program window closes
      And the process terminates  # Как описано в разделе 2.1
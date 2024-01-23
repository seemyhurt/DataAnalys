cmake_minimum_required(VERSION 3.14)

project(DataAnalys LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(density_distribution_analysis)
add_subdirectory(distribution_analysis)
add_subdirectory(least_square_method)
add_subdirectory(random_distribution_estimates)
add_subdirectory(random_variable_analysis)
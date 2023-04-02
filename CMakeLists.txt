cmake_minimum_required(VERSION 3.25)
project(Projet_Labyrinthe C)

set(CMAKE_C_STANDARD 11)

add_executable(Projet_Labyrinthe
        LabyrintheSansThreads.c)
add_executable(Projet_Labyrinthe2
        LabyrintheAvecMultiThreads.c)
add_executable(Projet_Labyrinthe3
        LabyrintheAvecMutex.c)

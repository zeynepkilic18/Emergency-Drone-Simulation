/**
 * @file simulator.h
 * @author 
 * @brief 
 * @version 0.1
 * @date 2024-04-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "list.h"

#define IDLE 0
#define IN_TRANSIT 1

/*type for a coordinate*/
typedef struct coord {
    int x;
    int y;
} Coord;

/*type for a survivor*/
typedef struct survivor {
    int status;
    Coord coord;
    struct tm discovery_time; /*you can use struct tm, too*/
    struct tm helped_time;
    char info[25];
} Survivor;

/*type for a cell in the map*/
typedef struct mapcell {
    Coord coord;       /*in our program simply i,j*/
    List *survivors; /*the survivors waiting help in this cell*/
    List *drones;
} MapCell;

/*2d map type*/
typedef struct map {
    int height; /*height of the map*/
    int width;  /*width of the map*/
    MapCell **cells; /*pointer to 2d MapCell*/
} Map;

/*type for a drone*/
typedef struct drone {
    int status;
    int numberofhelped;
    Coord destination;
    Coord coord;
    struct tm stime;
    char info[30];
} Drone;

extern Map map;

/*lists*/
extern List *survivors;/* survivor list, waiting for help*/         
extern List *helpedsurvivors; /* helped survivor list*/
extern List *drones;  /*drone list*/

/*model.c*/
void init_map(int height, int width);
void freemap();
Drone *create_drone(Coord *coord, char *info, struct tm *stime);
void survivor_generator(void *args);
Survivor *create_survivor(Coord *coord, char *info, struct tm *discovery_time);
void move_drone(Drone *drone);
void drone_runner(void *drone);
void drone_controller(void *args);

/*view.c*/
int init_sdl_window();
static void draw_rectangles();
int draw_map();
int check_events();
int quit_all();


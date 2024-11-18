/**
 * @file model.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-04-21
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "simulator.h"
#define MAX_SURIVOR_PER_CELL 3
#include <time.h> // 'time' ve 'localtime' fonksiyonları için gerekli
#include <unistd.h> // 'sleep' fonksiyonu için gerekli
#include <limits.h>
#define WORKING 1

/*SOME EXAMPLE FUNCTIONS GIVEN BELOW*/
Map map;
int numberofcells = 0;
List *survivors;
List *drones;
List *helpedsurvivors;


void init_map(int height, int width) {
    map.height = height;
    map.width = width;
    numberofcells = height * width;
    survivors = create_list(sizeof(Survivor), numberofcells * 10);

    /*pointer array*/
    map.cells = malloc(sizeof(MapCell *) * map.height);

    for (int j = 0; j < map.height; j++) {
        /*rows for each pointer*/
        map.cells[j] = malloc(sizeof(MapCell) * map.width);
    }

    for (int i = 0; i < map.height; i++) {
        for (int j = 0; j < map.width; j++) {
            map.cells[i][j].coord.x = i;
            map.cells[i][j].coord.y = j; /**/
            map.cells[i][j].survivors = create_list(sizeof(Survivor), 10);
        }
    }
    printf("height: %d, width:%d\n", map.height, map.width);
    

}

void freemap() {
    for (int i = 0; i < map.height; i++) {
        for (int j = 0; j < map.width; j++) {
            List *list = map.cells[i][j].survivors;
            list->destroy(list);
        }
        free(map.cells[i]);
    }
    free(map.cells);
}

Survivor *create_survivor(Coord *coord, char *info, struct tm *discovery_time) {
    Survivor *s = malloc(sizeof(Survivor));
    memset(s, 0, sizeof(Survivor));
    memcpy(&(s->discovery_time), discovery_time, sizeof(struct tm));
    strncpy(s->info, info, sizeof(s->info));
    memcpy(&(s->coord), coord, sizeof(Coord));

    printf("survivor: %s", asctime(&s->discovery_time));
    printf("%s\n\n", s->info);
    return s;
}

/*THREAD FUNCTION: generates random survivor
 */
void survivor_generator(void *args) {
    // generate random location
    if (map.cells != NULL) {
        time_t traw;
        struct tm t; /*used for localtime*/

        /*survivor info*/
        char info[5] = {'A' + (random() % 26), 
                        'A' + (random() % 26),
                        '0' + (random() % 9),
                        '0' + (random() % 9)}; 

        Coord coord = {random() % map.height, random() % map.width};

        time(&traw);
        localtime_r(&traw, &t);

        printf("creating survivor...%s\n", asctime(&t));
        Survivor *s = create_survivor(&coord, info, &t);

        /*add to general list*/
        add(survivors, s);

        /*add to the list in the cell*/
        List *list = map.cells[coord.x][coord.y].survivors;
        add(list, s);

        printf("survivor added, celllist-size:%d\n", list->number_of_elements);
    }
}

Drone *create_drone(Coord *coord, char *info, struct tm *stime) {
    Drone *d = malloc(sizeof(Drone));   /* Create survivor ile benzer mantıkta drone oluşturma */
    memset(d, 0, sizeof(Drone));
    memcpy(&(d->stime), stime, sizeof(struct tm));
    strncpy(d->info, info, sizeof(d->info));
    memcpy(&(d->coord), coord, sizeof(Coord));

    d->status = IDLE;
    return d;
}


/** a drone delivers aid pack to survivor,
the survivor is marked as helped and removed*/
 void help_survivor(Drone *d, Survivor *s) {
    Node *temp_node = malloc(sizeof(Node) + sizeof(Survivor));
    if (temp_node == NULL) {
        fprintf(stderr, "Bellek tahsisi hatası!!!\n");
        return;
    }

    Survivor *survivor = (Survivor *)((char *)temp_node + sizeof(Node));
    memcpy(survivor, s, sizeof(Survivor));
    
    /*TODO: edit help_time, add to the helped_survivors*/
    time_t current_time;
    time(&current_time);
    memcpy(&(survivor->helped_time), localtime(&current_time), sizeof(struct tm));
    
    // Yardım edilenler listesine ekle
    add(helpedsurvivors, survivor);

    /*TODO: numberofhelped++ */
    d->numberofhelped++;


    /*TODO: remove survivor from survivorlist, mapcell.survivors*/
    List *list = map.cells[s->coord.x][s->coord.y].survivors;
    removenode(list, temp_node);

    free(temp_node);
} 



/** moves(flies) drone on the map:
based on its speed it jumps cells toward its destination*/
void move_drone(Drone *drone) {
    if (drone->coord.x == drone->destination.x && drone->coord.y == drone->destination.y) {
        printf("Drone (%d, %d). konumuna vardı.\n", drone->coord.x, drone->coord.y);
        return;
    }

    if (drone->coord.x < drone->destination.x) {
        drone->coord.x++;
    } else if (drone->coord.x > drone->destination.x) {
        drone->coord.x--;
    }

    if (drone->coord.y < drone->destination.y) {
        drone->coord.y++;
    } else if (drone->coord.y > drone->destination.y) {
        drone->coord.y--;
    }

    printf("Drone (%d, %d). konuma hareket ediyor.\n", drone->coord.x, drone->coord.y);
}

/*THREAD FUNCTION: simulates a drone: */
void drone_runner(void *drone) {
    Drone *current_drone = (Drone *)drone;

    while (current_drone->status == WORKING) {
        sleep(1);

        if (current_drone->status == WORKING) {
            move_drone(current_drone);
        }
    }

    current_drone->status = IDLE;

    printf("Drone görevi bitirdi.\n");
}

/*THREAD FUNCTION: an AI that controls drones based on survivors*/
void drone_controller(void *args) {
    while (1) {
        for (int i = 0; i < drones->number_of_elements; i++) {
            Drone *current_drone = (Drone *)drones->peek(drones);

            if (current_drone->status != IDLE) {
                drones->pop(drones, NULL);
                continue;
            }

            if (survivors->number_of_elements == 0) {
                current_drone->status = IDLE;
                break;
            }

            drones->pop(drones, NULL);
        }

        sleep(1);
    }

}
/*you should add all the necessary functions
you can add more .c files e.g. survior.c, drone.c
But, for MVC model, controller is a bridge between view and model:
put data update functions only in model, not in your view or controller.
*/

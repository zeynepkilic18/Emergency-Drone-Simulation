/*
 * controller.c
 *      Author: adaskin
 */
#include <pthread.h> 
#include <stdlib.h>  
#include <stdio.h>   
#include <time.h>    
#include <unistd.h>
#include "SDL2/SDL.h"
#include "simulator.h"

/*do not change any of this*/
extern SDL_bool done;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char* argv[]) {
    /*initializes map*/
    init_map(10, 30);

    /*initializes window*/
    printf("initialize window\n");
    init_sdl_window(map);

    printf("draw map\n");
    /*draws initial map*/
    draw_map();

    /* repeat until window is closed */
    while (!done) {
        /*check user events: e.g. click to close x*/
        check_events();

        /* update model:survivors, drones etc. */
        survivor_generator(NULL);

        /*draws new updated map*/
        draw_map();

        SDL_Delay(1000); /*sleep(1);*/
    }

    /* Survivor Generator fonksiyonunu çalıştırmak için thread oluşturduk */
    pthread_t survivor_generator_thread;
    pthread_create(&survivor_generator_thread, NULL, (void *)survivor_generator, NULL);

    /* Drone runner fonksiyonunu çalıştırmak için thread oluşturduk */
    pthread_t drone_runner_thread;
    pthread_create(&drone_runner_thread, NULL, (void *)drone_runner, NULL);

    /* Drone controller fonksiyonunu çalıştırmak için thread oluşturduk */
    pthread_t drone_controller_thread;
    pthread_create(&drone_controller_thread, NULL, (void *)drone_controller, NULL);

    printf("quitting...\n");
    freemap();

    /* Tüm thread'lerin bitmesini bekliyoruz */
    pthread_join(survivor_generator_thread, NULL);
    pthread_join(drone_runner_thread, NULL);
    pthread_join(drone_controller_thread, NULL);
    

    /*quit everything*/
    quit_all();

    return 0;
}


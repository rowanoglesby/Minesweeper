/**
* @file Main.c
* @brief  Is an entry point for terminal Minesweeper
* initilaizes NOTCURSES which runs configuration screen and enters into main group loop
 */
 #include <stdlib.h>
 #include <notcurses/notcurses.h>
 #include "config.h"
 #include "game.h"
 #include "ui.h"

 /**
 * @brief Program entry point.
 * @return 0 on sucsess and a 1 on failure
 */
 Int main(void) {
    Struct notcurses_options opts ={0};
    Struct notcurses *nc = notcurses_init(&opts, NULL);
    If (!nc) {
        fprintf(stderr, "Failed to start Notcurses\n");
        Return 1;
    }
    GameConfig cfg = config_prompt(nc);
    game_run(nc, &cfg);
    return(0)
 }
/**
 * 
 * Rowan Oglesby
 * 5-17-2026
 * 900367291
 * 
* @file Main.c
* @brief  Is an entry point for terminal Minesweeper
* initilaizes NOTCURSES which runs configuration screen and enters into main group loop
*/
#include <stdlib.h>
#include <notcurses/notcurses.h>
#include "Extra_Definitions.h"
/**
 * @brief Program entry point.
 * @return 0 on sucsess and a 1 on failure
 */
int main(void) {
   struct notcurses_options opts ={0};
   struct notcurses *nc = notcurses_init(&opts, NULL);
   if (!nc) {
       fprintf(stderr, "Failed to start Notcurses\n");
       return 1;
   }
   GameConfig cfg = config_prompt(nc);
   game_run(nc, &cfg);
   return(0);
}
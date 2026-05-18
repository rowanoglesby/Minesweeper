/** 
 * Rowan Oglesby
 * 5-13-2026
 * 900367291
 * 
 * @file input.c
 * @brief deals with all of the inputs from the player so they can play the game.
 */

#include "Extra_Definitions.h"
#include <stdlib.h>
#define BOARD_ORIGIN_ROW 2
#define CELL_WIDTH       2

/**
 * @brief Inputs for arrow keys and wasd keys
 */
static Action key_to_action(const struct ncinput *ni) {
    switch (ni->id) {
        case 'w': case NCKEY_UP:    return ACTION_UP;
        case 's': case NCKEY_DOWN:  return ACTION_DOWN;
        case 'a': case NCKEY_LEFT:  return ACTION_LEFT;
        case 'd': case NCKEY_RIGHT: return ACTION_RIGHT;
        case 'f':                   return ACTION_FLAG;
        case 'c': case NCKEY_ENTER:
        case '\n': case '\r':       return ACTION_CLEAR;
        case 'q': case NCKEY_ESC:   return ACTION_QUIT;
        default:                    return ACTION_NONE;
    }
}

/** 
 * @brief Just takes in the mouse input and correlates it to board square.
 * If outside of board does nothing.
 */
static bool mouse_to_board(int term_row, int term_col,
                            int board_rows, int board_cols,
                            int *out_row, int *out_col) {
    int row = term_row - BOARD_ORIGIN_ROW;
    int col = term_col / CELL_WIDTH;

    if (row < 0 || row >= board_rows) return false;
    if (col < 0 || col >= board_cols) return false;

    *out_row = row;
    *out_col = col;
    return true;
}

/**
 * Waits for input from player, either mouse or keys.
 */
InputEvent input_get(struct notcurses *nc, int board_rows, int board_cols) {
    InputEvent ev = { .action = ACTION_NONE, .row = -1, .col = -1 };

    struct ncinput ni;
    notcurses_get_blocking(nc, &ni);

    if (ni.evtype == NCTYPE_PRESS &&
        (ni.id == NCKEY_BUTTON1 || ni.id == NCKEY_BUTTON3)) {

        int br, bc;
        bool on_board = mouse_to_board((int)ni.y, (int)ni.x,
                                       board_rows, board_cols,
                                       &br, &bc);
        if (on_board) {
            ev.row    = br;
            ev.col    = bc;
            ev.action = (ni.id == NCKEY_BUTTON1) ? ACTION_CLEAR : ACTION_FLAG;
        }
        return ev;
    }

    ev.action = key_to_action(&ni);
    return ev;
}

/**
*Checks to make sure the mouse works. If it doesnt then the arrow keys work.
 */
bool input_enable_mouse(struct notcurses *nc) {
    return notcurses_mice_enable(nc, NCMICE_BUTTON_EVENT) == 0;
}

void input_disable_mouse(struct notcurses *nc) {
    notcurses_mice_disable(nc);
}
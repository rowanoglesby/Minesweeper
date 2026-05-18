/** 
 * Rowan Oglesby
 * 5-13-2026
 * 900367291
 * 
 * @file game.c
 * @brief this code is for the game. It sets up the game,
 * deals with the input, then decides when someone won or not.
 */

#include "Extra_Definitions.h"
#include <stdlib.h>
#include <string.h>
/** 
 * @brief Clears everything form old game and sets up board.
 */
void game_init(GameState *gs, struct notcurses *nc, const GameConfig *cfg) {
    memset(gs, 0, sizeof(*gs));

    gs->cfg        = *cfg;
    gs->cursor_row = 0;
    gs->cursor_col = 0;
    gs->running    = true;

    board_initial(&gs->board, cfg->rows, cfg->cols, cfg->mines);
    ui_init(&gs->ui, nc);
}


/** 
 * Runs the game
 */
void game_run(struct notcurses *nc, const GameConfig *cfg) {
    GameState gs;
    game_init(&gs, nc, cfg);

    bool mouse_on = input_enable_mouse(nc);
    (void)mouse_on;

    ui_draw(&gs.ui, &gs.board, &gs.cfg, gs.cursor_row, gs.cursor_col);

    while (gs.running) {

        InputEvent ev = input_get(nc, gs.board.rows, gs.board.cols);

        game_handle_input(&gs, ev);

        if (!gs.running) break;

        ui_draw(&gs.ui, &gs.board, &gs.cfg, gs.cursor_row, gs.cursor_col);

        if (gs.board.game_over) {
            ui_draw_game_over(&gs.ui, &gs.board);
            break;
        }

        if (gs.board.won) {
            ui_draw_win(&gs.ui, &gs.board);
            break;
        }
    }

    input_disable_mouse(nc);
}

/*
 * Takes an input and runs it ot update board
 */
void game_handle_input(GameState *gs, InputEvent ev) {
    int target_row = gs->cursor_row;
    int target_col = gs->cursor_col;

    /** For Mouse Click */
    if (ev.row >= 0 && ev.col >= 0) {
        target_row     = ev.row;
        target_col     = ev.col;
        gs->cursor_row = target_row;
        gs->cursor_col = target_col;
    }

    switch (ev.action) {

        case ACTION_UP:
            gs->cursor_row--;
            game_clamp_cursor(gs);
            break;

        case ACTION_DOWN:
            gs->cursor_row++;
            game_clamp_cursor(gs);
            break;

        case ACTION_LEFT:
            gs->cursor_col--;
            game_clamp_cursor(gs);
            break;

        case ACTION_RIGHT:
            gs->cursor_col++;
            game_clamp_cursor(gs);
            break;

        case ACTION_CLEAR:
            /* FOr variants to reveal ie liar*/
            variant_reveal(&gs->board, &gs->cfg, target_row, target_col);
            break;

        case ACTION_FLAG:
            /** Flag on and off*/
            board_flag(&gs->board, target_row, target_col);
            break;

        case ACTION_QUIT:
            gs->running = false;
            break;

        case ACTION_NONE:
        default:
            break;
    }
}

void game_clamp_cursor(GameState *gs) {
    if (gs->cursor_row < 0)               gs->cursor_row = 0;
    if (gs->cursor_row >= gs->board.rows) gs->cursor_row = gs->board.rows - 1;
    if (gs->cursor_col < 0)               gs->cursor_col = 0;
    if (gs->cursor_col >= gs->board.cols) gs->cursor_col = gs->board.cols - 1;
}
/**
 * Rowan Oglesby
 * 5-13-2026
 * 900367291
 * 
 * @file Extra_Definitions.h
 * @brief Defines all missing variables from my last code.
 */

#ifndef EXTRA_DEFINITIONS_H
#define EXTRA_DEFINITIONS_H


#include <stdbool.h>
#include <notcurses/notcurses.h>
#include "board.h"

/**
 * @brief Sets up for board size
 */
typedef enum{
    SIZE_SMALL   = 0,
    SIZE_MEDIUM  = 1,
    SIZE_LARGE   = 2,
    SIZE_CUSTOM  = 3
} BoardSizeMode;

/**
 * @brief Sets up for variants.
 */
typedef enum {
    VARIANT_NORMAL       = 0,
    VARIANT_CHECKERBOARD = 1,
    VARIANT_LIAR         = 2
} GameVariant;

typedef struct {
    int rows;
    int cols;
    int mines;
    GameVariant variant;
} GameConfig;
GameConfig config_prompt(struct notcurses *nc);
const char *config_variant_name(GameVariant v);
void       config_validate(GameConfig *cfg, struct notcurses *nc);

/**
 * @brief For UI
 */
typedef struct {
    struct notcurses *nc;
    struct ncplane  *std;
} UI;

/* Forward declarations */
typedef struct Board Board;

void ui_init(UI *ui, struct notcurses *nc);
void ui_draw(UI *ui, const Board *b, const GameConfig *cfg, int cursor_row, int cursor_col);
void ui_draw_status(UI *ui, const Board *b, const GameConfig *cfg);
void ui_draw_cell(UI *ui, const Board *b,const GameConfig *cfg, int row, int col, bool is_cursor);
void ui_draw_game_over(UI *ui, Board *b);
void ui_draw_win(UI *ui, const Board *b);
void ui_draw_message_box(UI *ui, const char *title, const char *subtitle, int r, int g, int b_col);



/**
 * @brief A lot of variables for gamestate
 */

typedef struct {
   Board     board;
   GameConfig cfg;
   UI          ui;
   int cursor_row;
   int cursor_col;
   bool   running;
} GameState;

/**
 * @brief for the actions in input
 */
typedef enum {
    ACTION_NONE = 0,
    ACTION_UP = 1,
    ACTION_DOWN = 2,
    ACTION_LEFT = 3,
    ACTION_RIGHT = 4,
    ACTION_CLEAR = 5,
    ACTION_FLAG = 6,
    ACTION_QUIT = 7,
} Action;

/**
 * @brief for board controls for mouse click.
 */
typedef struct {
    Action action;
    int row;
    int col;
} InputEvent;

/**
 * @brief For GameState, like runtime stuff.
 */
void game_init(GameState *gs, struct notcurses *nc, const GameConfig *cfg);
void game_run(struct notcurses *nc, const GameConfig *cfg);
void game_handle_input(GameState *gs, InputEvent ev);
void game_clamp_cursor(GameState *gs);


InputEvent input_get(struct notcurses *nc, int board_rows, int board_cols);
bool input_enable_mouse(struct notcurses *nc);
void input_disable_mouse(struct notcurses *nc);


void variant_reveal(Board *b, const GameConfig *cfg, int row, int col);
int variant_liar_count(const Board *b, int row, int col);
#endif

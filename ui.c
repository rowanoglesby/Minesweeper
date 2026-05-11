/** 
* @brief this code draws everything to the terminal. Notcurses library was used here.
* the screen should display the ammount of mies left, flags placed, and how to use controls,
*along with the board.
 */

#include "ui.h"
#include "variants.h" 
#include <stdio.h>
#include <string.h>

/** @brief defines the board start and cell width. */
#define BOARD_ORIGIN_ROW 2
#define CELL_WIDTH 2

/** All different characters for cells*/
#define GLYPH_HIDDEN  "░░"   /** Closed tile*/
#define GLYPH_FLAG    "⚑ "   /** flagged */
#define GLYPH_MINE    "✸ "   /**mine revealed */
#define GLYPH_EMPTY   "  "   /** Open revealed tile */

/*
 * Colors for the numbers 1-8.
 * Index 0 is unused because we never draw the number 0 as a digit.
 * Each row is { red, green, blue }.
 * These match the classic Windows Minesweeper colors.
 */
static const uint8_t NUMBER_COLORS[9][3] = {
    {   0,   0,   0 },  /* 0 - unused */
    {   0, 100, 255 },  /* 1 - blue */
    {   0, 160,   0 },  /* 2 - green */
    { 220,  30,  30 },  /* 3 - red */
    {   0,   0, 160 },  /* 4 - dark blue */
    { 160,   0,   0 },  /* 5 - dark red */
    {   0, 180, 180 },  /* 6 - cyan */
    { 140,   0, 140 },  /* 7 - purple */
    {  80,  80,  80 },  /* 8 - gray */
};

/** Clears the plane to restart */
void ui_init(UI *ui, struct notcurses *nc) {
    ui->nc  = nc;
    ui->std = notcurses_stdplane(nc);
    ncplane_erase(ui->std);
}

/** Redraws the plane */
void ui_draw(UI *ui, const Board *b, const GameConfig *cfg,
             int cursor_row, int cursor_col) {
    ncplane_erase(ui->std);

    ui_draw_status(ui, b, cfg);

    for (int r = 0; r < b->rows; r++) {
        for (int c = 0; c < b->cols; c++) {
            bool is_cursor = (r == cursor_row && c == cursor_col);
            ui_draw_cell(ui, b, cfg, r, c, is_cursor);
        }
    }

    /** goes to terminal */
    notcurses_render(ui->nc);
}

/** 
* @brief draws the gamemode, ammount of flags/ mines left, controlls.
*/
void ui_draw_status(UI *ui, const Board *b, const GameConfig *cfg) {
    struct ncplane *std = ui->std;
    char buf[128];

    /**Mines left*/
    int remaining = b->total_mines - b->flags_placed;
    ncplane_set_fg_rgb8(std, 255, 220, 0);
    snprintf(buf, sizeof(buf), " ⚑ Mines: %d ", remaining);
    ncplane_putstr_yx(std, 0, 0, buf);

    /**Gamemode*/
    ncplane_set_fg_rgb8(std, 100, 200, 255);
    snprintf(buf, sizeof(buf), "[ %s ]", config_variant_name(cfg->variant));
    unsigned int term_rows, term_cols;
    notcurses_term_dim_yx(ui->nc, &term_rows, &term_cols);
    int center_x = ((int)term_cols - (int)strlen(buf)) / 2;
    if (center_x < 0) center_x = 0;
    ncplane_putstr_yx(std, 0, center_x, buf);

    /**placed flags*/
    ncplane_set_fg_rgb8(std, 180, 180, 180);
    snprintf(buf, sizeof(buf), "%d / %d flags ", b->flags_placed, b->total_mines);
    int right_x = (int)term_cols - (int)strlen(buf);
    if (right_x < 0) right_x = 0;
    ncplane_putstr_yx(std, 0, right_x, buf);

    /**controls*/
    ncplane_set_fg_rgb8(std, 100, 100, 100);
    ncplane_putstr_yx(std, 1, 0, " wasd: move   f: flag   c: clear   q: quit");
}

/**
* @brief Draws the cells, including highlighted cell, 
*flagged cells, open cells, numbered cells, and mine cells
 */
void ui_draw_cell(UI *ui, const Board *b, const GameConfig *cfg,
                  int row, int col, bool is_cursor) {
    struct ncplane *std = ui->std;

    /**Board to screen conversion*/
    int screen_row = BOARD_ORIGIN_ROW + row;
    int screen_col = col * CELL_WIDTH;

    const Cell *cell = &b->cells[row][col];

    /**Shows tile the player is on by highlighting it*/
    if (is_cursor) {
        ncplane_set_bg_rgb8(std, 60, 60, 120);
        ncplane_set_fg_rgb8(std, 255, 255, 255);
    } else {
        ncplane_set_bg_default(std);
    }

    /**Flaged not revealed tiles*/
    if (cell->is_flagged && !cell->is_revealed) {
        if (!is_cursor) ncplane_set_bg_rgb8(std, 40, 40, 40);
        ncplane_set_fg_rgb8(std, 255, 180, 0);
        ncplane_putstr_yx(std, screen_row, screen_col, GLYPH_FLAG);

    /**Not revealed tiles*/
    } else if (!cell->is_revealed) {
        if (!is_cursor) ncplane_set_bg_rgb8(std, 50, 50, 50);
        ncplane_set_fg_rgb8(std, 120, 120, 120);
        ncplane_putstr_yx(std, screen_row, screen_col, GLYPH_HIDDEN);

    /**Shows mines after game is over*/
    } else if (cell->has_mine) {
        if (!is_cursor) ncplane_set_bg_rgb8(std, 80, 0, 0);
        ncplane_set_fg_rgb8(std, 255, 60, 60);
        ncplane_putstr_yx(std, screen_row, screen_col, GLYPH_MINE);

    /**Open tile with no close mines*/
    } else if (cell->adj_mines == 0) {
        if (!is_cursor) ncplane_set_bg_rgb8(std, 25, 25, 25);
        ncplane_set_fg_rgb8(std, 60, 60, 60);
        ncplane_putstr_yx(std, screen_row, screen_col, GLYPH_EMPTY);

    /**Cell with neigboring mine count*/
    } else {
        /** 
        * @brief now liar mode works and numbers are not corect and actually lie.
         */
        int display_count;
        if (cfg->variant == VARIANT_LIAR) {
            display_count = variant_liar_count(b, row, col);
        } else {
            display_count = cell->adj_mines;
        }

        /**Check to make sure we dont crash if 9 is selected, or -1*/
        if (display_count < 0) display_count = 0;
        if (display_count > 8) display_count = 8;

        /**Draw open for 0*/
        if (display_count == 0) {
            if (!is_cursor) ncplane_set_bg_rgb8(std, 25, 25, 25);
            ncplane_set_fg_rgb8(std, 60, 60, 60);
            ncplane_putstr_yx(std, screen_row, screen_col, GLYPH_EMPTY);
        } else {
            if (!is_cursor) ncplane_set_bg_rgb8(std, 25, 25, 25);
            ncplane_set_fg_rgb8(std,
                                NUMBER_COLORS[display_count][0],
                                NUMBER_COLORS[display_count][1],
                                NUMBER_COLORS[display_count][2]);
            char glyph[4];
            snprintf(glyph, sizeof(glyph), "%d ", display_count);
            ncplane_putstr_yx(std, screen_row, screen_col, glyph);
        }
    }

    ncplane_set_bg_default(std);
}

/**
* @brief makes sure game doesnt break when trying to quit.
 */
void ui_draw_game_over(UI *ui, const Board *b) {
    ncplane_erase(ui->std);

    GameConfig dummy_cfg = {0};  /** for draw- variants or not*/

    for (int r = 0; r < b->rows; r++) {
        for (int c = 0; c < b->cols; c++) {
            Cell original = b->cells[r][c];

            Cell temp = original;
            if (temp.has_mine) temp.is_revealed = true;

            b->cells[r][c] = temp;
            ui_draw_cell(ui, b, &dummy_cfg, r, c, false);
            b->cells[r][c] = original;  /**put cell back*/
        }
    }

    ui_draw_message_box(ui,
                        "  Game Lost!  ",
                        "  Press any key to return to menu  ",
                        220, 50, 50);

    notcurses_render(ui->nc);

    struct ncinput ni;
    notcurses_get_blocking(ui->nc, &ni);
}
/**
* @brief shows when player wins.
 */
void ui_draw_win(UI *ui, const Board *b) {
    ncplane_erase(ui->std);

    GameConfig dummy_cfg = {0};
    for (int r = 0; r < b->rows; r++)
        for (int c = 0; c < b->cols; c++)
            ui_draw_cell(ui, b, &dummy_cfg, r, c, false);

    ui_draw_message_box(ui,
                        "  Winner!  ",
                        "  Press any key to return to menu  ",
                        80, 220, 80);

    notcurses_render(ui->nc);

    struct ncinput ni;
    notcurses_get_blocking(ui->nc, &ni);
}

/**
* @brief displays a centered text for draws and losses.
 */
void ui_draw_message_box(UI *ui, const char *title,
                         const char *subtitle,
                         int r, int g, int b_col) {
    struct ncplane *std = ui->std;

    /**How big is the terminal*/
    unsigned int term_rows, term_cols;
    notcurses_term_dim_yx(ui->nc, &term_rows, &term_cols);

    /**Boarder box needs to be bigger than the text to avoid the text going over it*/
    int box_w   = (int)strlen(subtitle) + 4;
    int box_h   = 5;
    int box_col = ((int)term_cols - box_w) / 2;
    int box_row = ((int)term_rows - box_h) / 2;
    if (box_col < 0) box_col = 0;
    if (box_row < 0) box_row = 0;

    /**Box color*/
    ncplane_set_bg_rgb8(std, 20, 20, 30);
    for (int row = box_row; row < box_row + box_h; row++) {
        for (int col = box_col; col < box_col + box_w; col++) {
            ncplane_putstr_yx(std, row, col, " ");
        }
    }

    /**For the boarder*/
    ncplane_set_fg_rgb8(std, 100, 100, 160);
    ncplane_set_bg_rgb8(std, 20, 20, 30);

    ncplane_putstr_yx(std, box_row,           box_col,             "╔");
    ncplane_putstr_yx(std, box_row + box_h-1, box_col,             "╚");
    ncplane_putstr_yx(std, box_row,           box_col + box_w - 1, "╗");
    ncplane_putstr_yx(std, box_row + box_h-1, box_col + box_w - 1, "╝");

    for (int col = box_col + 1; col < box_col + box_w - 1; col++) {
        ncplane_putstr_yx(std, box_row,           col, "═");
        ncplane_putstr_yx(std, box_row + box_h-1, col, "═");
    }

    for (int row = box_row + 1; row < box_row + box_h - 1; row++) {
        ncplane_putstr_yx(std, row, box_col,             "║");
        ncplane_putstr_yx(std, row, box_col + box_w - 1, "║");
    }

    /**Tile on second row*/
    int title_col = box_col + (box_w - (int)strlen(title)) / 2;
    if (title_col < box_col + 1) title_col = box_col + 1;
    ncplane_set_fg_rgb8(std, (uint8_t)r, (uint8_t)g, (uint8_t)b_col);
    ncplane_set_bg_rgb8(std, 20, 20, 30);
    ncplane_putstr_yx(std, box_row + 1, title_col, title);

    /**Text on the 4th row*/
    int sub_col = box_col + (box_w - (int)strlen(subtitle)) / 2;
    if (sub_col < box_col + 1) sub_col = box_col + 1;
    ncplane_set_fg_rgb8(std, 160, 160, 160);
    ncplane_putstr_yx(std, box_row + 3, sub_col, subtitle);
}
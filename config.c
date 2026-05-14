/** 
 * Rowan Oglesby
 * 5-13-2026
 * 900367291
 * 
 * @file cofig.c
 * @brief This is the homepage for the game and sets up the place wherre players input gamemodes, etc.
 */

#include "Extra_Definitions.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/** 
 * Has the three gamemode board sizes.
 */
static const int PRESETS[3][3] = {
    {  9,  9,  10 },  /* Beginner     */
    { 16, 16,  40 },  /* Intermediate */
    { 30, 16,  99 },  /* Expert       */
};

/** Shows the three options */
static const char *SIZE_LABELS[] = {
    " 9x9   - 10 mines  (Beginner)    ",
    "16x16  - 40 mines  (Intermediate)",
    "30x16  - 99 mines  (Expert)      ",
    "Custom - You choose               ",
};

/** Shows the variant options */
static const char *VARIANT_LABELS[] = {
    "Normal       - Classic Minesweeper              ",
    "Checkerboard - Some mines count as 2            ",
    "Liar         - Counts are off by +/-1           ",
};

/** Clears the screen */
static void clear_screen(struct notcurses *nc) {
    struct ncplane *std = notcurses_stdplane(nc);
    ncplane_erase(std);
}

/* Shows the title */
static void draw_banner(struct ncplane *std) {
    ncplane_set_fg_rgb8(std, 255, 200, 0);
    "MINESWEEPER";
    ncplane_putstr_yx(std, 7, 2, "  A terminal Minesweeper — use wasd to move, f to flag, c to clear");
}

/** 
 * @brief shows the main menu where player can scroll through options.
 */
static int run_menu(struct notcurses *nc, const char *title,
                    const char **items, int count, int start_y) {
    struct ncplane *std = notcurses_stdplane(nc);
    int selected = 0;

    while (1) {
        ncplane_set_fg_rgb8(std, 100, 200, 255);
        ncplane_putstr_yx(std, start_y, 4, title);

        for (int i = 0; i < count; i++) {
            if (i == selected) {
                ncplane_set_fg_rgb8(std, 255, 220, 0);
                ncplane_putstr_yx(std, start_y + 1 + i, 6, "> ");
            } else {
                ncplane_set_fg_rgb8(std, 180, 180, 180);
                ncplane_putstr_yx(std, start_y + 1 + i, 6, "  ");
            }
            ncplane_putstr(std, items[i]);
        }

        notcurses_render(nc);

        /** wait for input, */
        struct ncinput ni;
        notcurses_get_blocking(nc, &ni);
        if (ni.id == 'w' || ni.id == NCKEY_UP) {
            selected = (selected - 1 + count) % count;
        } else if (ni.id == 's' || ni.id == NCKEY_DOWN) {
            selected = (selected + 1) % count;
        } else if (ni.id == NCKEY_ENTER || ni.id == '\n' || ni.id == '\r') {
            break;
        }
    }

    return selected;
}

/** 
 * @brief Makes a player select a value between min and max.
 */
static int prompt_int(struct notcurses *nc, const char *prompt,
                      int row, int min, int max) {
    struct ncplane *std = notcurses_stdplane(nc);
    char buf[16] = {0};
    int  len = 0;

    while (1) {
        ncplane_set_fg_rgb8(std, 100, 200, 255);
        ncplane_putstr_yx(std, row, 4, prompt);
        ncplane_set_fg_rgb8(std, 255, 255, 255);
        ncplane_putstr_yx(std, row, (int)(4 + strlen(prompt)), buf);
        ncplane_putstr(std, "   ");
        notcurses_render(nc);

        struct ncinput ni;
        notcurses_get_blocking(nc, &ni);

        if (ni.id >= '0' && ni.id <= '9' && len < 4) {
            buf[len++] = (char)ni.id;
            buf[len]   = '\0';

        } else if ((ni.id == NCKEY_BACKSPACE || ni.id == 127) && len > 0) {
            buf[--len] = '\0';

        } else if (ni.id == NCKEY_ENTER || ni.id == '\n') {
            if (len == 0) continue;

            int val = atoi(buf);
            if (val >= min && val <= max) return val;

            /** Shows error if the number is outside limits */
            ncplane_set_fg_rgb8(std, 255, 80, 80);
            ncplane_putstr_yx(std, row + 1, 4, "  Out of range — try again.  ");
            notcurses_render(nc);
            memset(buf, 0, sizeof(buf));
            len = 0;
        }
    }
}

/** 
 * @brief pulls it all together in steps, 
 calculating board size, gamemode, then making sure this is ok
 */
GameConfig config_prompt(struct notcurses *nc) {
    GameConfig cfg = {0};

    clear_screen(nc);
    struct ncplane *std = notcurses_stdplane(nc);
    draw_banner(std);

    /**Board Size */
    int size_choice = run_menu(nc, "Select board size:", SIZE_LABELS, 4, 9);

    if (size_choice < 3) {
        cfg.rows  = PRESETS[size_choice][0];
        cfg.cols  = PRESETS[size_choice][1];
        cfg.mines = PRESETS[size_choice][2];
    } else {
/** Custom Board */
        clear_screen(nc);
        draw_banner(std);
        ncplane_set_fg_rgb8(std, 100, 200, 255);
        ncplane_putstr_yx(std, 9, 4, "Custom board setup:");

        /** Determines the max board we can have */
        unsigned int term_rows, term_cols;
        notcurses_term_dim_yx(nc, &term_rows, &term_cols);
        int max_rows = (int)term_rows - 6;
        int max_cols = (int)term_cols / 2;

        cfg.rows  = prompt_int(nc, "  Rows  (1 - max): ", 11, 1, max_rows);
        cfg.cols  = prompt_int(nc, "  Cols  (1 - max): ", 12, 1, max_cols);

        /** mine cells cannot take up the entire board, so 1- board # */
        int max_mines = cfg.rows * cfg.cols - 1;
        if (max_mines > 1000) max_mines = 1000; 
        cfg.mines = prompt_int(nc, "  Mines (1-1000):  ", 13, 1, max_mines);
    }

    /** Game variations */
    clear_screen(nc);
    draw_banner(std);
    int variant_choice = run_menu(nc, "Select game variant:", VARIANT_LABELS, 3, 9);
    cfg.variant = (GameVariant)variant_choice;  

    config_validate(&cfg, nc);

    return cfg;
}

/** returns the chosen value */
const char *config_variant_name(GameVariant v) {
    switch (v) {
        case VARIANT_NORMAL:       return "Normal";
        case VARIANT_CHECKERBOARD: return "Checkerboard";
        case VARIANT_LIAR:         return "Liar";
        default:                   return "Unknown";
    }
}

/** 
 *Makes sure nothing crashes by overloading.
 */
void config_validate(GameConfig *cfg, struct notcurses *nc) {
    unsigned int term_rows, term_cols;
    notcurses_term_dim_yx(nc, &term_rows, &term_cols);

    int max_rows = (int)term_rows - 6;
    int max_cols = (int)term_cols / 2;

    if (cfg->rows < 1)        cfg->rows = 1;
    if (cfg->rows > max_rows) cfg->rows = max_rows;
    if (cfg->cols < 1)        cfg->cols = 1;
    if (cfg->cols > max_cols) cfg->cols = max_cols;

    int max_mines = cfg->rows * cfg->cols - 1;
    if (max_mines > 1000) max_mines = 1000;

    if (cfg->mines < 1)         cfg->mines = 1;
    if (cfg->mines > max_mines) cfg->mines = max_mines;
}
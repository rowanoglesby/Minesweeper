/** 
 * Rowan Oglesby
 * 5-13-2026
 * 900367291
 * 
 * @file variants.c
 * @brief For the three variants in the game
 */

#include "Extra_Definitions.h"
#include "board.h"
#include <stdlib.h>

/** Offset so key input works. */
static const int DR[] = {-1, -1, -1,  0,  0,  1,  1,  1};
static const int DC[] = {-1,  0,  1, -1,  1, -1,  0,  1};

/**make it checkered*/
bool variant_is_dark_tile(int row, int col) {
    return (row + col) % 2 != 0;
}

/**
 * @brief for checkerboard makes mines on dark worth more than light spaces, 
 *reorgainizing mine placement
 */
void variant_checkerboard_calc(Board *b) {
    for (int r = 0; r < b->rows; r++) {
        for (int c = 0; c < b->cols; c++) {

            int count = 0;

            /**Look at 8 surrouding*/
            for (int d = 0; d < 8; d++) {
                int nr = r + DR[d];
                int nc = c + DC[d];

                /**Check for out of bounds */
                if (!board_in_bounds(b, nr, nc)) continue;


                /**Makes sure all mines are not next to eachother*/
                if (!b->cells[nr][nc].has_mine) continue;

                /**Dark is two, light is one*/
                if (variant_is_dark_tile(nr, nc)) {
                    count += 2;
                } else {
                    count += 1;
                }
            }

            /**Not above 8*/
            if (count > 8) count = 8;
            b->cells[r][c].adj_mines = count;
        }
    }
}

/** 
 * @brief For liar gamemode. Should offset tile +-1 and stay that way for the game.
 */
int variant_liar_count(const Board *b, int row, int col) {
    int true_count = b->cells[row][col].adj_mines;

    /**open tile needs to stay open*/
    if (true_count == 0) return 0;

    /**Random up or down one */
    int hash   = (row * 7 + col * 13);
    int offset = (hash % 2 != 0) ? 1 : -1;
    int result = true_count + offset;

    /**Make sure the number is between 0-8*/
    if (result < 0) result = 0;
    if (result > 8) result = 8;

    if (result == true_count && true_count > 1) {
        result = true_count - 1;
    }
    return result;
}

/**
 * @brief changes the revealing tiles for checkerboard, but the rest stays untouched.
 */
void variant_reveal(Board *b, const GameConfig *cfg, int row, int col) {
    /**Check for the first move*/
    bool was_first_move = b->first_move;

    board_reveal(b, row, col);

    /**Changes the reveals for checkerboard */
    if (was_first_move && cfg->variant == VARIANT_CHECKERBOARD) {
        variant_checkerboard_calc(b);
    }
}
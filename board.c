
/**
 * Rowan Oglesby
 * 5-17-2026
 * 900367291
 * 
 * @file board.c
 * @brief Drawing the board 
 */

#include "board.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

/** @brief diretion offsetters */
static const int DR[] = {-1, -1, -1, 0, 0, 1, 1, 1};
static const int DC[] = {-1, 0, 1, -1, 1, -1, 0, 1};

/** @brief Starts board with dimensions and mines */

void board_initial(Board *b, int rows, int cols, int mines) {
    memset(b, 0, sizeof(*b));
    b->rows        = rows;
    b->cols        = cols;
    b->mines_total = mines;
    b->first_move  = true;
}
/** @brief First click is safe but randomizes mines around this */
void board_place_mines(Board *b, int safe_row, int safe_col) {
static bool seeded = false;
if (!seeded) {
    srand((unsigned)time(NULL));
    seeded = true;
}
int placed = 0;
while (placed < b->mines_total) {
    int r = rand() % b->rows;
    int c = rand() % b->cols;

    if ((r == safe_row && c == safe_col) || b->cells[r][c].Mine)
        continue;
    b->cells[r][c].Mine = true;
    placed++;
}
board_adj(b);
}

/** @brief calculates numbers on squares, or adj mines */
void board_adj(Board *b) {
    for (int r = 0; r < b->rows; r++) {
        for (int c = 0; c < b->cols; c++) {
            int count = 0;
            for (int d = 0; d < 8; d++){
                int nr = r + DR[d];
                int nc = c + DC[d];
                if (board_bounds_check(b, nr, nc) && b->cells[nr][nc].Mine) 
                count++;   
            }
            b->cells[r][c].adj_mines = count;
        }
    }
}
/** @brief clears non ajacent tiles all at once */
void board_reveal(Board *b, int row, int col) {
    if (!board_bounds_check(b, row, col))
    return;
Cell *cell = &b->cells[row][col];

if (cell->empty || cell->flagged)
return;
if (b->first_move){
    board_place_mines(b, row, col);
    b->first_move = false;
}
cell->empty = true;
if (cell->Mine) {
    b->game_over = true;
    return;
}
b->open_cell++;
if (cell->adj_mines == 0){
    for (int d = 0; d < 8; d++) {
        board_reveal(b, row + DR[d], col + DC[d]);
    }
}
board_win_condition(b);
}
/** @brief makes flag on or off */
void board_flag(Board *b, int row, int col) {
    if (!board_bounds_check(b, row, col))
    return;
Cell *cell = &b->cells[row][col];
if (cell->empty)
return;
cell->flagged = !cell->flagged;
b->flags_placed += cell->flagged ? 1 : -1;
}
/** @brief checks win condition */
void board_win_condition(Board *b) {
    int safe_cells = (b->rows * b->cols) - b->mines_total;
    if (b->open_cell >= safe_cells)
    b->won = true;
}
/** @brief Returns num of unflagged mines */
int board_remaining_mines(const Board *b) {
    return b->mines_total - b->flags_placed;
}
/** @brief check for if row and col are inbounds */
bool board_bounds_check(const Board *b, int row, int col) {
    return row >= 0 && row < b->rows && col >= 0 && col < b->cols;
}
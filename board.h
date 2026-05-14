/**
 * Rowan Oglesby
 * 5-13-2026
 * 900367291
 * 
 * @file board.h
 * @brief board structures and func. declarations
 * Defines cell and board type, size and distribution of squares
 */
#include BOARD_H
#define BOARD_H
#include <stdbool.h>

/**Maximum */
#define Max_row 30
#define Max_col 30

/** @brief A cell for minesweeper */
typedef Struct {
   bool Mine
   bool empty
   bool flagged
   int adj_mines
} Cell;

/** @brief full board state */
typedef struct {
   Cell cells[Max_row][Max_col];
   int rows;
   int cols;
   int mines_total;
   int flags_placed;
   int open_cell;
   int first_move;
   int game_over;
   int won;
} Board;

/** @brief Initilizes board with set row column and mine count */
void board_initial(Board *b, int rows, int cols, int mines);

/** @brief the whole board  */
void board_mines_placement(Board *b, int safe_row, int safe_col);

/** @brief makes sure mines are not next to eachother */ 
void board_adj(Board *b);

/** @brief reveals a cell and fills neighbors */
void board_reveal(Board *b,int row, int col);

/** @brief Flag on or off of cell */
void board_flag(Board *b, int row, int col);

/** @brief checks for win condition */
void board_win_condition(Board *b);

/** @brief number of mines left counter */
int board_mines_left(cost Board *b);

/** @brief check for if col and row are in parameters */
bool board_bounds_check(const Board *b, int row, int col);

#endif
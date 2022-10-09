// Assignment 1 20T1 COMP1511: Minesweeper
// minesweeper.c
//
// This program was written by Colin Ngo (z5310745)
// on 22/3/2020 - 27/3/2020
//
// Version 1.0.0 (2020-03-08): Assignment released.
// Version 1.0.1 (2020-03-08): Fix punctuation in comment.
// Version 1.0.2 (2020-03-08): Fix second line of header comment to say minesweeper.c

#include <stdio.h>
#include <stdlib.h>

// Possible square states.
#define VISIBLE_SAFE    0
#define HIDDEN_SAFE     1
#define HIDDEN_MINE     2

// The size of the starting grid.
#define SIZE 8


// The possible command codes.
#define DETECT_ROW              1
#define DETECT_COL              2
#define DETECT_SQUARE           3
#define REVEAL_SQUARE           4
#define GAMEPLAY_MODE           5
#define DEBUG_MODE              6
#define REVEAL_RADIAL           7

#define REVEAL_GRID             3
#define MAX_HINTS               3


void initialise_field(int minefield[SIZE][SIZE]);
void print_debug_minefield(int minefield[SIZE][SIZE]);
void print_gameplay_minefield(int minefield[SIZE][SIZE], int gameWon, int gameLost);
void plant_mines(int minefield[SIZE][SIZE]);
void detect_rows(int minefield[SIZE][SIZE], int hintsUsed, int mode);
void detect_columns(int minefield[SIZE][SIZE], int hintsUsed, int mode);
void detect_squares(int minefield[SIZE][SIZE], int hintsUsed, int mode);
void reveal_square(int minefield[SIZE][SIZE], int gameWon, int gameLost, int mode, int firstTurn);
void reveal_radial(int minefield[SIZE][SIZE], int gameWon, int gameLost, int mode, int firstTurn);
void safeFirstTurn(int minefield[SIZE][SIZE], int firstTurn, int row, int column);


int main(void) {
    int minefield[SIZE][SIZE];
    initialise_field(minefield);
    printf("Welcome to minesweeper!\n");
    printf("How many mines? ");

    // Scan in the pairs of mines and place them on the grid.
    plant_mines(minefield);

    printf("Game Started\n");
    print_debug_minefield(minefield);
    
    // User input determines whether the user wants to use their hints, reveal
    // a square, or to change game modes.
    int userinput = 0;
    
    // Game condition is set, where gameWon and gameLost = 0 means that the game 
    // is still continued, If one of the ints change to 1, that implies that the 
    // game is either won or lost.
    int gameWon = 0;
    int gameLost = 0;
    
    // This int determines if the game is played in gameplay mode, or debug mode, where
    // 0 = debug, and 1 = gameplay.
    int mode = 0;
    
    // This int sets the hints used = 0. When one of the detects are used, it will
    // add to the hint counter.
    int hintsUsed = 0;
    
    // This int tracks if the user is in his first turn or not, where firstTurn = 0 means
    // it is in first turn. 
    int firstTurn = 0;
    
    while (scanf("%d", &userinput) == 1 && gameLost == 0 && gameWon == 0) {

        if (userinput == DETECT_ROW) { 
            detect_rows(minefield, hintsUsed, mode);
            hintsUsed++;
        } else if (userinput == DETECT_COL) {
            detect_columns(minefield, hintsUsed, mode);
            hintsUsed++;
        } else if (userinput == DETECT_SQUARE) {
            detect_squares(minefield, hintsUsed, mode);
            hintsUsed++;        
        } else if (userinput == REVEAL_SQUARE) {
            reveal_square(minefield, gameWon, gameLost, mode, firstTurn);
            firstTurn++;
        } else if (userinput == GAMEPLAY_MODE) {
            printf("Gameplay mode activated\n");
            mode = 1;
            print_gameplay_minefield(minefield, gameWon, gameLost);
        } else if (userinput == DEBUG_MODE) {
            printf("Debug mode activated\n");
                mode = 0;
                print_debug_minefield(minefield);
        } else if (userinput == REVEAL_RADIAL) {
            reveal_radial(minefield, gameWon, gameLost, mode, firstTurn);
            firstTurn++;
        }    
    }
      
        

    // TODO: Scan in commands to play the game until the game ends.
    // A game ends when the player wins, loses, or enters EOF (Ctrl+D).
    // You should display the minefield after each command has been processed.      

    return 0;
}

// Set the entire minefield to HIDDEN_SAFE.
void initialise_field(int minefield[SIZE][SIZE]) {
    int i = 0;
    while (i < SIZE) {
        int j = 0;
        while (j < SIZE) {
            minefield[i][j] = HIDDEN_SAFE;
            j++;
        }
        i++;
    }
}

// Print out the actual values of the minefield.
void print_debug_minefield(int minefield[SIZE][SIZE]) {
    int i = 0;
    while (i < SIZE) {
        int j = 0;
        while (j < SIZE) {
            printf("%d ", minefield[i][j]);
            j++;
        }
        printf("\n");
        i++;
    }
}

// Prints out the field in gameplay mode.
void print_gameplay_minefield(int minefield[SIZE][SIZE], int gameWon, int gameLost) {
   
   // These print out the face when the game begins. 
    if (gameLost == 1) {
        printf("xx\n");
        printf("/\\\n");
    } else {
        printf("..\n");
        printf("\\/\n");
    }

    // These print out the x axis of the grid. 
    printf("    00 01 02 03 04 05 06 07\n");
    printf("   -------------------------\n");
    
    // This big while loop prints out the grid itself.
    int i = 0;
    while (i < SIZE) {
        // Using the i counter for the y axis, this prints out the y coordinates
        // of the grid.
        
        printf("0%d |", i);
        int j = 0;
          
        // Using while loops to go through the whole map, if its a HIDDEN_MINE or a
        // HIDDEN_SAFE it will print out a "##". If the game is lost (i.e. gameLost == 1)
        // it will print out a "()" instead through the use of if conditions. 
        
        while (j < SIZE) {
            if (minefield[i][j] == HIDDEN_SAFE || minefield[i][j] == HIDDEN_MINE) {
                if (minefield[i][j] == HIDDEN_MINE && gameLost == 1) {
                    printf("() ");
                } else {
                    printf("## ");
                }
            
            // This else if is used to determine what to print for VISIBLE_SAFE. It is
            // made up of an if and else statement inside, where a while loop is used
            // to count the adjacent mines, and determine whether to print an empty space
            // or print the amount of mines nearby.
                
            } else if (minefield[i][j] == VISIBLE_SAFE) {
                
                int counterX = 0;
                int rowindex = i - 1;
                int minetotal = 0;
                
                // This while function in a while function checks for any mines in a 3 x 3 
                // grid, where it starts from the top left corner of the grid, then scans 
                // through row by row for mines using a sum counter. Through the "minetotal",
                // this will determine which if/else statement the coordinates fit under.     
                           
                while (counterX < REVEAL_GRID) {
                    int counterY = 0;
                    int columnindex = j - 1;
                    
                    while (counterY < REVEAL_GRID) {
                    
                        if (columnindex >= SIZE || rowindex >= SIZE || columnindex < 0 || rowindex < 0) {
                            counterY++; 
                            columnindex++;
                        } else if (minefield[rowindex][columnindex] == HIDDEN_MINE) {
                            minetotal++;
                            counterY++;
                            columnindex++;
                        } else {
                            counterY++;
                            columnindex++;
                        }
                    }
                    rowindex++;
                    counterX++;
                }
                
                if (minetotal > 0) {
                    printf("0%d ", minetotal);
                }
                
                else {
                    printf("   ");
                }
            } 
            j++;
        }
        
        printf("|\n");
        i++;         
    }   
    
    printf("   -------------------------\n");
}


// Asks for number of mines and,
// scans the mines and places them into the field.
void plant_mines(int minefield[SIZE][SIZE]) {
    int mine_number;
    scanf("%d", &mine_number);
    
    printf("Enter pairs:\n");

    int i = 0;
    int mine1;
    int mine2;
    
    while (i < mine_number) {
        scanf("%d %d", &mine1, &mine2);    
        
        // The "if" and "else if" statements account for invalid input,
        // and the program should not attempt to place it.   
         
        if (mine1 >= SIZE || mine2 >= SIZE) {
            i++;
        } else if (mine1 < 0 || mine2 < 0) {
            i++;
        } else {   
            minefield[mine1][mine2] = HIDDEN_MINE;
            i++;
        }
    }
}  

// If user entered value for row, then displays
// if there are any mines in that row.
void detect_rows(int minefield[SIZE][SIZE], int hintsUsed, int mode) {
    int gameWon = 0;
    int gameLost = 0;
    int row;    
    scanf("%d", &row);
    
    if (hintsUsed > 2) {
        printf("Help already used\n");
        print_debug_minefield(minefield);
    }    
    
    else {
    
        int i = 0;
        int sum_of_mines = 0;
        
        // This loop allows for the program to check whether each space of the row
        // has a mine or not. If so, it will add onto the sum counter.
        while (i < SIZE) {        
            if (minefield[row][i] == HIDDEN_MINE) {
                sum_of_mines++;
            } 
            i++;
        }
        
        printf("There are %d mine(s) in row %d\n", sum_of_mines, row);
        
        // A if statement is used to check on the mode of the game, and print
        // either in debug mode or gameplay mode. This will be seen frequently.
        
        if (mode == 0) {
            print_debug_minefield(minefield); 
        } else {
            print_gameplay_minefield(minefield, gameWon, gameLost);
        }   

    }          
}
        
// If user entered value for column, then displays
// if there are any mines in that column.           
void detect_columns(int minefield[SIZE][SIZE], int hintsUsed, int mode) {
    int gameWon = 0;
    int gameLost = 0;
    int column;    
    scanf("%d", &column);
    
    if (hintsUsed > 2) {
        printf("Help already used\n");
        print_debug_minefield(minefield);
    }
    
    else {
        int i = 0;
        int sum_of_mines = 0;
        
        // This loop allows for the program to check whether each space of the column
        // has a mine or not. If so, it will add onto the sum counter.
        while (i < SIZE) {        
            if (minefield[i][column] == HIDDEN_MINE) {
                sum_of_mines++;
            }
            i++; 
        }
        
        printf("There are %d mine(s) in column %d\n", sum_of_mines, column);   
        
        if (mode == 0) {
            print_debug_minefield(minefield);
        } else {
            print_gameplay_minefield(minefield, gameWon, gameLost);
        }
    }
}    

// If this user entered value for squares, then program will count the number of
// mines in nxn grid.
void detect_squares(int minefield[SIZE][SIZE], int hintsUsed, int mode) {
    int gameWon = 0;
    int gameLost = 0;
    int row;
    int column;
    int square;
    scanf("%d %d %d", &row, &column, &square);
    
    if (hintsUsed > 2) {
        printf("Help already used\n");
        print_debug_minefield(minefield);
    }
  
    else {
        
        int counterX = 0;
        // This int places the row and column asked into the top right of the box 
        // created by the n x n grid. We only use divide by 2 as int division does not
        // account for remainders.
        int rowindex = row -(square / 2);
        int sum_of_mines = 0;
        
        // This counter within a counter starts from the top right of the n x n grid
        // and scans every sqaure within the grid, adding the bombs into a counter.
        while (counterX < square) {
            int counterY = 0;
            int columnindex = column -(square / 2);        
        
            while (counterY < square) {
                if (columnindex >= SIZE || rowindex >= SIZE || columnindex < 0 || rowindex < 0) {
                    counterY++;
                    columnindex++;
                } else if (minefield[rowindex][columnindex] == HIDDEN_MINE) {
                    counterY++;
                    columnindex++;
                    sum_of_mines++;
                } else {
                    counterY++;
                    columnindex++;
                }
            }
            rowindex++;
            counterX++;
        }
        
        printf("There are %d mine(s) in the square centered at row %d, column %d of size %d\n", sum_of_mines, row, column, square);
        
        if (mode == 0) {
            print_debug_minefield(minefield);
        } else {
            print_gameplay_minefield(minefield, gameWon, gameLost);
        }
    }
}

// This function reveals a square on the minefield, under a set of conditions.
// If it is near an adjacent mine, it does not reveal anything. If it is
// not near a mine, it reveals a 3 x 3 square.
       
void reveal_square(int minefield[SIZE][SIZE], int gameWon, int gameLost, int mode, int firstTurn) {
    int row;
    int column;
    scanf("%d %d", &row, &column);
    
    // This counterX serves as a counter to all the while loops.
    
    int counterX = 0;    

    // This if statement checks whether the user's first turn was on a mine. This
    // will enter the safeFirstTurn function, where the map shifts downwards until
    // the user's input is safe to do so.
    
    if (minefield[row][column] == HIDDEN_MINE && firstTurn == 0) {
        safeFirstTurn(minefield, firstTurn, row, column);
        firstTurn++;
    }            
   
    // This if statement is for when the user inputs a coordinate where a
    // mine is, the game will end through placing gameLost = 1, and ending the loop.
    
    if (minefield[row][column] == HIDDEN_MINE && firstTurn != 0) {
        printf("Game over\n");
        gameLost = 1;
        
        if (mode == 0) {
            print_debug_minefield(minefield);
        } else {
            print_gameplay_minefield(minefield, gameWon, gameLost);
        }

    }     
    
    // This big else if statement holds whenever the user inputs a coordinate
    // that is valid. It contains a while loop to scan for mines, then an if and
    // else statement for the two scenarios that is possible. More info below.
    
    else if (minefield[row][column] == HIDDEN_SAFE) {    
        int minetotal = 0;
        int rowindex = row - 1;
        counterX = 0;
        
        // This while function in a while function checks for any mines in a 3 x 3 
        // grid, where it starts from the top left corner of the grid, then scans 
        // through row by rowfor mines using a sum counter. Through the "minetotal",
        // this will determine which if/else statement the coordinates fit under.
        
        while (counterX < REVEAL_GRID) {
            int counterY = 0;
            int columnindex = column - 1;
                   
            while (counterY < REVEAL_GRID) {
                // This if statement will be used often, as when the column or row
                // indexes go out of array bounds, the counter will carry only
                // and skip that position.
                
                if (columnindex >= SIZE || rowindex >= SIZE || columnindex < 0 || rowindex < 0) {
                    counterY++;
                    columnindex++;
                } else if (minefield[rowindex][columnindex] == HIDDEN_MINE) {
                    minetotal++;
                    counterY++;
                    columnindex++;
                } else {
                    counterY++;
                    columnindex++;
                }
            }
            rowindex++;
            counterX++;
        }
        
            
        // This if statement is for when there are no adjacent mines (minetotal = 0), 
        // it will reveal a 3 x 3 grid of "VISIBLE_SAFE", where the row and column 
        // the user inputted is the centre of the grid. The same while loop principals
        // apply, where it will start from the top left corner and change the
        // array index row by row.
        
        if (minetotal == 0) {
            rowindex = row - 1;
            counterX = 0;
            while (counterX < REVEAL_GRID) {
                int counterY = 0;
                int columnindex = column - 1;
                
                while (counterY < REVEAL_GRID) {
                    // As such, the if statement is used again for out of bounds
                    // array indexes.
                    
                    if (columnindex >= SIZE || rowindex >= SIZE || columnindex < 0 || rowindex < 0) {
                        columnindex++;
                        counterY++;
                    } else if (minefield[rowindex][columnindex] == VISIBLE_SAFE) {
                        columnindex++;
                        counterY++;
                    } else {
                        minefield[rowindex][columnindex] = VISIBLE_SAFE;
                        columnindex++;
                        counterY++;
                    }
                }
                rowindex++;
                counterX++;
            }
            
            // This while function is placed after the sqaures have been revealed
            // to check if the game should still continue or end. This is done
            // through a counter sum within a while loop, checking if there are
            // any hidden safes left on the grid.
            
            counterX = 0;
            int hiddensafes = 0;
    
            while (counterX < SIZE) {
                int counterY = 0;
        
                while (counterY < SIZE) {
                    if (minefield[counterX][counterY] == HIDDEN_SAFE) {
                        hiddensafes++;
                    }
                    counterY++;
                }
                counterX++;
            }      
    
            // If hiddensafes = 0, that means there are no hidden safe spots left,
            // and the user wins the game, placing the gameWon = 0 and
            // ending the loop.
            // If hiddensafes = 1, the game is continued until all hidden safe
            // spots are discovered, or the user loses.
            
            if (hiddensafes == 0) {
                printf("Game won!\n");
                gameWon = 1;
                
                if (mode == 0) { 
                    print_debug_minefield(minefield);
                } else {
                    print_gameplay_minefield(minefield, gameWon, gameLost);
                }
                
                
            } else {          
                if (mode == 0) {
                    print_debug_minefield(minefield);
                } else {
                    print_gameplay_minefield(minefield, gameWon, gameLost);
                }
                
            }
            
        } 
        
        
        // This else statement is for when there are adjacent mine(s) (minetotal > 0),
        // it will only reveal the coordinates the user inputted. 
        
        else {
            minefield[row][column] = VISIBLE_SAFE;
            
            // This while function is placed after the sqaures have been revealed
            // to check if the game should still continue or end. This is done
            // through a counter sum within a while loop, checking if there are
            // any hidden safes left on the grid.
            
            counterX = 0;
            int hiddensafes = 0;
    
            while (counterX < SIZE) {
                int counterY = 0;
        
                while (counterY < SIZE) {
                    if (minefield[counterX][counterY] == HIDDEN_SAFE) {
                        hiddensafes++;
                    }
                    counterY++;
                }
                counterX++;
            }      
    
            // If hiddensafes = 0, that means there are no hidden safe spots left,
            // and the user wins the game, placing the gameWon = 0 and
            // ending the loop.
            // If hiddensafes = 1, the game is continued until all hidden safe
            // spots are discovered, or the user loses.
            
            if (hiddensafes == 0) {
                printf("Game won!\n");
                gameWon = 1;
                
                if (mode == 0) {
                    print_debug_minefield(minefield);
                } else {
                    print_gameplay_minefield(minefield, gameWon, gameLost);
                }

                
            } else {                
                if (mode == 0) {
                    print_debug_minefield(minefield);
                } else {
                    print_gameplay_minefield(minefield, gameWon, gameLost);
                }
                
            }
            
            
        }
          
    }
}

void reveal_radial(int minefield[SIZE][SIZE], int gameWon, int gameLost, int mode, int firstTurn) {
    int row;
    int column;
    
    scanf("%d %d", &row, &column);
    
    int counterX = 0;
    int rowindex = row - 1; 
    int rowindex2 = 0;
    int columnindex2;
    int minetotal = 0;
    
    
    // This counter within a counter starts from the top right of the n x n grid
    // and scans every sqaure within the grid, adding the bombs into a counter.     
    while (counterX < REVEAL_GRID) {
        int counterY = 0;
        int columnindex = column - 1;
               
        while (counterY < REVEAL_GRID) {
            // This if statement will be used often, as when the column or row
            // indexes go out of array bounds, the counter will carry only
            // and skip that position.
            
            if (columnindex >= SIZE || rowindex >= SIZE || columnindex < 0 || rowindex < 0) {
                counterY++;
                columnindex++;
            } else if (minefield[rowindex][columnindex] == HIDDEN_MINE) {
                minetotal++;
                counterY++;
                columnindex++;
            } else {
                counterY++;
                columnindex++;
            }
        }
        rowindex++;
        counterX++;
    }
    
    // This if statement checks whether the user's first turn was on a mine. This
    // will enter the safeFirstTurn function, where the map shifts downwards until
    // the user's input is safe to do so.
    
    if (minefield[row][column] == HIDDEN_MINE && firstTurn == 0) {
        safeFirstTurn(minefield, firstTurn, row, column);
        firstTurn++;
    }    
    
    // This if statement checks whether the user's inputs land on a mine (not
    // first turn). If so, the game is lost (gamemode = 1) and the map is printed
    // out.
    
    if (minefield[row][column] == HIDDEN_MINE && firstTurn != 0) {
        printf("Game over\n");
        gameLost = 1;
        
        if (mode == 0) {
            print_debug_minefield(minefield);
        } else {
            print_gameplay_minefield(minefield, gameWon, gameLost);
        }
    }
    
    // This else if statement checks whether the user's input has any adjacent
    // mines near it, through the use of the while loop counter above. If 
    // minetotal > 0, the function will only reveal that square.
    
    else if (minetotal > 0 && minefield[row][column] != HIDDEN_MINE) {
        minefield[row][column] = VISIBLE_SAFE;
        
        if (mode == 0) {
            print_debug_minefield(minefield);
        } else {
            print_gameplay_minefield(minefield, gameWon, gameLost);
        }
    }
    
    // Else, if the user's input has no adjacent mine, and the square is a 
    // HIDDEN_SAFE, this will reveal a radius of lines around the selected squares
    // going up, down, left, right and in 45 degree angles.
    
    else {
        minefield[row][column] = VISIBLE_SAFE;
        
        // This function scans the top left diagonal through the use of  while loops.
        // This is achieved through subtracting one from the row and column everytime 
        // a square has been scanned.
        // If there are any adjacent mines in the square that is scanned (minetotal > 0)
        // it will only reveal that square and stop the whole while loop.

        counterX = 0;
        rowindex2 = row - 1;
        columnindex2 = column - 1;

        while (minetotal == 0 && rowindex2 < SIZE && rowindex2 >= 0 && columnindex2 < SIZE && columnindex2 >= 0) {
            rowindex = rowindex2 - 1;
            while (counterX < REVEAL_GRID) {
                int counterY = 0;
                int columnindex = columnindex2 - 1;
                       
                while (counterY < REVEAL_GRID) {
                    // This if statement will be used often, as when the column or row
                    // indexes go out of array bounds, the counter will carry only
                    // and skip that position.
                    
                    if (columnindex >= SIZE || rowindex >= SIZE || columnindex < 0 || rowindex < 0) {
                        counterY++;
                        columnindex++;
                    } else if (minefield[rowindex][columnindex] == HIDDEN_MINE) {
                        minetotal++;
                        counterY++;
                        columnindex++;
                    } else {
                        counterY++;
                        columnindex++;
                    }
                }
                rowindex++;
                counterX++;
            }
            minefield[rowindex2][columnindex2] = VISIBLE_SAFE;
            rowindex2--;
            columnindex2--;
            counterX = 0;
        }
        
        // This function scans the top right line through while loops. This is achieved
        // through subtracting one from the row and adding one to the column everytime
        // a square has been scanned. If there are any adjacent mines revealed in
        // the square that has been scanned (minetotal > 0), it will only reveal
        // the square and stop the whole while loop.
        
        minetotal = 0;
        counterX = 0;
        rowindex2 = row - 1;
        columnindex2 = column + 1;

        while (minetotal == 0 && rowindex2 < SIZE && rowindex2 >= 0 && columnindex2 < SIZE && columnindex2 >= 0) {
            rowindex = rowindex2 - 1;
            while (counterX < REVEAL_GRID) {
                int counterY = 0;
                int columnindex = columnindex2 - 1;
                       
                while (counterY < REVEAL_GRID) {
                    // This if statement will be used often, as when the column or row
                    // indexes go out of array bounds, the counter will carry only
                    // and skip that position.
                    
                    if (columnindex >= SIZE || rowindex >= SIZE || columnindex < 0 || rowindex < 0) {
                        counterY++;
                        columnindex++;
                    } else if (minefield[rowindex][columnindex] == HIDDEN_MINE) {
                        minetotal++;
                        counterY++;
                        columnindex++;
                    } else {
                        counterY++;
                        columnindex++;
                    }
                }
                rowindex++;
                counterX++;
            }
            minefield[rowindex2][columnindex2] = VISIBLE_SAFE;
            rowindex2--;
            columnindex2++;
            counterX = 0;
        }    
        
        // Likewhise, this function scans the bottom right diagonal. This is achieved
        // through while loops, and adding one onto the row and column everytime a 
        // sqaure has been scanned. A mine counter loop is also included, as whenever
        // there are adjacent mines, the while loop will only reveal that square and
        // stop the function.
        
        minetotal = 0;
        counterX = 0;
        rowindex2 = row + 1;
        columnindex2 = column + 1;

        while (minetotal == 0 && rowindex2 < SIZE && rowindex2 >= 0 && columnindex2 < SIZE && columnindex2 >= 0) {
            rowindex = rowindex2 - 1;
            while (counterX < REVEAL_GRID) {
                int counterY = 0;
                int columnindex = columnindex2 - 1;
                       
                while (counterY < REVEAL_GRID) {
                    // This if statement will be used often, as when the column or row
                    // indexes go out of array bounds, the counter will carry only
                    // and skip that position.
                    
                    if (columnindex >= SIZE || rowindex >= SIZE || columnindex < 0 || rowindex < 0) {
                        counterY++;
                        columnindex++;
                    } else if (minefield[rowindex][columnindex] == HIDDEN_MINE) {
                        minetotal++;
                        counterY++;
                        columnindex++;
                    } else {
                        counterY++;
                        columnindex++;
                    }
                }
                rowindex++;
                counterX++;
            }
            minefield[rowindex2][columnindex2] = VISIBLE_SAFE;
            rowindex2++;
            columnindex2++;
            counterX = 0;
        }   
        
        // Likewise, this reveals the bottom left diagonal, where the row is added
        // by one, and column is subtracted by one. As such, if there are any 
        // adjacent mines, the function will only reveal that square and the while
        // loops stops (as minetotal > 0).
        minetotal = 0;
        counterX = 0;
        rowindex2 = row + 1;
        columnindex2 = column - 1;

        while (minetotal == 0 && rowindex2 < SIZE && rowindex2 >= 0 && columnindex2 < SIZE && columnindex2 >= 0) {
            rowindex = rowindex2 - 1;
            while (counterX < REVEAL_GRID) {
                int counterY = 0;
                int columnindex = columnindex2 - 1;
                       
                while (counterY < REVEAL_GRID) {
                    // This if statement will be used often, as when the column or row
                    // indexes go out of array bounds, the counter will carry only
                    // and skip that position.
                    
                    if (columnindex >= SIZE || rowindex >= SIZE || columnindex < 0 || rowindex < 0) {
                        counterY++;
                        columnindex++;
                    } else if (minefield[rowindex][columnindex] == HIDDEN_MINE) {
                        minetotal++;
                        counterY++;
                        columnindex++;
                    } else {
                        counterY++;
                        columnindex++;
                    }
                }
                rowindex++;
                counterX++;
            }
            minefield[rowindex2][columnindex2] = VISIBLE_SAFE;
            rowindex2++;
            columnindex2--;
            counterX = 0;
        }
        
        // Similarly, this while loop only reveals the squares above, where the row
        // is subtracted by one and the column remains the same. As such, if there are any 
        // adjacent mines, the function will only reveal that square and the while
        // loops stops (as minetotal > 0).
        minetotal = 0;
        counterX = 0;
        rowindex2 = row - 1;
        columnindex2 = column;

        while (minetotal == 0 && rowindex2 < SIZE && rowindex2 >= 0 && columnindex2 < SIZE && columnindex2 >= 0) {
            rowindex = rowindex2 - 1;
            while (counterX < REVEAL_GRID) {
                int counterY = 0;
                int columnindex = columnindex2 - 1;
                       
                while (counterY < REVEAL_GRID) {
                    // This if statement will be used often, as when the column or row
                    // indexes go out of array bounds, the counter will carry only
                    // and skip that position.
                    
                    if (columnindex >= SIZE || rowindex >= SIZE || columnindex < 0 || rowindex < 0) {
                        counterY++;
                        columnindex++;
                    } else if (minefield[rowindex][columnindex] == HIDDEN_MINE) {
                        minetotal++;
                        counterY++;
                        columnindex++;
                    } else {
                        counterY++;
                        columnindex++;
                    }
                }
                rowindex++;
                counterX++;
            }
            minefield[rowindex2][columnindex2] = VISIBLE_SAFE;
            rowindex2--;
            counterX = 0;
        }
        
        // Similarly, this while loop only reveals the squares below, where the row
        // is added by one and the column remains the same. As such, if there are any 
        // adjacent mines, the function will only reveal that square and the while
        // loops stops (as minetotal > 0).
        minetotal = 0;
        counterX = 0;
        rowindex2 = row + 1;
        columnindex2 = column;

        while (minetotal == 0 && rowindex2 < SIZE && rowindex2 >= 0 && columnindex2 < SIZE && columnindex2 >= 0) {
            rowindex = rowindex2 - 1;
            while (counterX < REVEAL_GRID) {
                int counterY = 0;
                int columnindex = columnindex2 - 1;
                       
                while (counterY < REVEAL_GRID) {
                    // This if statement will be used often, as when the column or row
                    // indexes go out of array bounds, the counter will carry only
                    // and skip that position.
                    
                    if (columnindex >= SIZE || rowindex >= SIZE || columnindex < 0 || rowindex < 0) {
                        counterY++;
                        columnindex++;
                    } else if (minefield[rowindex][columnindex] == HIDDEN_MINE) {
                        minetotal++;
                        counterY++;
                        columnindex++;
                    } else {
                        counterY++;
                        columnindex++;
                    }
                }
                rowindex++;
                counterX++;
            }
            minefield[rowindex2][columnindex2] = VISIBLE_SAFE;
            rowindex2++;
            counterX = 0;
        }
        
        // Likewise, this while loop only reveals the squares to the left, where the row
        // remains the same, and the column is subtracted by one. As such, if there are any 
        // adjacent mines, the function will only reveal that square and the while
        // loops stops (as minetotal > 0).
        
        minetotal = 0;
        counterX = 0;
        rowindex2 = row;
        columnindex2 = column - 1;

        while (minetotal == 0 && rowindex2 < SIZE && rowindex2 >= 0 && columnindex2 < SIZE && columnindex2 >= 0) {
            rowindex = rowindex2 - 1;
            while (counterX < REVEAL_GRID) {
                int counterY = 0;
                int columnindex = columnindex2 - 1;
                       
                while (counterY < REVEAL_GRID) {
                    // This if statement will be used often, as when the column or row
                    // indexes go out of array bounds, the counter will carry only
                    // and skip that position.
                    
                    if (columnindex >= SIZE || rowindex >= SIZE || columnindex < 0 || rowindex < 0) {
                        counterY++;
                        columnindex++;
                    } else if (minefield[rowindex][columnindex] == HIDDEN_MINE) {
                        minetotal++;
                        counterY++;
                        columnindex++;
                    } else {
                        counterY++;
                        columnindex++;
                    }
                }
                rowindex++;
                counterX++;
            }
            minefield[rowindex2][columnindex2] = VISIBLE_SAFE;
            columnindex2--;
            counterX = 0;
        }
        
        // Likewise, this while loop only reveals the squares to the right, where 
        // the row remains the same and the column is added by one. As such, if there 
        // are any adjacent mines, the function will only reveal that square and
        // the while loops stops (as minetotal > 0).
        minetotal = 0;
        counterX = 0;
        rowindex2 = row;
        columnindex2 = column + 1;

        while (minetotal == 0 && rowindex2 < SIZE && rowindex2 >= 0 && columnindex2 < SIZE && columnindex2 >= 0) {
            rowindex = rowindex2 - 1;
            while (counterX < REVEAL_GRID) {
                int counterY = 0;
                int columnindex = columnindex2 - 1;
                       
                while (counterY < REVEAL_GRID) {
                    // This if statement will be used often, as when the column or row
                    // indexes go out of array bounds, the counter will carry only
                    // and skip that position.
                    
                    if (columnindex >= SIZE || rowindex >= SIZE || columnindex < 0 || rowindex < 0) {
                        counterY++;
                        columnindex++;
                    } else if (minefield[rowindex][columnindex] == HIDDEN_MINE) {
                        minetotal++;
                        counterY++;
                        columnindex++;
                    } else {
                        counterY++;
                        columnindex++;
                    }
                }
                rowindex++;
                counterX++;
            }
            minefield[rowindex2][columnindex2] = VISIBLE_SAFE;
            columnindex2++;
            counterX = 0;
        }
        
        // This while function is placed after the sqaures have been revealed
        // to check if the game should still continue or end. This is done
        // through a counter sum within a while loop, checking if there are
        // any hidden safes left on the grid.    
            
        counterX = 0;
        int hiddensafes = 0;
        while (counterX < SIZE) {
            int counterY = 0;
    
            while (counterY < SIZE) {
                if (minefield[counterX][counterY] == HIDDEN_SAFE) {
                    hiddensafes++;
                }
                counterY++;
            }
            counterX++;
        }      

        // If hiddensafes = 0, that means there are no hidden safe spots left,
        // and the user wins the game, placing the gameWon = 0 and
        // ending the loop.
        // If hiddensafes = 1, the game is continued until all hidden safe
        // spots are discovered, or the user loses.

        if (hiddensafes == 0) {
            printf("Game won!\n");
            gameWon = 1;
            
            if (mode == 0) { 
                print_debug_minefield(minefield);
            } else {
                print_gameplay_minefield(minefield, gameWon, gameLost);
            }       
        } else {          
            if (mode == 0) {
                print_debug_minefield(minefield);
            } else {
                print_gameplay_minefield(minefield, gameWon, gameLost);
            }  
        }
    }    
} 

// This void allows the grid to shift downwards if the player loses on their first turn.
void safeFirstTurn(int minefield[SIZE][SIZE], int firstTurn, int row, int column) {
    int tempArray[SIZE] = {};
    int counterX = 0;
    
    // A temporary array is made to store the values of the last row. This is done through
    // the use of a while loop to scan and place every value into that array.
    
    while (minefield[row][column] == HIDDEN_MINE) {
        while (counterX < SIZE) {
            tempArray[counterX] = minefield[SIZE - 1][counterX];
            counterX++;
        }
        
        // This counter is used to shift each row downwards. I started from the 6th
        // row, and moved every row forwards.
        counterX = 6;
        
        while (counterX >= 0) {
            int counterY = 0;
            
            while (counterY < SIZE) {
                minefield[counterX + 1][counterY] = minefield[counterX][counterY];
                counterY++;
            }
            counterX--;
        }
        
        // This while loop is used to place the values from the temporary array (last row)
        // into the first row of the minefield array.
        counterX = 0;
        
        while (counterX < SIZE) {
            minefield[0][counterX] = tempArray[counterX];
            counterX++;
        }
    }
    
}           

         

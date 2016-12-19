#define R1 11
#define G1 12
#define EN 8
#define A 3
#define B 5
#define C 7
#define D 10
#define L 13
#define S 15

#define ROWS 8
#define COLUMNS 80
#define MAX_MESSAGE 1000

// call init() to initialise the display
int init();

// sets display on
void displayon();

// sets display off
void displayoff();

// clears each row in the display, a 'blank slate'
void clear();

// Shifts a bit on the current row
// The value of (R1,G1) will be shifted
void shift();

// Sets the current colour to use for the next shift
// 1: red
// 2: green
// 3: orange
// anything else: off
// Annoyingly, setting high = off
void colour(int);

// Sets colour based on colour() and shifts to current row
void colourshift(int);

// Enable lights on row n. 0-indexed, top = 0.
// Note: only a single row can be displayed at a time
void showrow(int);

// Shifts row row_value of a letter on to the currently selected row
// Must provide width of letter too (might be redundant, just give the row)
void shift_letter_row(int, int, int);

// Given a string, will give the amount of columns based on fontv.c
int column_count(char*);

// Will refresh the display n times. Set 0 for infinite
void refresh();

// Push a static message to the LCD
void pushmessage(char*);

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "ldp.h"
#include "fontv.c"

pthread_mutex_t display_mutex;
int display_matrix[ROWS][COLUMNS];

int message_matrix[ROWS][MAX_MESSAGE];

void clear_matrix(){
  int i,j;
  for (i = 0; i < ROWS; i++){
    for (j = 0; j < COLUMNS; j++){
      display_matrix[i][j] = 0;
    }
  }
}

int init(){
  if (wiringPiSetupPhys() == -1)
    return 1;
  pinMode (R1,OUTPUT);
  pinMode (G1,OUTPUT);
  pinMode (EN,OUTPUT);
  pinMode (A,OUTPUT);
  pinMode (B,OUTPUT);
  pinMode (C,OUTPUT);
  pinMode (D,OUTPUT);
  pinMode (L,OUTPUT);
  pinMode (S,OUTPUT);
  digitalWrite(R1,1);
  digitalWrite(G1,1);
  digitalWrite(S,1);
  digitalWrite(L,0);
  digitalWrite(EN,0);
  clear();
  return 0;
}

void displayoff(){
  digitalWrite(EN,0);
}

void displayon(){
  digitalWrite(EN,1);
}

void clear(){
 digitalWrite(R1,1);
 digitalWrite(G1,1);
 int i;
 for (i = 0; i < 80; i++){
  digitalWrite(S,1);
  digitalWrite(S,0);
  digitalWrite(S,1);
 }
 displayoff();
}

void shift(){
  digitalWrite(S,1);
  digitalWrite(S,0);
  digitalWrite(S,1);
}

// TODO: Make this use bitshifts
void colour(int n){
  switch(n){
    case 3: //orange
      digitalWrite(R1,0);
      digitalWrite(G1,0);
      break;
    case 2: //green
      digitalWrite(R1,1);
      digitalWrite(G1,0);
      break;
    case 1: //red
      digitalWrite(R1,0);
      digitalWrite(G1,1);
      break;
    default:
      digitalWrite(R1,1);
      digitalWrite(G1,1);
  }
}

void colourshift(int n){
  colour(n);
  shift();
}

void showrow(int n){
  digitalWrite(EN,0);
  digitalWrite(A,(n >> 0) & 1);
  digitalWrite(B,(n >> 1) & 1);
  digitalWrite(C,(n >> 2) & 1);
  digitalWrite(D,(n >> 3) & 1);

  digitalWrite(L,1);
  digitalWrite(L,0);
  digitalWrite(EN,1);
}

void shift_letter_row(int row_value, int width, int colour){
  int i,j;
  for (j = width-1; j >= 0; j--){
    if ((row_value >> j) & 1)
      colourshift(colour);
    else
      colourshift(0);
  }
}

int column_count(char* m){
  int total = 0;
  int i;
  for (i = 0; i < strlen(m); i++){
    total += letters[m[i]][0];
  }
  return total;
}

// infinite loop that constantly refreshes the display
// based on what's in display_matrix
// (run this in a thread or you're gonna have a bad time)
void refresh(char* message){
  int i,j;
  while(1){
    // Lock the mutex while we refresh the display
    pthread_mutex_lock(&display_mutex);
    for (i = 0; i < ROWS; i++){
      showrow(i);
      for (j = 0; j < COLUMNS; j++){
        colourshift(display_matrix[i][j]);
      }
    }
    pthread_mutex_unlock(&display_mutex);
    usleep(33);
  }
}
// Push a message to the message matrix
void pushmessage(char* message){
  int i,j,k,c,cur_col,cur_length;
  srand(time(NULL));
  // For each row
  for (i = 0; i < 8; i++){
    cur_col = 0;
    // For each letter in the message
    for (j = 0; j < strlen(message); j++){
      // For each column in the letter, set that bit if it needs to be displayed
      cur_length = letters[(int)message[j]][0];
      for (k = 0; k < cur_length; k++){
        if ((letters[(int)message[j]][i+1] >> k) & 1){
          // For some reason we need to push it to the matrix backwards?
          display_matrix[i][(cur_length-k)+cur_col] = 1;
        }
      }
      cur_col += cur_length;
    }
  }
}

void main(int argc, char* argv[]){
  init();
  clear_matrix();
  char* message = argv[1];
  int i,j;

  // Spawn the refresh loop as a separate thread
  pthread_t rfrsh;
  pthread_create(&rfrsh,NULL,(void *)refresh,message);
  pthread_mutex_init(&display_mutex,NULL);
  pushmessage(message);
  pthread_exit(&rfrsh);
}

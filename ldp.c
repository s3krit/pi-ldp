#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
#include <pthread.h>
#include "ldp.h"
#include "fontv.c"

thread_mutex_t display_mutex;

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

void main(int argc, char* argv[]){
  init();
  int i,j;
  int* full_matrix = malloc(80*sizeof(int));
  char* message = argv[1];

  // Spawn the refresh loop as a separate thread
  pthread_t rfrsh;
  pthread_create(&rfrsh,NULL,refresh,NULL);
  pthread_mutex_init(&refresh_mutex,NULL);

  // Lock while we add the text
  pthread_mutex_lock(&refresh_mutex);
  for (i = 1; i < 9; i++){
    for (j = 0; j < strlen(message); j++){
      shift_letter_row(letters[(int)message[j]][i],letters[(int)message[j]][0],2);
    }
    showrow(i-1);
  }
  pthread_mutex_unlock(&refresh_mutex);

  pthread_exit(&rfrsh);
}

void refresh(){
  int i;
  while(1)
    // Lock the mutex while we refresh the display
    pthread_mutex_lock(&refresh_mutex);
    for (i = 0; i < 8; i++){
      showrow(i);
    }
    pthread_mutex_unlock(&refresh_mutex);
    // sleep for 1/n to achieve n frames per second???
    sleep(1.0/30.0);
  }
}

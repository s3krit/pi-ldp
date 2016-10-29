#include <stdio.h>
#include <wiringPi.h>
#include "ldp.h"

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

void colour(int n){
  switch(n){
    case 3: //orange
      digitalWrite(R1,0);
      digitalWrite(G1,1);
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

void main(){
  init();
  int i,j;
  while (1){
    displayoff();
    for (j = 0; j < 8; j++){
      colourshift(j%2);
      showrow(j);
    }
    displayon();
  }
}

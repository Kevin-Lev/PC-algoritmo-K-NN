#include <iostream>
#include <pthread.h>
#include <cmath>

#define matrixColumn 5
#define matrixRow 5

using namespace std;

int main () { //Euclidian
  float Pmembers[matrixColumn] = {1, 2, 3, 4, 5},
        Qmembers[matrixColumn] = {0, 3, 2, 7, 4},
        euclidianDistance[matrixRow] = {0, 0, 0, 0, 0};
        
  float aux = 0.0;

  for (int row = 0; row < matrixRow; row++) {
    for (int column = 0; column < matrixColumn; column++) { 
      aux = abs(pow(Pmembers[row] - Qmembers[column], 2.0));
      euclidianDistance[row] += aux;
    }
    
    euclidianDistance[row] = sqrt(euclidianDistance[row]);
    cout << euclidianDistance[row] << endl;
    aux = 0;
  }
}

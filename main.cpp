#include <iostream>
#include <pthread.h>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

int contaLinhas(string nomeArquivo){
  int cont=0;
  string unused;
  fstream arquivo;
  arquivo.open(nomeArquivo);
  while(getline(arquivo, unused)){
    cont+=1;
  }
  arquivo.close();
  return cont;
}

struct dadosArquivo {
  string genero;
  vector<float> linhasArquivo;
};

void printaVetor(dadosArquivo vetor[], int tamVetor){
  for(int i=0;i<tamVetor;i++){
    for(int j=0;j<vetor[i].linhasArquivo.size();j++){
      cout << vetor[i].linhasArquivo[j] << endl;
    }
    cout << vetor[i].genero << endl << endl;
  }
}

void insereVetor(int tamLinhas, dadosArquivo *vetorDados, string nomeArquivo){
  int j = 0, k = 0, cont = 0;
  float numchars;
  string linhaArquivo, aux;
  fstream arquivo;

  arquivo.open(nomeArquivo);

  while(getline(arquivo, linhaArquivo)){
    k = linhaArquivo.find_first_not_of("0123456789.,");
    for(int l=k;l<=linhaArquivo.size();l++){
      aux += linhaArquivo[l];
    }
    vetorDados[j].genero = aux;
    aux.clear();
    for(int i=0;i<linhaArquivo.size();i++){
      if(linhaArquivo[i] != ','){
        aux += linhaArquivo[i];
      }
      else{
        numchars = stof(aux);
        aux.clear();
        vetorDados[j].linhasArquivo.push_back(numchars);
      }
    }
    j++;
    aux.clear();
  }
}


int main(int argc, char* argv[]){

  string arquivoTest = argv[1], arquivoBase = argv[2];
  int k=0, l=0, linhasTest = contaLinhas(arquivoTest), linhasBase = contaLinhas(arquivoBase);
  float aux, distancia = 0, menorDistancia = 100000;
  vector<float> vetorDistancias;
  dadosArquivo dadosTest[linhasTest], dadosBase[linhasBase];
  insereVetor(linhasTest, dadosTest, arquivoTest);
  insereVetor(linhasBase, dadosBase, arquivoBase);
  //printaVetor(dadosTest, linhasTest);
  //printaVetor(dadosBase, linhasBase);

  cout << "Calculando a distância entre as colunas das matrizes..." << endl;

  for(int i=0;i<linhasTest;i++){
    for(int j=0;j<linhasBase;j++){
      for(int k=0;k<dadosBase[j].linhasArquivo.size();k++){  // dadosTest.linhasArquivo e dadosBase.linhasArquivo tem o mesmo tamanho.
        //cout << "dadosTest[" << i << "].linhasArquivo[" << k << "]: " << dadosTest[i].linhasArquivo[k] << endl;
        //cout << "dadosBase[" << j << "].linhasArquivo[" << k << "]: " << dadosBase[j].linhasArquivo[k] << endl << endl;
        distancia = distancia + abs(pow((dadosTest[i].linhasArquivo[k] - dadosBase[j].linhasArquivo[k]), 2));
      }
      distancia = sqrt(distancia);
      //cout << "distancia: " << distancia << endl << endl;
      if(distancia < menorDistancia){
        menorDistancia = distancia;
      }
      vetorDistancias.push_back(menorDistancia);
      menorDistancia = 100000;
      distancia = 0;
    }
  }

}

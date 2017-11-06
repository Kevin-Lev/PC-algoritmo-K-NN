#include <iostream>
#include <pthread.h>
#include <thread>
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

struct distanciaClasse {
  string genero;
  float distancia;
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

vector<distanciaClasse> vetorDistancias;
vector<float> vetMenoresDist;


vector<distanciaClasse> calculaDistancia(int linhasTest, int linhasBase, dadosArquivo dadosTest[], dadosArquivo dadosBase[]){
  float menorDistancia = 100000;
  float distancia = 0;
  distanciaClasse distClasse;
  vector<distanciaClasse> vetorDistancias;

  for(int i=0;i<linhasTest;i++){
    for(int j=0;j<linhasBase;j++){
      for(int k=0;k<dadosBase[j].linhasArquivo.size();k++){  // dadosTest.linhasArquivo e dadosBase.linhasArquivo tem o mesmo tamanho.
        distancia = distancia + abs(pow((dadosTest[i].linhasArquivo[k] - dadosBase[j].linhasArquivo[k]), 2));
      }
      distancia = sqrt(distancia);
      if(distancia < menorDistancia){
        menorDistancia = distancia;
        distClasse.genero = dadosBase[j].genero;
      }
      distancia = 0;
    }
      distClasse.distancia = menorDistancia;
      vetorDistancias.push_back(distClasse);
      menorDistancia = 100000;
      distancia = 0;
  }
  return vetorDistancias;
}


int main(int argc, char* argv[]){

  unsigned concurrentThreadsSupported = std::thread::hardware_concurrency();
  string arquivoTest = argv[1], arquivoBase = argv[2];
  int k=0, l=0, args[concurrentThreadsSupported], linhasTest = contaLinhas(arquivoTest), linhasBase = contaLinhas(arquivoBase);
  float aux, distancia = 0;
  dadosArquivo dadosTest[linhasTest], dadosBase[linhasBase];
  pthread_t thread[concurrentThreadsSupported];

  insereVetor(linhasTest, dadosTest, arquivoTest);
  insereVetor(linhasBase, dadosBase, arquivoBase);
  //printaVetor(dadosTest, linhasTest);
  //printaVetor(dadosBase, linhasBase);

  cout << "Este sistema suporta e irá usar " << concurrentThreadsSupported << " Threads" << endl;
  cout << "Calculando a distância entre as colunas das matrizes..." << endl << endl;
  vetorDistancias = calculaDistancia(linhasTest, linhasBase, dadosTest, dadosBase);
  for(int i=0;i<vetorDistancias.size();i++){
    cout << i << endl << endl;
    cout << vetorDistancias[i].genero << endl;
    cout << vetorDistancias[i].distancia << endl << endl;
  }

  /*for(int j=0; j<linhasTest;j++){
    for(int i=0;i<concurrentThreadsSupported;i++){
      args[i] = i+1;
      pthread_create(&thread[i], NULL,calculaDistancia2, NULL);
      pthread_join(thread[i], NULL);
    }
  }*/
}

#include <iostream>
#include <pthread.h>
#include <thread>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <time.h>

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

struct argsfuncDist {
  int linhasTest, linhasBase, numThreads, posInicial, posFinal, inicioI, idThread;
  dadosArquivo *dadosTest;
  dadosArquivo *dadosBase;
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
  int j = 0, k = 0;
  float numchars;
  string linhaArquivo, aux;
  fstream arquivo;

  arquivo.open(nomeArquivo);

  while(getline(arquivo, linhaArquivo)){
    k = linhaArquivo.find_first_not_of("0123456789.,-");
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
distanciaClasse *vetorAux;
pthread_barrier_t barreira;


void *threadDistancia(void *arg){
  pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
  float menorDistancia = 100000;
  distanciaClasse distClasse;
  float distancia = 0;
  string nomeGenero;
  argsfuncDist *argDist = (argsfuncDist*) arg;


  for(int i=0;i<argDist->linhasTest;i++){
    for(int j=argDist->posInicial;j<=argDist->posFinal;j++){
      for(int k=0;k<argDist->dadosBase[j].linhasArquivo.size();k++){
        distancia = distancia + abs(pow((argDist->dadosTest[i].linhasArquivo[k] - argDist->dadosBase[j].linhasArquivo[k]), 2));
      }
      distClasse.distancia = sqrt(distancia);
      distClasse.genero = argDist->dadosBase[j].genero;
      pthread_mutex_lock(&mut);
      vetorAux[j] = distClasse;
      pthread_mutex_unlock(&mut);
      distancia = 0;
    }

    pthread_barrier_wait(&barreira);
    if(argDist->idThread == 0){
      distanciaClasse distAux;
      distAux.distancia = 100000;
      for(int l=0;l<argDist->linhasBase;l++){
        if(vetorAux[l].distancia < distAux.distancia){
          distAux.genero = vetorAux[l].genero;
          distAux.distancia = vetorAux[l].distancia;
        }
      }
      vetorDistancias.push_back(distAux);
    }
    distancia = 0;
    pthread_barrier_wait(&barreira);
  }

}

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
      distClasse.distancia = sqrt(distancia);
      distClasse.genero = dadosBase[j].genero;
      vetorAux[j] = distClasse;
      distancia = 0;
    }
    distanciaClasse distAux;
    distAux.distancia = 100000;
    for(int l=0;l<linhasBase;l++){
      if(vetorAux[l].distancia < distAux.distancia){
        distAux.genero = vetorAux[l].genero;
        distAux.distancia = vetorAux[l].distancia;
      }
    }
      vetorDistancias.push_back(distAux);
  }
  return vetorDistancias;
}


int main(int argc, char* argv[]){

  unsigned concurrentThreadsSupported = std::thread::hardware_concurrency();
  int args[concurrentThreadsSupported], linhasTest = contaLinhas(argv[1]), linhasBase = contaLinhas(argv[2]), divisaoBase, restoBase, distancia = 0;
  float speedUp;
  distanciaClasse auxiliarLocal[linhasBase];
  dadosArquivo dadosTest[linhasTest], dadosBase[linhasBase];
  argsfuncDist vetorParametros[concurrentThreadsSupported];
  pthread_t thread[concurrentThreadsSupported];
  pthread_barrier_init(&barreira, NULL, concurrentThreadsSupported);
  time_t inicio, inicioThreads, fim, fimThreads;


  insereVetor(linhasTest, dadosTest, argv[1]);
  insereVetor(linhasBase, dadosBase, argv[2]);
  //printaVetor(dadosTest, linhasTest);
  //printaVetor(dadosBase, linhasBase);

  vetorAux = auxiliarLocal;

  cout << endl << "Este sistema suporta e irá usar " << concurrentThreadsSupported << " Threads" << endl << endl;
  cout << "Nome do arquivo de teste: " << argv[1] << endl << "Nome do arquivo de treino " << argv[2] << endl;
  cout << "Calculando de forma sequencial a distância entre as colunas das matrizes..." << endl << endl;

  divisaoBase = linhasBase/concurrentThreadsSupported;
  restoBase = linhasBase%concurrentThreadsSupported;

  inicio = time(NULL);
  vetorDistancias = calculaDistancia(linhasTest, linhasBase, dadosTest, dadosBase);
  fim = time(NULL);

  cout << "Calculando de forma concorrente a distância entre as colunas das matrizes..." << endl << endl;


  for(int i=0;i<concurrentThreadsSupported;i++){
    vetorParametros[i].linhasTest = linhasTest;
    vetorParametros[i].linhasBase = linhasBase;
    vetorParametros[i].numThreads = concurrentThreadsSupported;
    vetorParametros[i].posInicial = 0;
    vetorParametros[i].posFinal = divisaoBase - 1;
    vetorParametros[i].dadosTest = dadosTest;
    vetorParametros[i].dadosBase = dadosBase;
  }

  inicioThreads = time(NULL);
  for(int i=0;i<concurrentThreadsSupported;i++){
    vetorParametros[i].idThread = i;
    if(i > 0){
      if(i == concurrentThreadsSupported-1){
        vetorParametros[i].posInicial = vetorParametros[i].posInicial + divisaoBase;
        vetorParametros[i].posFinal = vetorParametros[i].posFinal + divisaoBase + restoBase;
      }
      else{
        vetorParametros[i].posInicial = vetorParametros[i].posInicial + divisaoBase;
        vetorParametros[i].posFinal = vetorParametros[i].posFinal + divisaoBase;
      }
    }
    pthread_create(&thread[i], NULL, threadDistancia, &vetorParametros[i]);
    if(i == concurrentThreadsSupported-1){
      vetorParametros[i].idThread = i;
      break;
    }
  }

  for(int i=0;i<concurrentThreadsSupported;i++){
    pthread_join(thread[i], NULL);
  }
  fimThreads = time(NULL);

  //printaVetor(vetorDistancias);
  speedUp = (difftime(fim, inicio))/(difftime(fimThreads, inicioThreads));

  cout << "tempo gasto para o cálculo de distância sequencial: " << difftime(fim, inicio) << " segundos" << endl;
  cout << "tempo gasto para o cálculo de distância concorrente: " << difftime(fimThreads, inicioThreads) << " segundos" << endl << endl;
  cout << "Speed Up[" << concurrentThreadsSupported << "]: " << speedUp << " segundos" << endl;


}

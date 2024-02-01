#include <iostream>           
#include <thread>             
#include <mutex>              
#include <condition_variable> 
#include "Controleur.hpp" 

using namespace std;

mutex mtx;
condition_variable cv;
Controleur ctrl(0);   // LE PROCESSUS CONTROLEUR

// TrainAB 
void circuleAB(int numTrain){
  cout << "Train n° " << numTrain << " arrive en A vers B " << endl;
  this_thread::sleep_for(chrono::milliseconds(rand() % 1000) );
  unique_lock<mutex> lck(mtx);
    cv.wait(lck,[numTrain]{return ctrl.controlinEnA(numTrain);}); // ATTENTE D'AUTORISATION DE CIRCULER
    lck.unlock();
  // DEBUT DU PARCOURS A->B
  cout << "Train n° "<< numTrain << " circule de A vers B  >>>>>> " << endl;
  this_thread::sleep_for(chrono::milliseconds(rand() % 100) );

  // FIN DU PARCOURS A->B
  cout << "Train n° " << numTrain << " quitte le tronçon de voie unique " << endl;
    lck.lock();
  ctrl.controloutEnB(numTrain);  // SIGNAL DE SORTIE AU CONTROLEUR
    lck.unlock();
  cv.notify_all();
}

// TrainBA 
void circuleBA(int numTrain){
  cout << "Train n° " << numTrain << " arrive en B vers A " << endl;
  this_thread::sleep_for(chrono::milliseconds(rand() % 1000) );
  unique_lock<mutex> lck(mtx);
  cv.wait(lck,[numTrain]{return ctrl.controlinEnB(numTrain);}); // ATTENTE D'AUTORISATION DE CIRCULER
  lck.unlock();
  // DEBUT DU PARCOURSs B->A
  cout << "Train n° "<< numTrain << " circule de B vers A  <<<<<<<<" << endl;
  this_thread::sleep_for(chrono::milliseconds(rand() % 100) );
  
  // FIN DU PARCOURS B->A
  cout << "Train n° " << numTrain << " quitte le tronçon de voie unique " << endl;
   lck.lock();
  ctrl.controloutEnA(numTrain);   // SIGNAL DE SORTIE AU CONTROLEUR
   lck.unlock();
  cv.notify_all();
}

int main(){
  const int nbr = 9;
  srand(time(nullptr));
  thread trainsAB[nbr];
  thread trainsBA[nbr];

  // INITALISE TRAINS ET CONTROLEUR
  for (int i = 1; i < nbr; i++){
    trainsAB[i] = thread(circuleAB, i);
    trainsBA[i] = thread(circuleBA, -i);
  }
// JOIN DES THREADS 
  for (int i = 1; i < nbr; i++) {
    if (trainsAB[i].joinable()) trainsAB[i].join(); 
    if (trainsBA[i].joinable()) trainsBA[i].join();   
  }
  return 0;
}

bool Controleur::controlinEnA(int numero) {
  std::unique_lock<std::mutex> lock(mutex_);
  cv_.wait(lock, [this] { return val == 0 && trainsEnB == 0; }); // Attente jusqu'à ce que val soit égal à 0 et aucun train en B
  val++;
  trainsEnA++;
  return true;
}

bool Controleur::controlinEnB(int numero) {
  std::unique_lock<std::mutex> lock(mutex_);
  cv_.wait(lock, [this] { return val == 0 && trainsEnA == 0; }); // Attente jusqu'à ce que val soit égal à 0 et aucun train en A
  val++;
  trainsEnB++;
  return true;
}

void Controleur::controloutEnB(int numero) {
  std::unique_lock<std::mutex> lock(mutex_);
  val--;
  trainsEnB--;
  lock.unlock();
  cv_.notify_all(); // Notification de tous les threads en attente
}

void Controleur::controloutEnA(int numero) {
  std::unique_lock<std::mutex> lock(mutex_);
  val--;
  trainsEnA--;
  lock.unlock();
  cv_.notify_all(); // Notification de tous les threads en attente
}

#ifndef CONTROLEUR_HPP
#define CONTROLEUR_HPP

#include <thread>             
#include <iostream>
#include <mutex>
#include <condition_variable>

class Controleur {
public:
  Controleur(int valeur_initiale) : val(valeur_initiale), trainsEnA(0), trainsEnB(0) {}

  // Méthode pour permettre à un train de circuler de A vers B
  bool controlinEnA(int numero);

  // Méthode pour permettre à un train de circuler de B vers A
  bool controlinEnB(int numero);

  // Méthode pour signaler la sortie d'un train de A vers B
  void controloutEnB(int numero);

  // Méthode pour signaler la sortie d'un train de B vers A
  void controloutEnA(int numero);

private:
  int val;
  int trainsEnA;
  int trainsEnB;
  std::mutex mutex_;
  std::condition_variable cv_;
};

#endif //CONTROLEUR_HPP

#include <iostream>

#include "controller.h"

int main(){
  Controller ctrl_Fan;
  ctrl_Fan.start();
  std::cout << "Sistema online" << '\n';

  while(ctrl_Fan.inOperation()){
    //codigo paralelo ao controle
    std::this_thread::yield();//para nao bloquear o processo
  };
  std::cout << "Sistema finalizado" << '\n';
  
  return 0;
}

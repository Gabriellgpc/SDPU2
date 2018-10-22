// #include "SocketException.h"
#include <string>
#include <iostream>

#include "socket4L.h"

int main ()
{
  ServerSocket server;
  ServerSocket newSock;
  std::string dataInput;

  server.create();
  server.bind(28500);

  std::cout << "running....\n";
  while(true){
    std::cout << "Aguardando conexao..." << '\n';
    server.listen();
    server.accept(newSock);
    newSock >> dataInput;
    std::cout << "Cliente enviou >> "<< dataInput << '\n';
    newSock << "5";
    newSock.disconnect();
  }

  return 0;
}

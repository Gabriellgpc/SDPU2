#include "socket4L.h"

#include <iostream>
#include <string>

int main ()
{
  // 10.13.113.132
  // "192.168.0.26"
  char ip[] = "10.13.112.73";
  int port = 28500;
  std::string resposta;
  std::string msg;
  std::string valor;
  while(true){
    ClientSocket client_socket ( ip, 28500);

    std::cout << "msg: ";
    std::cin >> msg;

    if(msg == "exit"){
      client_socket.disconnect();
      break;
    }

    std::cout << "Enviando msg ..." << '\n';
    client_socket << msg;      //envia msg via socket

    if(msg == "request"){
      std::cout << "Aguardando resposta..." << '\n';
      client_socket >> resposta; //recebe msg via socket
      std::cout << "Resposta recebido: "<<resposta << '\n';
    }


    // if(msg == "request"){
    //   if(resposta == "OK"){
    //     client_socket >> valor;
    //     std::cout << "Retorno do request: "<< std::stod(valor) << '\n';
    //   }else
    //     std::cout << "Request Erro" << '\n';
    // }

  }
  std::cout << "conexao encerrada..." << '\n';

  return 0;
}

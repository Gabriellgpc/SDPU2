//TODO
#include <system_tools.h>
#include <socket4L.h>

#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h> //getopt, optarg

//WARNING caso o tempo seja muito pequeno, pode haver desconexao
#define timer_step 0.1 //segundos, tempo de amostragem do pwm

#define PORT 28500

void plot(unsigned curve, const std::string &arq);
bool showOptions();
void seedCommand(std::string command, std::string &answer);

std::string ip;

int main(int argc, char** argv)
{
  int c;

  while( (c = getopt(argc, argv, "i:")) != -1)

  switch (c) {
    case 'i':
      ip = optarg;
    break;
  }
  if(ip.size() == 0)
  {
    std::cout << "Informe o IP do servidor:\t";
    std::cin >> ip;
    // std::cin.ignore(255, '\n');
  }
  system("clear");
  std::cout << "IP do servidor definido como:\t"<< ip << '\n';
  sleep(0.3);
  //laco principal
  while(showOptions()){
    sleep(0.3);
  };

  std::cout << "Programa Cliente encerrado com sucesso!" << '\n';

  return 0;
}

void plot(unsigned curve, const std::string &arq){
  std::string path_arq = "../etc/";
  std::string answer;
  std::ofstream O;
  double pwmValue;
  double limitTime = (curve == 1)?30:40;

  path_arq += arq;
  O.open(path_arq.c_str());
  O << std::to_string(curve) << ',';//salva o tipo da curva


  system("clear");
  std::cout << "Iniciando o sistema..." << '\n';
  seedCommand( (curve == 1)?"MODE_1":"MODE_2", answer);
  std::cout << "Capturando dados..." << '\n';
  seedCommand("START", answer);

  double start = tools::clock();
  while(true){
    seedCommand("REQUEST", answer);
    pwmValue = std::stod(answer);

    O << pwmValue << ", ";

    if((tools::clock() - start ) > limitTime)
      break;

    tools::sleep(timer_step);
  }
  O << 0;
  std::cout << "Plot finalizado, dados do PWM salvo em:\t"<< path_arq << '\n';
  O.close();
}

bool showOptions(){
  unsigned int op;
  char c;
  std::string answer;


  do{
    std::cout << "1- Iniciar sistema" << '\n';
    std::cout << "2- Parar o sistema" << '\n';
    std::cout << "3- Plot" << '\n';
    std::cout << "4- Leitura atual da saida PWM" << '\n';
    std::cout << "5- Leitura do sensor de Temperatura" << '\n';
    std::cout << "6- Leitura do sensor de luminosidade" << '\n';
    std::cout << "7- Desligar o sistema" << '\n';
    std::cout << "8- Mudar comportamento do sistema(curva)" << '\n';
    std::cout << "9- Encerrar o programa cliente" << '\n';
    std::cout << "10- Visualizar o grafico" << '\n';
    std::cout << "Escolha uma opcao valida..." << '\n';

    std::cin >> op;
    system("clear");
    if(op > 10 || op == 0)
    {
      std::cout <<"Opcao invalida...\n" << '\n';
    }
  }while(op > 10 || op == 0);

  //Tratando a escolha
  switch (op) {
    case 1:
      seedCommand("START", answer);
      std::cout << "Sistema iniciado" << '\n';
    break;
    case 2:
      seedCommand("STOP", answer);
      std::cout << "Sistema interrompido" << '\n';
    break;
    case 3:
      do{
        system("clear");
        std::cout << "Escolha o tipo da curva" << '\n';
        std::cout << "1- Curva Padrao" << '\n';
        std::cout << "2- Curva 2" << '\n';
        std::cin >> op;

        if(op > 2 || op == 0)
        {
          std::cout << "Curva invalida..." << '\n';
          std::cout << "Voce digitou:\t"<< op << '\n';
          std::cout << "Press to continue..." << '\n';
          std::cin.get(c);
          std::cin.ignore(1,'\n');
        }
        else
        {
          plot(op, tools::getData());//salva os dados num arquivo cujo nome eh a data e hora atual do sistema
        }
        tools::sleep(0.3);
        std::cout << "Press to continue..." << '\n';
        std::cin.get(c);
        std::cin.ignore(1,'\n');
      }while( op > 2 || op == 0);

    break;
    case 4:
      seedCommand("REQUEST", answer);
      std::cout << "Valor atual do PWM:\t"<< std::stod(answer) << '%' <<'\n';

      std::cout << "Press to continue..." << '\n';
      std::cin.get(c);
      std::cin.ignore(1,'\n');
    break;
    case 5:
      seedCommand("TEMP",answer);
      std::cout << "Leitura do sensor de Temperatura:\t"<< std::stod(answer)<< 'C' << '\n';

      std::cout << "Press to continue..." << '\n';
      std::cin.get(c);
      std::cin.ignore(1,'\n');
    break;
    case 6:
      seedCommand("LUMIN",answer);
      std::cout << "Leitura do sensor de Luminosidade:\t"<< std::stod(answer)<< "lm" << '\n';

      std::cout << "Press to continue..." << '\n';
      std::cin.get(c);
      std::cin.ignore(1,'\n');
    break;
    case 7:
      seedCommand("SHUTDOWN", answer);
      std::cout << "Sistema Encerrado" << '\n';
      std::cout << "Desconectando o cliente..." << '\n';
      return false;
    break;
    case 8:
      do{
        system("clear");
        std::cout << "Define o comportamento do sistema" << '\n';
        std::cout << "1- Curva Padrao" << '\n';
        std::cout << "2- Curva 2" << '\n';
        std::cin >> op;

        if(op > 2 || op == 0)
        {
          std::cout << "Curva invalida..." << '\n';
          std::cout << "Voce digitou:\t"<< op << '\n';
          std::cout << "Press to continue..." << '\n';
          std::cin.get(c);
          std::cin.ignore(1,'\n');
        }
        else
        {
          seedCommand( (op == 1)?"MODE_1":"MODE_2", answer );
        }
      }while( op > 2 || op == 0);
    break;
    case 9:
      system("clear");
      std::cout << "Encerrando o cliente..." << '\n';
      return false;
    break;
    case 10:
      system("python3 ../etc/ploter.py");
      system("clear");
    break;
    default:
      std::cerr << "Erro, nao deveria ter chegado aqui..." << '\n';
  }
  return true;
}

void seedCommand(std::string command, std::string &answer){
  ClientSocket client_socket ( ip.c_str(), PORT);
  client_socket << command;

  tools::str2upper(command);
  if(command == "REQUEST" || command == "TEMP" || command == "LUMIN")
    client_socket >> answer;

  client_socket.disconnect();
}

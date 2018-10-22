#include <string>
#include <iostream>
#include <mutex>
#include "controller.h"

//Funcoes auxiliares
void func_pinController(const void*X){
  ((Controller*)X)->pinController();
}
void func_comunication(const void*X){
  ((Controller*)X)->comunication();
}
void func_acquisition(const void*X){
  ((Controller*)X)->acquisition();
}

void func_button_interrupt(void*X){
  ((Controller*)X)->botton_interrupt();
}
std::mutex mtx;
// Metodos da classe controle


Controller::Controller():
  serverCtrl(),
  mode(DEFAULT),
  enable(false),
  newData(false),
  temp(0),
  lumin(0),
  finish(false),
  pwmValueDrive(0),
  timeRef_debounce(0),
  gpioLedEnable(PIN::LedEnable),
  gpioButton(PIN::Button),
  aioLumi(PIN::SensorLumi),
  aioTemp(PIN::SensorTemp),
  pwmLedSensor(PIN::LedSensor),
  pwmDriveFan(PIN::DriveFan),
  thr_acquisition(func_acquisition, (const void*)this),
  thr_pinController(func_pinController, (const void*)this),
  thr_comunication(func_comunication, (const void*)this)
{
  //configuracao dos pinos
  gpioLedEnable.dir(mraa::DIR_OUT);
  gpioButton.dir(mraa::DIR_IN);

  gpioLedEnable.write(0);
  gpioButton.write(0);
  //interrupcao para borda de descida
  gpioButton.isr(mraa::EDGE_FALLING,func_button_interrupt, (void*)this);

  //habilita as saidas pwm
  pwmLedSensor.enable(true);
  pwmDriveFan.enable(true);
}


Controller::~Controller(){
  this->shutdown();
}

void Controller::botton_interrupt(){
  if((tools::clock() - timeRef_debounce) > TIME_DEBOUNCE){
    enable = !enable;
  }
  timeRef_debounce = tools::clock();
}

void Controller::pinController(){

  while(!finish)
  {
    while(!enable)//aguarda o sistema ser habilitado
    {
      if(finish)break;
      std::this_thread::yield();
    }
    //funcionamento do sistema habilitado
    gpioLedEnable.write(1);
    double t_start = tools::clock();
    while(enable){
      //pedido de leitura dos sensores
      newData = true;
      while(newData){
        if(finish)break;
        std::this_thread::yield();
      };//aguarda a leitura dos dados
      //entrando na zona critica
      mtx.lock();
      pwmValueDrive = curve(tools::clock() - t_start)*(temp + REF_4_TEMP)*ALPHA + lumin*BETA*0.05;
      //saindo da zona critica
      pwmDriveFan.write(pwmValueDrive);
      pwmLedSensor.write(lumin*BETA);
      mtx.unlock();
    };
    //sistema passou de habilitado para desabilitado
    gpioLedEnable.write(0);
    pwmDriveFan.write(0);
    pwmLedSensor.write(0);
    pwmValueDrive = 0;
  }

}

void Controller::acquisition(){

  while(!finish){
    while((this->newData == false) && !enable)//aguarda pedido de novos dados
    {
      if(finish)break;
      std::this_thread::yield();//parar a execução do thread atual e dar prioridade a outros processos
    }
    //zona critica
    mtx.lock();
    this->temp    = aioTemp.read()*(REF_4_TEMP/512.0);
    this->lumin   = aioLumi.read()*(REF_4_LUMI/512.0);
    this->newData = false;
    //fim da zona critica
    mtx.unlock();
  }

}

void Controller::comunication(){
  ServerSocket newSock;
  std::string command;
  std::string arg;

  serverCtrl.create();
  serverCtrl.bind(PORT);

  while(!finish){
    serverCtrl.listen();
    serverCtrl.accept(newSock);

    newSock >> command;
    tools::str2upper(command);

    if(command == "REQUEST")
    {
      while(newData){} //aguarda a thread de acquisition terminar de adquirir novos dados
      newData = false;
      mtx.lock();
      newSock << std::to_string(pwmValueDrive);
      mtx.unlock();
    }
    else if(command == "START")
    {
      enable = true;
    }
    else if(command == "STOP")
    {
      enable = false;
    }
    else if(command == "SHUTDOWN")
    {
      finish = true;
      enable = false;
    }
    else if(command == "MODE_1")
    {
      this->setMode(MODE1);
      std::cout << "Curva 1 selecionada" << '\n';
    }else if(command == "MODE_2")
    {
      this->setMode(MODE2);
      std::cout << "Curva 2 selecionada" << '\n';
    }else if(command == "TEMP")
    {
      mtx.lock();
      newSock << std::to_string(temp);
      mtx.unlock();
    }else if(command == "LUMIN")
    {
      mtx.lock();
      newSock << std::to_string(lumin);
      mtx.unlock();
    }
    else
    {
      newSock << "INVALID_COMMAND";
    }
    newSock.disconnect();
  }

}
void Controller::shutdown(){

  pwmLedSensor.enable(false);
  pwmDriveFan.enable(false);

  gpioLedEnable.write(0);
  gpioButton.write(0);

  finish = true;
  enable = false;


  thr_pinController.join();
  thr_acquisition.join();
  thr_comunication.join();
};

void Controller::setMode(const MODE &new_mode){
  this->mode = new_mode;
  this->enable = false;
}

float Controller::curve(const float& t){
  switch (mode){
    case MODE1:
    case DEFAULT:
      return this->curve01(t);
    break;
    case MODE2:
      return this->curve02(t);
    break;
    default:
      // std::cerr << "Modo de operacao invalido" << '\n';
      return 0;
    break;
  }
}

float Controller::curve01(const float& t){

  if( t <= 10 && t >= 0)
    return (0.35/10)*t;

  if( t > 10 && t <= 15)
    return 0.35;

  if( t > 15 && t <= 20)
    return (0.06)*(t-15) + 0.35;

  if( t > 20 && t <= 25)
    return 0.65;

  if( t > 25 && t <= 30)
    return (-0.13)*(t-25) + 0.65;
  //outros casos, erro ou final da curva
  // std::cout << "Fim da curva 1" << '\n';
  this->stop();
  return 0;
}

float Controller::curve02(const float& t){
  if( t <= 10 && t >= 0)
    return (t*t*0.004);

  if( t > 10 && t <= 15)
    return 0.4;

  if( t > 15 && t <= 18)
    return (0.1334)*(t-15) + 0.4;

  if( t > 18 && t <= 20)
    return 0.8;

  if( t > 20  && t <= 25)
    return (-0.08)*(t-20) + 0.8;

  if( t > 25  && t <= 30)
    return 0.4;
  if( t > 30 && t <= 40)
    return (-0.04)*(t-30) + 0.4;
  //outros casos, erro ou final da curva
  // std::cout << "Fim da curva 2" << '\n';
  this->stop();
  return 0;
}

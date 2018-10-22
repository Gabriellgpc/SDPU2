//*** CLASSE PRINCIPAL ***//
#include <thread> // std::thread

#include <mraa/common.hpp>
#include <mraa/gpio.hpp>
#include <mraa/pwm.hpp>
#include <mraa/aio.hpp>

#include <socket4L.h> //class Socket_4Linux
#include <system_tools.h>

#define PORT 28500
//para minimizar o efeito do debounce no botao de acionamento
#define TIME_DEBOUNCE 0.3 //segundos
#define REF_4_TEMP  27.0   //graus Celcius
#define REF_4_LUMI  500.0  //unidade de luminocidade
#define ALPHA 1.0/54     // inverso da maxima temperatura esperada
#define BETA 1.0/1000    // inverso da maxima luminancia esperada
// Modos de operacao
// Defininem o comportamento do secador de graos
enum MODE{
  DEFAULT,//gera a curva padrao
  MODE1,  //igual ao modo Default
  MODE2
  // ...
};

class Controller
{
private:
  ServerSocket serverCtrl;
  MODE mode;
  bool finish; //Flag para desligar o sistema
  bool enable;  //Flag para ativar/desativar o sistema, True: habilitado, False: desabilitado

  mraa::Gpio gpioLedEnable;
  mraa::Gpio gpioButton;
  mraa::Aio  aioLumi;
  mraa::Aio  aioTemp;
  mraa::Pwm  pwmLedSensor;
  mraa::Pwm  pwmDriveFan;

  double timeRef_debounce;
  volatile std::atomic<bool> newData; //Ha dados dos sensores para serem lidos
  volatile std::atomic<uint8_t> temp; //valor de temperatura em graus celcius
  volatile std::atomic<uint8_t> lumin;//valor da luminancia
  volatile std::atomic<float> pwmValueDrive;

  // thread para leitura de dados
  std::thread thr_acquisition;
  //thread que realizara o contro dos pinos
  std::thread thr_pinController;
  //thread responsavel pelo recebimento e envio de dados
  std::thread thr_comunication;

  //Funcao que sera associada a interrupcao do botao
  void botton_interrupt();

  enum PIN{
    LedEnable  = 8, // Pino do led para indicar o estados do sistema: ON/OFF
    LedSensor  = 5, // Pino do Led associado a a leitura de um dos sensores
    Button     = 9, // Pino associado ao botao
    DriveFan   = 3, // Pino para PWM do motor
    SensorLumi = 1, // Pino para leitura AD do sensor de luminancia
    SensorTemp = 0  // Pino para leitura AD do sensor de temperatura
  };

  //Funcoes que ditam o comportamento da saida PWM para o motor
  //retornam o valor do PWM
  //equivalente ao Z(t) no projeto
  float curve(const float& t); //este eh o metodo que deve ser chamado para o retorno final(ele chaveia entre os demais, de acordo com o modo atual)
  float curve01(const float& t); //curva padrao
  float curve02(const float& t); //curva 2
  //Metodos para as threads:
  void pinController();
  void comunication();
  void acquisition();

  //Funcoes auxiliares(para permitir usar metodos nas threads)
  friend void func_pinController(const void*X);
  friend void func_comunication(const void*X);
  friend void func_acquisition(const void*X);
  friend void func_button_interrupt(void*X);
  //desliga o sistema e encerra o processo
  // void shutdown();
public:
  Controller();
  ~Controller();

  //Retorna o status do sistema
  inline bool inOperation()const {return  !finish;}
  //Aprincipio so sera usado por comandos via socket, logo n sera necessario este metodo para o main
  MODE getMode()const {return mode;};
  void setMode(const MODE &new_mode);//Aprincipio so sera usado por comandos via socket, logo n sera necessario este metodo para o main
  //retoma o sistema da onde parou, ou inicia do zero
  inline void start(){ this->enable = true; };
  //Suspende o sistema temporariamente
  //ao ser religado com start(), o sistema retoma
  //da onde parou
  inline void stop(){ this->enable = false; };//so desabilita o sistema, mas n encerra o processo
  //desliga o sistema e encerra o processo
  void shutdown();
};

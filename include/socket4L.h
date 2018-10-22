#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>

const int MAXHOSTNAME = 200;
const int MAXCONNECTIONS = 5;
const int MAXRECV = 500;

class Socket{
private:
  int m_sock;
  sockaddr_in m_addr;
  bool connected;
public:
 Socket();
 virtual ~Socket();

 // Server initialization
 bool create();
 bool bind ( const int port );
 bool listen() const;
 bool accept ( Socket& ) const;

 // Client initialization
 bool connect ( const std::string host, const int port);
 bool disconnect();
 // Data Transimission
 bool send(const std::string) const;
 int recv(std::string&) const;

 const Socket& operator<<( const std::string& )const;
 const Socket& operator>>( std::string& ) const;

 // const Socket& operator<<(const std::string &msg);
 // const Socket& operator>>(std::string &msg);

 void set_non_blocking ( const bool );
 bool is_valid() const { return m_sock != -1; }
};
/** ################################################################**/

class ServerSocket: public Socket{
public:
 ServerSocket ( int port );
 ServerSocket (){};
 virtual ~ServerSocket();

 void accept ( ServerSocket& );
 // WARNING Implementacoes futuras:
 // void close(ServerSocket& );
};

/** ################################################################**/
// Definition of the ClientSocket class

class ClientSocket: public Socket
{
 public:
  ClientSocket ( std::string host, int port );
  virtual ~ClientSocket(){};

};

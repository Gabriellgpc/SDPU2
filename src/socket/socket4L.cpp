#include <iostream>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "socket4L.h"

Socket::Socket() :
  m_sock (-1),
  connected(false)
{
  memset ( &m_addr,0,sizeof ( m_addr ) );
}

Socket::~Socket()
{
  if ( is_valid() )
    ::close ( m_sock );
}

bool Socket::create()
{
  m_sock = socket (AF_INET, SOCK_STREAM, 0);

  if ( ! is_valid() )
    return false;

  // TIME_WAIT - argh
  int on = 1;
  if ( setsockopt ( m_sock, SOL_SOCKET, SO_REUSEADDR, ( const char* ) &on, sizeof ( on ) ) == -1 )
    return false;

  return true;
}



bool Socket::bind ( const int port )
{

  if ( ! is_valid() )
    {
      return false;
    }

  m_addr.sin_family = AF_INET;
  m_addr.sin_addr.s_addr = INADDR_ANY;
  m_addr.sin_port = htons ( port );

  int bind_return = ::bind ( m_sock,
			     ( struct sockaddr * ) &m_addr,
			     sizeof ( m_addr ) );


  if ( bind_return == -1 )
    {
      return false;
    }

  return true;
}


bool Socket::listen() const
{
  if ( ! is_valid() )
    {
      return false;
    }

  int listen_return = ::listen ( m_sock, MAXCONNECTIONS);

  if ( listen_return == -1 )
  {
    return false;
  }

  return true;
}


bool Socket::accept ( Socket& new_socket ) const
{
  int addr_length = sizeof ( m_addr );
  new_socket.m_sock = ::accept ( m_sock, ( sockaddr * ) &m_addr, ( socklen_t * ) &addr_length );

  if ( new_socket.m_sock <= 0 )
    return false;
  else
    return true;
}


bool Socket::send ( const std::string s ) const
{
  int status = ::send ( m_sock, s.c_str(), s.size(), MSG_NOSIGNAL );
  if ( status == -1 )
    {
      return false;
    }
  else
    {
      return true;
    }
}


int Socket::recv ( std::string& s ) const
{
  char buf [ MAXRECV + 1 ];
  s = "";
  memset ( buf, 0, MAXRECV + 1 );
  int status = ::recv ( m_sock, buf, MAXRECV, 0 );
  if ( status == -1 )
  {
    std::cout << "status == -1   errno == " << errno << "  in Socket::recv\n";
    return 0;
  }
  else if ( status == 0 )
    {
      return 0;
    }
  else
    {
      s = buf;
      return status;
    }
}


bool Socket::connect ( const std::string host, const int port )
{
  if ( ! is_valid() ) return false;

  m_addr.sin_family = AF_INET;
  m_addr.sin_port = htons ( port );

  int status = inet_pton ( AF_INET, host.c_str(), &m_addr.sin_addr );

  if ( errno == EAFNOSUPPORT ) return false;

  status = ::connect ( m_sock, ( sockaddr * ) &m_addr, sizeof ( m_addr ) );

  if ( status == 0 ){
    connected = true;
    return true;
  }
  else{
    connected = false;
    return false;
  }
}

bool Socket::disconnect(){
  if(!connected)
    return false;
  close(m_sock);
  connected = false;
  return true;
}

void Socket::set_non_blocking ( const bool b )
{

  int opts;

  opts = fcntl(m_sock,F_GETFL);

  if ( opts < 0 )
  {
    return;
  }

  if ( b )
    opts = ( opts | O_NONBLOCK );
  else
    opts = ( opts & ~O_NONBLOCK );

  fcntl ( m_sock,
	  F_SETFL,opts );

}


const Socket& Socket::operator << ( const std::string& s ) const
{
  if ( !Socket::send(s)){
    // throw SocketException ( "Could not write to socket." );
    std::cerr << "Could not write to socket" << '\n';
  }
  return *this;
}

const Socket& Socket::operator >> ( std::string& s ) const
{
  if (!Socket::recv(s)){
    // throw SocketException ( "Could not read from socket." );
    std::cerr << "Could not read from socket." << '\n';
    // exit(1);

  }
  return *this;
}

/*##################################################################*/
ServerSocket::ServerSocket( int port )
{
  if ( ! Socket::create() )
    {
      // throw SocketException ( "Could not create server socket." );
      std::cerr << "Could not create server socket." << '\n';
      exit(1);
    }

  if ( ! Socket::bind ( port ) )
    {
      // throw SocketException ( "Could not bind to port." );
      std::cerr << "Could not bind to port." << '\n';
      exit(1);
    }

  if ( ! Socket::listen() )
    {
      // throw SocketException ( "Could not listen to socket." );
      std::cerr << "Could not listen to scoket" << '\n';
      exit(1);
    }

}

ServerSocket::~ServerSocket()
{
}

// const ServerSocket& ServerSocket::operator << ( const std::string& s ) const
// {
//   if ( !Socket::send(s)){
//     // throw SocketException ( "Could not write to socket." );
//     std::cerr << "Could not write to socket" << '\n';
//     exit(1);
//   }
//   return *this;
// }
//
// const ServerSocket& ServerSocket::operator >> ( std::string& s ) const
// {
//   if (!Socket::recv(s)){
//     // throw SocketException ( "Could not read from socket." );
//     std::cerr << "Could not read from socket." << '\n';
//     exit(1);
//   }
//   return *this;
// }

void ServerSocket::accept ( ServerSocket& sock)
{
  if ( !Socket::accept(sock )){
    // throw SocketException ( "Could not accept socket." );
    std::cerr <<  "Could not accept socket." << '\n';
    exit(1);
  }
}

//#########################################################
ClientSocket::ClientSocket ( std::string host, int port )
{
  if ( ! Socket::create() )
    {
      // throw SocketException ( "Could not create client socket." );
      std::cerr << "Could not create client socket." << '\n';
      exit(1);
    }

  if ( ! Socket::connect ( host, port ) )
    {
      // throw SocketException ( "Could not bind to port." );
      std::cerr << "Could not bind to port." << '\n';
      exit(1);
    }

}


// const ClientSocket& ClientSocket::operator << ( const std::string& s ) const
// {
//   if ( ! Socket::send ( s ) )
//     {
//       // throw SocketException ( "Could not write to socket." );
//       std::cerr << "Could not write to socket." << '\n';
//       exit(1);
//     }
//
//   return *this;
//
// }
//
//
// const ClientSocket& ClientSocket::operator >> ( std::string& s ) const
// {
//   if ( ! Socket::recv ( s ) )
//     {
//       // throw SocketException ( "Could not read from socket." );
//       std::cerr << "Could not read from socket." << '\n';
//       exit(1);
//     }
//
//   return *this;
// }

#ifndef	__SOCKET_IO_H__
#define	__SOCKET_IO_H__

#include <WinSock2.h>
#include <memory>

class SocketIO;
typedef	std::tr1::shared_ptr< SocketIO >	SocketIOPtr;
typedef	std::tr1::weak_ptr< SocketIO >	WeakSocketIOPtr;

class SocketIO
{
public:
	SocketIO( SOCKET s )
		: _socket( s ) ,
		_initFlag( false )
	{}
	virtual ~SocketIO(void)
	{
		
	}

public:
	WeakSocketIOPtr		__self;								//	对自己的弱引用
	SOCKET				_socket;
	bool				_initFlag;							//	是否已经初始化
public:
	//	设置对自己的弱引用
	virtual	void	SetPtr( SocketIOPtr ptr )
	{
		__self = ptr;
	}
};
#endif	/*__SOCKET_IO_H__*/

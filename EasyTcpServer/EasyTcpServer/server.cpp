#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#include<WinSock2.h> //此头文件必须在前面
#include<windows.h>  //windows socket
#include<iostream>
#include <stdio.h>
#include<string.h>

#pragma comment(lib,"ws2_32.lib")//添加链接库
enum CMD {//表示命令的类型
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_ERROR
};
struct DataHeader {//包头
	short dataLength;//数据长度
	short cmd;//命令
};
//DataPackage
struct Login :public DataHeader{//登陆
	Login() {
		dataLength = sizeof(Login);//根据继承来的头部变化的设置信息
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char PassWord[32];
};
struct LoginResult:public DataHeader {
	LoginResult() {
		dataLength = sizeof(LoginResult);
		cmd = CMD_LOGIN_RESULT;
		result = 0;
	}
	int result;
};
struct Logout :public DataHeader {//登出
	Logout() {
		dataLength = sizeof(Logout);
		cmd = CMD_LOGOUT;
	}
	char userName[32];
};
struct LogoutResult :public DataHeader {
	LogoutResult() {
		dataLength = sizeof(LogoutResult);
		cmd = CMD_LOGOUT_RESULT;
		result = 0;
	}
	int result;
};
int main() {
	WORD ver = MAKEWORD(2, 2);//表示使用WINSOCK2版本
	WSADATA dat;//第二个参数是指向WSADATA的指针.该函数返回一个INT型值, 通过检查这个值来确定初始化是否成功
	WSAStartup(ver, &dat);
	//1 建立一个socket
	SOCKET _sock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//ipv4,数据类型,tcp/udp
	
    //2 bind 绑定用于接受客户端连接的网络端口
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;//协议类型
	_sin.sin_port = htons(4567);//主机和网络数据序不同，要进行转换
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;//哪个地址都行//inet_addr("127.0.0.1")服务器绑定到主机地址(本机地址所以要inet_addr转换)
	if (SOCKET_ERROR == bind(_sock, (sockaddr*)&_sin, sizeof(_sin)))//监听的目的是判断绑定是否成功,bind函数：socket类型，socket的网路地址描述数据，数据长度
	{
		std::cout << "错误,绑定用于接受客户端接连的网络端口失败..." << std::endl;
	}
	else{
		std::cout << "绑定网络端口成功..." << std::endl;
	}
	//3 listen 监听网络端口
	if (SOCKET_ERROR == listen(_sock, 5))//第二个参数表示可以有多少人连接
	{
		std::cout << "错误,监听网络端口失败..." << std::endl;
	}
	else {
		std::cout << "监听网络端口成功..." << std::endl;
	}

	//4 accepet等待客户端连接
	sockaddr_in clientAddr = {};
	int nAddrLen = sizeof(sockaddr_in);
	SOCKET _cSock = INVALID_SOCKET;//定义为一个不用的socket地址
	
	_cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
	if (INVALID_SOCKET == _cSock) {
		std::cout << "错误，接收到无效客户端SOCKET...\n" << std::endl;
	}
	else {
		printf_s("新客户端加入:socket=%d,IP=%s \n", (int)_cSock,inet_ntoa(clientAddr.sin_addr));//将客户端地址转化为可读类型，本函数将一个用in参数所表示的Internet地址结构转换成以“.” 间隔的诸如“a.b.c.d”的字符串形式
	}

	while (true) {//服务端循环接受数据
		//缓冲区
		char szRecv[1024] = {};
		//5 接收客户端的数据
		int nLen = recv(_cSock, szRecv, sizeof(DataHeader), 0);//先接收头，注意参数2和参数3不来自同一个对象
		DataHeader* header=(DataHeader*)szRecv;
		if (nLen <= 0) {
			printf("客户端已退出,任务结束。\n");
			break;
		}
		switch (header->cmd) {
		case CMD_LOGIN:
		{
			
			recv(_cSock, szRecv+sizeof(DataHeader), header->dataLength-sizeof(DataHeader), 0);
			Login* login = (Login*)szRecv;
			printf("收到命令:CMD_LOGIN,数据长度:%d,用户名:%s,密码：%s\n", login->dataLength, login->userName, login->PassWord);
			//忽略判断用户名、密码是否正确的过程
			LoginResult ret ;
			send(_cSock, (char*)&ret, sizeof(LoginResult), 0);
		}
		break;
		case CMD_LOGOUT:
		{
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength-sizeof(DataHeader), 0);
			Logout* loginout = (Logout*)szRecv;
			printf("收到命令:CMD_LOGOUT,数据长度:%d,用户名:%s\n", loginout->dataLength,loginout->userName);
			//忽略判断用户名、密码是否正确的过程
			LogoutResult ret;
			send(_cSock, (char*)&ret, sizeof(ret), 0);

		}
		break;
		default: 
		{
			DataHeader header = { 0,CMD_ERROR };
			send(_cSock, (char*)&header, sizeof(header), 0);
		}
		break;
		}
	}

	//6 关闭套接字closesocket
	closesocket(_sock);
	printf("退出,任务结束。\n");
	WSACleanup();
	getchar();
	return 0;
}



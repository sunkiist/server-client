#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS //为了能写scanf
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
struct Login :public DataHeader {//登陆
	Login() {
		dataLength = sizeof(Login);//根据继承来的头部变化的设置信息
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char PassWord[32];
};
struct LoginResult :public DataHeader {
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
	SOCKET _sock=socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == _sock) {
		printf("错误，建立socket失败...\n");
	}
	else {
		printf("成功,建立socket成功\n");
	}
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int ret=connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
	if (SOCKET_ERROR == ret) {
		printf("错误，连接服务器失败\n");
	}
	else {
		printf("成功，连接服务器成功\n");
		
	}
	
	while (true) {
		//3 输入请求命令
		char cmdBuf[128] = {};
		scanf("%s", cmdBuf);
		//4 处理请求
		if (0 == strcmp(cmdBuf, "exit"))
		{
			printf("接收到exit命令，任务结束\n");
			break;
		}
		else if (0==strcmp(cmdBuf,"login")){
			Login login;
			strcpy(login.userName, "gyf");
			strcpy(login.PassWord, "33333333");
			//向服务器发送数据
			send(_sock, (const char*)&login, sizeof(login) , 0);
			//接收服务器返回的数据
			LoginResult loginRet = {};
			recv(_sock, (char*)&loginRet, sizeof(loginRet), 0);
			printf("LoginResult: %d\n", loginRet.result);
		}
		else if (0 == strcmp(cmdBuf, "logout")) {
			Logout logout;
			strcpy(logout.userName, "gyf");
			//向服务器发送数据
			send(_sock, (const char*)&logout, sizeof(logout) , 0);
			//接收服务器返回的数据
			LogoutResult logoutRet = {};
			recv(_sock, (char*)&logoutRet, sizeof(logoutRet), 0);
			printf("LogoutResult: %d\n", logoutRet.result);
		}
		else {
			printf("收到不支持的命令，请重新输入。\n");
		}
	}
	//7 关闭套接字closesocket
	closesocket(_sock);
	//清除Windows socket环境
	WSACleanup();
	printf("已退出。");
	getchar();
	return 0;
}
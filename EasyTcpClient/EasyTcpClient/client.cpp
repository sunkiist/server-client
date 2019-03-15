#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS //Ϊ����дscanf
#include<WinSock2.h> //��ͷ�ļ�������ǰ��
#include<windows.h>  //windows socket
#include<iostream>
#include <stdio.h>
#include<string.h>
#pragma comment(lib,"ws2_32.lib")//������ӿ�

enum CMD {//��ʾ���������
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_ERROR
};
struct DataHeader {//��ͷ
	short dataLength;//���ݳ���
	short cmd;//����
};
//DataPackage
struct Login :public DataHeader {//��½
	Login() {
		dataLength = sizeof(Login);//���ݼ̳�����ͷ���仯��������Ϣ
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
struct Logout :public DataHeader {//�ǳ�
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
	WORD ver = MAKEWORD(2, 2);//��ʾʹ��WINSOCK2�汾
	WSADATA dat;//�ڶ���������ָ��WSADATA��ָ��.�ú�������һ��INT��ֵ, ͨ��������ֵ��ȷ����ʼ���Ƿ�ɹ�
	WSAStartup(ver, &dat);
	SOCKET _sock=socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == _sock) {
		printf("���󣬽���socketʧ��...\n");
	}
	else {
		printf("�ɹ�,����socket�ɹ�\n");
	}
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int ret=connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
	if (SOCKET_ERROR == ret) {
		printf("�������ӷ�����ʧ��\n");
	}
	else {
		printf("�ɹ������ӷ������ɹ�\n");
		
	}
	
	while (true) {
		//3 ������������
		char cmdBuf[128] = {};
		scanf("%s", cmdBuf);
		//4 ��������
		if (0 == strcmp(cmdBuf, "exit"))
		{
			printf("���յ�exit����������\n");
			break;
		}
		else if (0==strcmp(cmdBuf,"login")){
			Login login;
			strcpy(login.userName, "gyf");
			strcpy(login.PassWord, "33333333");
			//���������������
			send(_sock, (const char*)&login, sizeof(login) , 0);
			//���շ��������ص�����
			LoginResult loginRet = {};
			recv(_sock, (char*)&loginRet, sizeof(loginRet), 0);
			printf("LoginResult: %d\n", loginRet.result);
		}
		else if (0 == strcmp(cmdBuf, "logout")) {
			Logout logout;
			strcpy(logout.userName, "gyf");
			//���������������
			send(_sock, (const char*)&logout, sizeof(logout) , 0);
			//���շ��������ص�����
			LogoutResult logoutRet = {};
			recv(_sock, (char*)&logoutRet, sizeof(logoutRet), 0);
			printf("LogoutResult: %d\n", logoutRet.result);
		}
		else {
			printf("�յ���֧�ֵ�������������롣\n");
		}
	}
	//7 �ر��׽���closesocket
	closesocket(_sock);
	//���Windows socket����
	WSACleanup();
	printf("���˳���");
	getchar();
	return 0;
}
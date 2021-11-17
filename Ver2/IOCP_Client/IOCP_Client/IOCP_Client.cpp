#include <winsock2.h>
#include <stdio.h>

#pragma comment(lib,"ws2_32.lib")
#pragma warning (disable:4996)

#define MAX_BUFFER 1024
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 3500

struct SOCKETINFO
{
	WSAOVERLAPPED overlapped;
	WSABUF dataBuffer;
	int receiveBytes;
	int sendBytes;
};

int main(int argc, char *argv[])
{
	// Winsock start - winsock.dll Code
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 0), &WSAData) != 0)
	{
		printf("Error - Cannot load 'winsock.dll' file\n");
		return 1;
	}

	// Socket 생성
	SOCKET listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (listenSocket == INVALID_SOCKET)
	{
		printf("Error - Invaild socket\n");
		return 1;
	}

	// 서버정보 객체설정
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);

	// 연결 요청
	if (connect(listenSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		printf("Error - Fail to connect\n");
		// 소켓 종료
		closesocket(listenSocket);
		// Winsock End
		WSACleanup();
		return 1;
	}
	else
	{
		printf("Server Connected\n* Enter Message\n->");
	}
	SOCKETINFO *socketInfo;
	DWORD sendBytes;
	DWORD receiveBytes;
	DWORD flags;

	while (1)
	{
		// 메시지 입력
		char messageBuffer[MAX_BUFFER];
		int i, bufferLen;
		for (i = 0; ; i++)
		{
			messageBuffer[i] = getchar();
			if (messageBuffer[i] == '\n')
			{
				messageBuffer[i++] = '\0';
				break;
			}
		}
		bufferLen = i;

		socketInfo = (struct SOCKETINFO *) malloc(sizeof(struct SOCKETINFO));
		memset((void *)socketInfo, 0x00, sizeof(struct SOCKETINFO));
		socketInfo->dataBuffer.len = bufferLen;
		socketInfo->dataBuffer.buf = messageBuffer;

		// 데이터 쓰기
		int sendBytes = send(listenSocket, messageBuffer, bufferLen, 0);
		if (sendBytes > 0)
		{
			printf("TRACE - Send message : %s (%d bytes)\n", messageBuffer, sendBytes);
			// 데이터 읽기
			int receiveBytes = recv(listenSocket, messageBuffer, MAX_BUFFER, 0);
			if (receiveBytes > 0)
			{
				printf("TRACE - Receive message : %s (%d bytes)\n* Enter Message\n->", messageBuffer, receiveBytes);
			}
		}
	}

	// 소켓종료
	closesocket(listenSocket);

	// Winsock End
	WSACleanup();

	return 0;
}
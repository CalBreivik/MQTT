#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdarg.h>
#include "MQTT_Header.h"
#include "MQTT_Config.h"
#pragma comment(lib,"ws2_32.lib")

SOCKET TCP_Socket;
PMQTT PFixed_Header_Remaining = NULL;
PMQTT PFixed_Header = NULL;
PMQTT PVariable_Header = NULL;
PMQTT Payload = NULL;
PMQTT PMQTT_Packet = NULL;
char *utf8tstr = NULL;
char *IP = NULL;
char *Port = NULL;

int main(int argc, char *argv[])
{
	char *ClientId, *UserName, *PassWord;
	char Recv_Data[1024];
	int i, byte = 0;
	/*GBK_To_UTF8("35107354");
	ClientId = (char *)malloc(sizeof(char) * (strlen(utf8tstr) + 1));
	memset(ClientId, 0, strlen(utf8tstr) + 1);
	memcpy(ClientId, utf8tstr, strlen(utf8tstr));
	free(utf8tstr);
	utf8tstr = NULL;
	GBK_To_UTF8("151131");
	UserName = (char *)malloc(sizeof(char) * (strlen(utf8tstr) + 1));
	memset(UserName, 0, strlen(utf8tstr) + 1);
	memcpy(UserName, utf8tstr, strlen(utf8tstr));
	free(utf8tstr);
	utf8tstr = NULL;
	//GBK_To_UTF8("12345");
	GBK_To_UTF8("nggVk6KyAEBnYDybpGxW1jkpuCA=");
	PassWord = (char *)malloc(sizeof(char) * (strlen(utf8tstr) + 1));
	memset(PassWord, 0, strlen(utf8tstr) + 1);
	memcpy(PassWord, utf8tstr, strlen(utf8tstr));
	free(utf8tstr);
	utf8tstr = NULL;*/
	MQTT_Variable_Header(CONNECT, 65535);
	//MQTT_Payload(3, ClientId, UserName, PassWord);
	MQTT_Payload(3, "35107354", "151131", "nggVk6KyAEBnYDybpGxW1jkpuCA=");
	MQTT_Fixed_Header(CONNECT);
	Cal_Remaining_Length(PVariable_Header->Cstr_Len + Payload->Cstr_Len);
	PMQTT_Packet = (PMQTT)malloc(sizeof(MQTT));
	PMQTT_Packet->Cstr_Len = PFixed_Header->Cstr_Len + PFixed_Header_Remaining->Cstr_Len + PVariable_Header->Cstr_Len + Payload->Cstr_Len;
	PMQTT_Packet->Cstr = (char *)malloc(sizeof(char) * (PMQTT_Packet->Cstr_Len + 1));
	memset(PMQTT_Packet->Cstr, 0, PMQTT_Packet->Cstr_Len + 1);
	memcpy(PMQTT_Packet->Cstr, PFixed_Header->Cstr, PFixed_Header->Cstr_Len);
	memcpy(PMQTT_Packet->Cstr + PFixed_Header->Cstr_Len, PFixed_Header_Remaining->Cstr, PFixed_Header_Remaining->Cstr_Len);
	memcpy(PMQTT_Packet->Cstr + PFixed_Header->Cstr_Len + PFixed_Header_Remaining->Cstr_Len, PVariable_Header->Cstr, PVariable_Header->Cstr_Len);
	memcpy(PMQTT_Packet->Cstr + PFixed_Header->Cstr_Len + PFixed_Header_Remaining->Cstr_Len + PVariable_Header->Cstr_Len, Payload->Cstr, Payload->Cstr_Len);
	printf("PMQTT_Packet:");
	for(i = 0; i <PMQTT_Packet->Cstr_Len; i++)
	{
		printf("%02x ", (unsigned char)PMQTT_Packet->Cstr[i]);
	}
	printf("\n");
	TCP_Socket = SocketConnect(IP, Port);
	byte = send(TCP_Socket, PMQTT_Packet->Cstr, PMQTT_Packet->Cstr_Len, 0);
	printf("Send %d Byte\n", byte);
	memset(Recv_Data, 0, sizeof(Recv_Data));
	byte = recv(TCP_Socket, Recv_Data, 1024, 0);
	printf("Recv %d Byte:", byte);
	for(i = 0; i < byte; i++)
	{
		printf("%02x ", (unsigned char)Recv_Data[i]);
	}
	printf("\n");
	system("pause");
	if(PFixed_Header_Remaining)
	{
		Free_Memory(PFixed_Header_Remaining);
	}
	if(PFixed_Header)
	{
		Free_Memory(PFixed_Header);
	}
	if(PVariable_Header)
	{
		Free_Memory(PVariable_Header);
	}
	if(Payload)
	{
		Free_Memory(Payload);
	}
	if(PMQTT_Packet)
	{
		Free_Memory(PMQTT_Packet);
	}
	shutdown(TCP_Socket, SD_BOTH);
	closesocket(TCP_Socket);
	WSACleanup();
	return(0);
}

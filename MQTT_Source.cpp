#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdarg.h>
#include "MQTT_Header.h"
#pragma comment(lib,"ws2_32.lib")

extern SOCKET TCP_Socket;
extern PMQTT PFixed_Header_Remaining, PFixed_Header, PVariable_Header, Payload, PMQTT_Packet;
//char *MQTT_Packet = NULL;
int Message_Id;
extern char *utf8tstr;
extern char *IP;
extern char* Port;

void GBK_To_UTF8(char *GBK_Str)
{
	int istrlen;
	wchar_t *unicodestr = NULL;
	if(GBK_Str)
	{
		istrlen = MultiByteToWideChar(CP_ACP, 0, GBK_Str, -1, NULL, 0);
		unicodestr = (wchar_t *)malloc(sizeof(wchar_t) * istrlen);
		memset(unicodestr, 0, sizeof(wchar_t) * istrlen);
		MultiByteToWideChar(CP_ACP, 0, GBK_Str, -1, unicodestr, sizeof(wchar_t) * istrlen);
		istrlen = WideCharToMultiByte(CP_UTF8, 0, unicodestr, -1, NULL, 0, NULL, NULL);
		utf8tstr = (char *)malloc(sizeof(char) * istrlen);
		memset(utf8tstr, 0, sizeof(char) * istrlen);
		WideCharToMultiByte(CP_UTF8, 0, unicodestr, -1, utf8tstr, istrlen, NULL, NULL);
		free(unicodestr);
		unicodestr = NULL;
	}
}

SOCKET SocketConnect(char *ip, char *port)
{
	SOCKET      TCP_Client;
	WSADATA        wsaData;
	INT                ret;
	unsigned short    Port;
	//int Recv_Timeout = 240000;
	if (ret = WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("Winsock DLL 加载失败,错误代码为：%d\n", ret);
		//system("pause");
		return(0);
	}
	TCP_Client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (TCP_Client == INVALID_SOCKET)
	{
		printf("创建套接字失败！");
		//system("pause");
		return(0);
	}
	//setsockopt(TCP_Client, SOL_SOCKET, SO_RCVTIMEO, (char *)&Recv_Timeout ,sizeof(int));
	SOCKADDR_IN Cli_Addr;
	Cli_Addr.sin_family = AF_INET;
	if(ip == NULL || port == NULL)
	{
		Cli_Addr.sin_port = htons(6002);
		Cli_Addr.sin_addr.S_un.S_addr = inet_addr("183.230.40.39");
		printf("IP地址为:183.230.40.39\n端口为:6002\n");
		/*if(fp)
		{
			fprintf(fp,"IP地址为:183.230.40.40\n端口为:1811\n");
		}*/
	}
	else
	{
		Port = StrTOInt(port, 1);
		Cli_Addr.sin_port = htons(Port);
		Cli_Addr.sin_addr.S_un.S_addr = inet_addr(ip);
		printf("IP为:%s\n端口为:%d\n",ip,(int)Port);
		/*if(fp)
		{
			fprintf(fp,"IP为:%s\n端口为:%d\n",ip,(int)Port);
		}*/
	}
	int TCP_Connect;
	TCP_Connect = connect(TCP_Client, (sockaddr *)&Cli_Addr, sizeof(Cli_Addr));
	if (TCP_Connect == SOCKET_ERROR)
	{
		printf("链接失败！");
		return(0);
	}
	return (TCP_Client);
}

int StrTOInt(char *str_port, int type_flag)
{
	int U_Short_Port;
	bool  flag = false;
	if(strlen(str_port) > 5)
	{
		printf("参数位数溢出！\n");
		return(PORT_TOO_LENGTH);
	}
	while(*str_port)
	{
		if(flag)
		{
			if(((int)(*str_port)) < 48 || ((int)(*str_port)) > 57)
			{
				printf("参数中存在非数字字符！\n");
				return(PORT_INLEGAL);
			}
			U_Short_Port *= 10;
			U_Short_Port += (int)(*str_port) - 48;
		}
		else
		{
			if(type_flag)
			{
				if(((int)(*str_port)) < 49 || ((int)(*str_port)) > 57)
				{
					printf("参数中存在非1-9的数字字符！\n");
					return(PORT_INLEGAL);
				}
			}
			else
			{
				if(((int)(*str_port)) < 48 || ((int)(*str_port)) > 57)
				{
					printf("参数中存在非的数字字符！\n");
					return(PORT_INLEGAL);
				}
			}
			U_Short_Port = (int)(*str_port) - 48;
			flag = true;
		}
		str_port++;
	}
	if(U_Short_Port > 65535)
	{
		printf("参数溢出！\n");
		return(PORT_OVERFLOW);
	}
	return(U_Short_Port);
}

CHECK_IP Cheek_IP(char *ip)
{
	char *cp_value = NULL;
	char *ip_data = NULL;
	char *ipv6_head = NULL;
	int  ipv4_count = 0, ipv6_count =0, n = 0;
	CHECK_IP ch_ip;
	ch_ip.ip_efec = false;
	ch_ip.ip_vers = 0;
	if(!ip)
	{
		printf("参数有误！\n");
		return(ch_ip);
	}
	ip_data = ip;
	ipv6_head = ip;
	while(*ip)
	{
		if(*ip == '.')
		{
			if(n < 1)
			{
				printf("非法IP地址！\n");
				return(ch_ip);
			}
			cp_value = (char *)malloc(sizeof(char) * (n + 1));
			if(cp_value)
			{
				memset(cp_value, 0, sizeof(char) * (n + 1));
				memcpy(cp_value, ip_data, n);
			}
			else
			{
				printf("验证IP时内存分配失败！\n");
				return(ch_ip);
			}
			n = StrTOInt(cp_value, 0);
			free(cp_value);
			cp_value = NULL;
			if(n < 0 || n > 255)
			{
				printf("非法IP地址！\n");
				return(ch_ip);
			}
			ip_data = ++ip;
			n = 0;
			ipv4_count++;
		}
		else if(*ip == ':')
		{
			if(n < 1)
			{
				if(ip == ipv6_head)
				{
					if(*(ip + 1) == ':')
					{
						if(*(ip + 2) == '\0')
						{
							printf("地址为IPv6中未指明地址！\n");
							return(ch_ip);
						}
						if((*(ip + 2) > 47) && (*(ip + 2) < 103))
						{
							if(*(ip + 2) == '1' && *(ip + 3) == '\0')
							{
								printf("地址为IPv6中环回地址！\n");
								return(ch_ip);
							}
							if((*(ip + 2) > 57) && (*(ip + 2) < 97))
							{
								if((*(ip + 2) < 65) || (*(ip + 2) > 70))
								{
									printf("非法IP地址！\n");
									return(ch_ip);
								}
							}
						}
						else
						{
							printf("非法IP地址！\n");
							return(ch_ip);
						}
					}
				}
				else
				{
					if((*(ip + 1) > 47) && (*(ip + 1) < 103))
					{
						if((*(ip + 1) > 57) && (*(ip + 1) < 97))
						{
							if((*(ip + 1) < 65) || (*(ip + 1) > 70))
							{
								printf("非法IP地址header_1！\n");
								return(ch_ip);
							}
						}
					}
					else
					{
						printf("非法IP地址header_else_1！\n");
						return(ch_ip);
					}
				}
			}
			else if(n < 5)
			{
				do
				{
					--n;
					if((*(ip_data + n) > 47) && (*(ip_data + n) < 103))
					{
						if((*(ip_data + n) > 57) && (*(ip_data + n) < 97))
						{
							if((*(ip_data + n) < 65) || (*(ip_data + n) > 70))
							{
								printf("非法IP地址！\n");
								return(ch_ip);
							}
						}
					}
					else
					{
						printf("非法IP地址！\n");
						return(ch_ip);
					}
				}while(n);
			}
			else
			{
				printf("非法IP地址！\n");
				return(ch_ip);
			}
			ip_data = ++ip;
			ipv6_count++;
		}
		else
		{
			++ip;
			++n;
		}
	}
	if(n > 0 && n < 5)
	{
		if(*(ip_data - 1) == '.')
		{
			cp_value = (char *)malloc(sizeof(char) * (n + 1));
			if(cp_value)
			{
				memset(cp_value, 0, sizeof(char) * (n + 1));
				memcpy(cp_value, ip_data, n);
			}
			else
			{
				printf("验证IP时内存分配失败！\n");
				return(ch_ip);
			}
			n = StrTOInt(cp_value, 0);
			free(cp_value);
			cp_value = NULL;
			if(n < 0 || n > 255)
			{
				printf("非法IP地址！\n");
				return(ch_ip);
			}
		}
		else
		{
			do
				{
					if((*(ip_data ++) > 47) && (*(ip_data ++) < 103))
					{
						if((*(ip_data ++) > 57) && (*(ip_data ++) < 97))
						{
							if((*(ip_data ++) < 65) || (*(ip_data ++) > 70))
							{
								printf("非法IP地址！\n");
								return(ch_ip);
							}
						}
					}
					else
					{
						printf("非法IP地址！\n");
						return(ch_ip);
					}
				}while(ip_data != ip);
		}
	}
	else
	{
		printf("非法IP地址！\n");
		return(ch_ip);
	}
	if(ipv4_count == 3)
	{
		if(ipv6_count == 3 || ipv6_count == 6)
		{
			ch_ip.ip_efec = true;
			ch_ip.ip_vers = 10;
		}
		else if(ipv6_count < 1)
		{
			ch_ip.ip_efec = true;
			ch_ip.ip_vers = 4;
		}
		else
		{
			printf("IPv4映射的IPv6地址不符合规范！\n");
			return(ch_ip);
		}
	}
	else if(ipv6_count > 0 && ipv6_count < 8)
	{
		if(ipv4_count < 1)
		{
			ch_ip.ip_efec = true;
			ch_ip.ip_vers = 6;
		}
		else
		{
			printf("IPv4映射的IPv6地址不符合规范！\n");
			return(ch_ip);
		}
	}
	else
	{
		printf("非法IP地址！\n");
		return(ch_ip);
	}
	return(ch_ip);
}

void Free_Memory(PMQTT P_Malloc_Struct)
{
	if(P_Malloc_Struct)
	{
		free(P_Malloc_Struct->Cstr);
		P_Malloc_Struct->Cstr = NULL;
		free(P_Malloc_Struct);
		P_Malloc_Struct = NULL;
	}
}

PMQTT Cal_Remaining_Length(int Remaining_Length)
{
	int Remaining_Number = 0;
	int Digit;
	PFixed_Header_Remaining = (PMQTT)malloc(sizeof(MQTT));
	PFixed_Header_Remaining->Cstr_Len = 0;
	PFixed_Header_Remaining->Cstr = (char *)malloc(sizeof(char));
	memset(PFixed_Header_Remaining->Cstr, 0, Remaining_Number + 1);
	do
	{
		PFixed_Header_Remaining->Cstr = (char *)realloc(PFixed_Header_Remaining->Cstr, sizeof(char)*(Remaining_Number + 2));
		memset(PFixed_Header_Remaining->Cstr + Remaining_Number, 0, Remaining_Number + 2);
		Digit = Remaining_Length % 128;
		Remaining_Length /= 128;
		if(Remaining_Length > 0)
		{
			Digit |= 0x80;
		}
		PFixed_Header_Remaining->Cstr[Remaining_Number] = Digit;
		Remaining_Number++;
	}while(Remaining_Length > 0);
	PFixed_Header_Remaining->Cstr_Len = Remaining_Number;
	return(PFixed_Header_Remaining);
}

PMQTT MQTT_Fixed_Header(int Message_Type/*,int Remaining_Length*/)
{
	int MQTT_Message_Type;
	int i=1;
	int Retain = 1;
	int QoS = 1;
	int Dup = 1;
	//Cal_Remaining_Length(Remaining_Length);
	PFixed_Header = (PMQTT)malloc(sizeof(MQTT));
	PFixed_Header->Cstr = (char *)malloc(sizeof(char) * 2);
	PFixed_Header->Cstr_Len = 1;
	memset(PFixed_Header->Cstr, 0, 2);
	if(Message_Type == PUBLISH)
	{
		PFixed_Header->Cstr[0] = (char)Retain;
		PFixed_Header->Cstr[0] = (char)((int)PFixed_Header->Cstr[0]|(QoS<<1));
		PFixed_Header->Cstr[0] = (char)((int)PFixed_Header->Cstr[0]|(Dup<<3));
		PFixed_Header->Cstr[0] = (char)((int)PFixed_Header->Cstr[0]|(Message_Type<<4));
	}
	else
	{
		MQTT_Message_Type = Message_Type<<4;
		PFixed_Header->Cstr[0] = (char)MQTT_Message_Type;
	}
	/*do
	{
		MQTT_Packet[i] = PFixed_Header_Remaining->Remaining_Value[i-1];
		//memcpy(&MQTT_Packet[i],&PFixed_Header_Remaining->Remaining_Value[i-1],1); 
		i++;
		PFixed_Header_Remaining->Rem_Byte_Number--;
	}while(PFixed_Header_Remaining->Rem_Byte_Number);*/
}

PMQTT MQTT_Variable_Header(int Packet_Type,int Keep_Alive, ...)
{
	//char *Variable_Header;
	char *Topic_Name = NULL;
	int Topic_Name_Len;
	va_list args;
	PVariable_Header = (PMQTT)malloc(sizeof(MQTT));
	/*switch(Packet_Type)
	{
		case CONNECT:
		{
			Variable_Header = (char *)malloc(sizeof(char)*11);
			memset(Variable_Header,0,11);
			Variable_Header[1] = (char)4;
			Variable_Header[2] = 'M';
			Variable_Header[3] = 'Q';
			Variable_Header[4] = 'T';
			Variable_Header[5] = 'T';
			Variable_Header[6] = (char)1;
			Variable_Header[7] = 0xec;
			Variable_Header[8] = (char)(Keep_Alive>>8);
			Variable_Header[9] = (char)Keep_Alive;
			break;
		}
		case CONNACK:
		{
			Variable_Header = (char *)malloc(sizeof(char)*3);
			memset(Variable_Header,0,3);
			Variable_Header[0] = 1;
			Variable_Header[1] = 0;
			break;
		}
		case PUBLISH:
		{
			Topic_Name_Len = strlen();
			Variable_Header = (char *)malloc(sizeof(char)*(Topic_Name_Len+5));
			memset(Variable_Header,0,Topic_Name_Len+5);
			Variable_Header[0] = (char)(Topic_Name_Len>>8);
			Variable_Header[1] = (char)(Topic_Name_Len>>8);
			memcpy(Variable_Header+2,,Topic_Name_Len); 
			Variable_Header[Topic_Name_Len+2] = (char)(Message_Id>>8);
			Variable_Header[Topic_Name_Len+3] = (char)Message_Id;
			break;
		}
		case PUBACK:
		{
			Variable_Header = (char *)malloc(sizeof(char)*3);
			memset(Variable_Header,0,3);
			Variable_Header[0] = (char)(Message_Id>>8);
			Variable_Header[1] = (char)Message_Id;
			break;
		}
		case PUBREC:
		{
			
		}
		default
	}*/
	if(Packet_Type == CONNECT)
	{
		PVariable_Header->Cstr = (char *)malloc(sizeof(char) * 11);
		PVariable_Header->Cstr_Len = 10;
		memset(PVariable_Header->Cstr, 0, 11);
		PVariable_Header->Cstr[1] = 4;
		PVariable_Header->Cstr[2] = 'M';
		PVariable_Header->Cstr[3] = 'Q';
		PVariable_Header->Cstr[4] = 'T';
		PVariable_Header->Cstr[5] = 'T';
		PVariable_Header->Cstr[6] = 4;
		PVariable_Header->Cstr[7] = 0xc0;
		PVariable_Header->Cstr[8] = Keep_Alive>>8;
		PVariable_Header->Cstr[9] = Keep_Alive;
	}
	else if(Packet_Type == CONNACK)
	{
		PVariable_Header->Cstr = (char *)malloc(sizeof(char) * 3);
		PVariable_Header->Cstr_Len = 2;
		memset(PVariable_Header->Cstr, 0, 3);
		PVariable_Header->Cstr[0] = 1;
		PVariable_Header->Cstr[1] = 0;
	}
	else if(Packet_Type == PUBLISH)
	{
		va_start(args, Keep_Alive);
		Topic_Name = va_arg(args, char*);
		Topic_Name_Len = strlen(Topic_Name);
		PVariable_Header->Cstr = (char *)malloc(sizeof(char) * (Topic_Name_Len + 5));
		memset(PVariable_Header->Cstr, 0, Topic_Name_Len + 5);
		PVariable_Header->Cstr[0] = Topic_Name_Len>>8;
		PVariable_Header->Cstr[1] = Topic_Name_Len;
		memcpy(PVariable_Header->Cstr + 2, Topic_Name, Topic_Name_Len); 
		PVariable_Header->Cstr[Topic_Name_Len + 2] = Message_Id>>8;
		PVariable_Header->Cstr[Topic_Name_Len + 3] = Message_Id;
		PVariable_Header->Cstr_Len = Topic_Name_Len + 4;
	}
	else if((Packet_Type > PUBLISH)&&(Packet_Type < PINGREQ))
	{
		PVariable_Header->Cstr = (char *)malloc(sizeof(char) * 3);
		PVariable_Header->Cstr_Len = 2;
		memset(PVariable_Header->Cstr, 0, 3);
		PVariable_Header->Cstr[0] = (char)(Message_Id>>8);
		PVariable_Header->Cstr[1] = (char)Message_Id;
	}
	else
	{
		free(PVariable_Header);
		PVariable_Header = NULL;
	}
	return(PVariable_Header);
}

PMQTT MQTT_Payload(int count, ...)
{
	char *cp = NULL;
	//char *Payload = NULL;
	int Arg_Num/*, Payload_Len = 0*/;
	va_list args;
	va_start(args, count);
	Arg_Num = count;
	Payload = (PMQTT)malloc(sizeof(MQTT));
	Payload->Cstr_Len = 0;
	while(Arg_Num)
	{
		cp = va_arg(args, char*);
		if(cp == NULL)
		{
			if(Payload)
			{
				Free_Memory(Payload);
			}
			return(Payload);
		}
		if(Arg_Num == count)
		{
			Payload->Cstr = (char *)malloc(sizeof(char) * (strlen(cp) + 3));
		}
		else
		{
			Payload->Cstr = (char *)realloc((void *)Payload->Cstr, sizeof(char) * (Payload->Cstr_Len + strlen(cp) + 3));
		}
		if(Payload)
		{
			memset(Payload->Cstr + Payload->Cstr_Len, 0, sizeof(char) * (strlen(cp) + 3));
			Payload->Cstr[Payload->Cstr_Len] = strlen(cp)>>8;
			Payload->Cstr[Payload->Cstr_Len + 1] = strlen(cp);
			memcpy(Payload->Cstr + Payload->Cstr_Len + 2, cp, strlen(cp));
			Payload->Cstr_Len += strlen(cp) + 2;
		}
		Arg_Num--;
	}
	va_end(args);
	return(Payload);
}

void Print_Log_Time(void)
{
	time_t Timestamp;
	struct  tm *Time;
	char *Week_Day[] = { "星期日", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六" };
	time(&Timestamp);
	Time = localtime(&Timestamp);
	printf("%d-%02d-%02d", Time->tm_year + 1900, Time->tm_mon + 1, Time->tm_mday);
	printf(" %s %02d:%02d:%02d:", Week_Day[Time->tm_wday], Time->tm_hour, Time->tm_min, Time->tm_sec);
	if(fp)
	{
		fprintf(fp,"%d-%02d-%02d", Time->tm_year + 1900, Time->tm_mon + 1, Time->tm_mday);
		fprintf(fp," %s %02d:%02d:%02d:", Week_Day[Time->tm_wday], Time->tm_hour, Time->tm_min, Time->tm_sec);
	}
}

void Print_Log(void)
{
	char         ch;
	if(strlen(cpath) == 0)
	{
		time_t Timestamp;
		struct  tm *Time;
		time(&Timestamp);
		int  i, j, ivalue;
		int times = 1000;
		Time = localtime(&Timestamp);
		int itime[] = {Time->tm_mon + 1, Time->tm_mday, Time->tm_hour, Time->tm_min, Time->tm_sec};
		memset(cpath, 0, sizeof(cpath));
		cpath[0] = 'L';
		cpath[1] = 'o';
		cpath[2] = 'g';
		cpath[strlen(cpath)] = '-';
		ivalue = Time->tm_year + 1900;
		while(times != 0)
		{
			cpath[strlen(cpath)] = ivalue/times + 48;
			ivalue %= times;
			times /= 10;
		}
		for(i = 0; i < 5; i++)
		{
			cpath[strlen(cpath)] = '-';
			ivalue = itime[i];
			times = 10;
			while(times != 0)
			{
				cpath[strlen(cpath)] = ivalue/times + 48;
				ivalue %= times;
				times /= 10;
			}
		}
		cpath[strlen(cpath)] = '.';
		cpath[strlen(cpath)] = 'l';
		cpath[strlen(cpath)] = 'o';
		cpath[strlen(cpath)] = 'g';
	}
	fp = fopen(cpath, "a+");
	if(fp == NULL)
	{
		perror("文件打开失败！");
	}
}

bool String_match(char *str_src,char *str_dst)
{
	if((!str_src)||(!str_dst))
	{
		return (false);
	}
	while(*str_src)
	{
		if((*str_dst)!=(*str_src))
		{
			return (false);
		}
		str_src++;
		str_dst++;
	}
	return (true);
}

bool Read_Configure_Info(void)
{
	FILE         *fp = NULL;
	char    Str[30] = {'0'};
	char *s = "Effective=1";
	int               i = 0;
	int                iLen;
	bool      bflag = false;
	fp = fopen("Initialization.ini","r+");
	if(!fp)
	{
		return (false);
	}
	fgets(Str, 30, fp);
	if(String_match(s,Str))
	{
		if(!feof(fp))
		{
			memset(Str, 0, sizeof(Str));
			fgets(Str, 30, fp);
			while(Str[i] != '=')
			{
				i++;
			}
			IP = (char *)malloc(sizeof(char) * strlen(Str + i + 1));
			if(IP)
			{
				memset(IP, 0, strlen(Str + i + 1));
				memcpy(IP, Str + i + 1, strlen(Str + i + 1) - 1);
			}
			else
			{
				printf("内存分配失败！\n");
				fclose(fp);
				return (false);
			}
			if(!feof(fp))
			{
				memset(Str,0,sizeof(Str));
				i = 0;
				fgets(Str, 30, fp);
				while(Str[i] != '=')
				{
					i++;
				}
				Port = (char *)malloc(sizeof(char) * strlen(Str + i + 1));
				if(Port)
				{
					memset(Port , 0, strlen(Str + i + 1));
					memcpy(Port, Str + i + 1, strlen(Str + i + 1) - 1);
				}
				else
				{
					printf("内存分配失败！\n");
					fclose(fp);
					return (false);
				}
				if(!feof(fp))
				{
					do
					{
						memset(Str,0,sizeof(Str));
						i = 0;
						fgets(Str, 5, fp);
						while(Str[i] != '=' && Str[i] != '\0')
						{
							i++;
						}
						if(i < strlen(Str) - 1)
						{
							if (Str[strlen(Str) - 1] == '\n')
							{
								Auth_Info = (char *)malloc(sizeof(char) * strlen(Str + i) + 1);
								if(Auth_Info)
								{
									memset(Auth_Info, 0, strlen(Str + i) + 1);
									Auth_Info[0] = '*';
									memcpy(Auth_Info + 1, Str + i + 1, strlen(Str + i + 1) - 1);
								}
							}
							else
							{
								Auth_Info = (char *)malloc(sizeof(char) * strlen(Str + i) + 2);
								if(Auth_Info)
								{
									memset(Auth_Info, 0, strlen(Str + i) + 2);
									Auth_Info[0] = '*';
									memcpy(Auth_Info + 1, Str + i + 1, strlen(Str + i + 1));
								}
							}
							if(!Auth_Info)
							{
								printf("内存分配失败！\n");
								fclose(fp);
								return (false);
							}
						}
						else if(i == strlen(Str) - 1)
						{
							Auth_Info = (char *)malloc(sizeof(char) * 2);
							if(Auth_Info)
							{
								memset(Auth_Info, 0, 2);
								Auth_Info[0] = '*';
							}
							else
							{
								printf("内存分配失败！\n");
								fclose(fp);
								return (false);
							}
						}
						else
						{
							if(Auth_Info)
							{
								iLen = strlen(Auth_Info);
								if(Str[strlen(Str) - 1] == '\n')
								{
									Auth_Info = (char *)realloc((void *)Auth_Info, sizeof(char) * (iLen + strlen(Str) + 1));
									if(Auth_Info)
									{
										memset(Auth_Info + iLen, 0, strlen(Str) + 1);
										memcpy(Auth_Info + iLen, Str, strlen(Str) - 1);
									}
								}
								else
								{
									Auth_Info = (char *)realloc((void *)Auth_Info, sizeof(char) * (iLen + strlen(Str) + 2));
									if(Auth_Info)
									{
										memset(Auth_Info + iLen, 0, strlen(Str) + 2);
										memcpy(Auth_Info + iLen, Str, strlen(Str));
									}
								}
								if(!Auth_Info)
								{
									printf("内存分配失败！\n");
									fclose(fp);
									return (false);
								}
							}
						}
					}while((Str[strlen(Str) - 1] != '\n') && (!feof(fp)));	
					Auth_Info[strlen(Auth_Info)] = '#';
					if(!feof(fp))
					{
						do
						{
							memset(Str, 0, sizeof(Str));
							i = 0;
							fgets(Str, 5, fp);
							while(Str[i] != '=' && Str[i] != '\0')
							{
								i++;
							}
							iLen = strlen(Auth_Info);
							if(i < strlen(Str) - 1)
							{
								if (Str[strlen(Str) - 1] == '\n')
								{
									Auth_Info = (char *)realloc((void *)Auth_Info, sizeof(char) * (iLen + strlen(Str + i)));
									if(Auth_Info)
									{
										memset(Auth_Info + iLen, 0, strlen(Str + i));
										memcpy(Auth_Info + iLen, Str + i + 1, strlen(Str + i + 1) - 1);
									}
								}
								else
								{
									Auth_Info = (char *)realloc((void *)Auth_Info, sizeof(char) * (iLen + strlen(Str + i) + 1));
									if(Auth_Info)
									{
										memset(Auth_Info + iLen, 0, strlen(Str + i) + 1);
										memcpy(Auth_Info + iLen, Str + i + 1, strlen(Str + i + 1));
									}
								}
								if(!Auth_Info)
								{
									printf("内存分配失败！\n");
									fclose(fp);
									return (false);
								}
								bflag = true;
							}
							else if(i == strlen(Str) - 1)
							{
								bflag = true;
							}
							else
							{
								if(bflag)
								{
									if(Str[strlen(Str)-1] == '\n')
									{
										Auth_Info = (char *)realloc((void *)Auth_Info, sizeof(char) * (iLen + strlen(Str) + 1));
										if(Auth_Info)
										{
											memset(Auth_Info + iLen, 0, strlen(Str) + 1);
											memcpy(Auth_Info + iLen, Str, strlen(Str) - 1);
										}
									}
									else
									{
										Auth_Info = (char *)realloc((void *)Auth_Info, sizeof(char) * (iLen + strlen(Str) + 2));
										if(Auth_Info)
										{
											memset(Auth_Info + iLen, 0, strlen(Str) + 2);
											memcpy(Auth_Info + iLen, Str, strlen(Str));
										}
									}
									if(!Auth_Info)
									{
										printf("内存分配失败！\n");
										fclose(fp);
										return (false);
									}
								}
							}
						}while((Str[strlen(Str) - 1] != '\n') && (!feof(fp)));
						bflag = false;
						Auth_Info[strlen(Auth_Info)] = '#';
						if(!feof(fp))
						{
							do
							{
								memset(Str, 0, sizeof(Str));
								i = 0;
								fgets(Str, 5, fp);
								while(Str[i] != '=' && Str[i] != '\0')
								{
									i++;
								}
								iLen = strlen(Auth_Info);
								if(i < strlen(Str) - 1)
								{
									if(Str[strlen(Str) - 1] == '\n')
									{
										Auth_Info = (char *)realloc((void *)Auth_Info, sizeof(char) * (iLen + strlen(Str + i)));
										if(Auth_Info)
										{
											memset(Auth_Info + iLen, 0, strlen(Str + i));
											memcpy(Auth_Info + iLen, Str + i + 1, strlen(Str + i + 1) - 1);
										}
									}
									else
									{
										Auth_Info = (char *)realloc((void *)Auth_Info, sizeof(char) * (iLen + strlen(Str + i) + 1));
										if(Auth_Info)
										{
											memset(Auth_Info + iLen, 0, strlen(Str + i) + 1);
											memcpy(Auth_Info + iLen, Str + i + 1, strlen(Str + i + 1));
										}
									}
									if(!Auth_Info)
									{
										printf("内存分配失败！\n");
										fclose(fp);
										return (false);
									}
									bflag = true;
								}
								else if(i == strlen(Str) - 1)
								{
									bflag = true;
								}
								else
								{
									if(bflag)
									{
										if(Str[strlen(Str) - 1] == '\n')
										{
											Auth_Info = (char *)realloc((void *)Auth_Info,  sizeof(char) * (iLen + strlen(Str) + 1));
											if(Auth_Info)
											{
												memset(Auth_Info + iLen, 0, strlen(Str) + 1);
												memcpy(Auth_Info + iLen, Str, strlen(Str) - 1);
											}
										}
										else
										{
											Auth_Info = (char *)realloc((void *)Auth_Info,  sizeof(char) * (iLen + strlen(Str) + 2));
											if(Auth_Info)
											{
												memset(Auth_Info + iLen, 0, strlen(Str) + 2);
												memcpy(Auth_Info + iLen, Str, strlen(Str));
											}
										}
										if(!Auth_Info)
										{
											printf("内存分配失败！\n");
											fclose(fp);
											return (false);
										}
									}
								}
							}while((Str[strlen(Str) - 1] != '\n') && (!feof(fp)));
							Auth_Info[strlen(Auth_Info)] = '*';
							bflag = false;
						}
					}
				}
			}
		}
		fclose(fp);
		return (true);
	}
	else
	{
		fclose(fp);
		return (false);
	}
}

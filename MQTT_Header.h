/*
 *Header.h
 *本项目头文件
 *
 *本头文件包括本项目中需要使用的所有结构体
 *函数，宏定义
 *
 */
 
#ifndef __MQTT_HEADER_H__
#define __MQTT_HEADER_H__

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#define PORT_TOO_LENGTH -1
#define PORT_INLEGAL    -2
#define PORT_OVERFLOW   -3
/*
 *客户端请求连接服务器
 */
#define CONNECT          1
/*
 *连接确认
 */
#define CONNACK          2
/*
 *发布消息
 */
#define PUBLISH          3
/*
 *发布确认
 */
#define PUBACK           4
/*
 *发布接收（有保证的交付第1部分）
 */
#define PUBREC           5
/*
 *发布释放（有保证的交付第2部分）
 */
#define PUBREL           6
/*
 *发布完成（有保证的交付第3部分）
 */
#define PUBCOMP          7
/*
 *客户端订阅请求
 */
#define SUBSCRIBE        8
/*
 *订阅确认
 */
#define SUBACK           9
/*
 *客户端取消订阅请求
 */
#define UNSUBSCRIBE      10
/*
 *取消订阅确认
 */
#define UNSUBACK         11
/*
 *PING请求
 */
#define PINGREQ          12
/*
 *PING响应
 */
#define PINGRESP         13
/*
 *客户端断开连接
 */
#define DISCONNECT       14

typedef struct MQTT_STRUCT
{
	char *Cstr;
	int Cstr_Len;
}MQTT, *PMQTT;

typedef struct CHECK_IP_STRUCT
{
	bool ip_efec;
	int ip_vers;
}CHECK_IP;

/*
 *将GBK字符串转换为UTF-8字符串函数
 */
void GBK_To_UTF8(char *GBK_Str);
 
/*
 *SOCKET链接函数
 */
 
SOCKET TCP_Connect(char *ip, char *port);
SOCKET SocketConnect(char *ip, char *port);
 
/*
 *将字符串转换为整数函数
 */
int StrTOInt(char *str_port, int type_flag);
 
/*
 * 检测IP地址确函数
 */
CHECK_IP Cheek_IP(char *ip);

/*
 *释放内存函数
 */
void Free_Memory(PMQTT P_Malloc_Struct);

/*
 *固定头剩余长度生成函数
 */
PMQTT Cal_Remaining_Length(int Remaining_Length);
 
/*
 *固定头首字节填充函数
 */
PMQTT MQTT_Fixed_Header(int Message_Type/*, int Remaining_Length*/);

/*
 *虚拟头部生成函数
 */
PMQTT MQTT_Variable_Header(int Packet_Type, int Keep_Alive, ...);

/*
 *有效载荷生成函数
 */
PMQTT MQTT_Payload(int count, ...); 

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __MQTT_HEADER_H__ */

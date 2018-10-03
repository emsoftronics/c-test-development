
#ifndef __NETCLIENT_H__
#define __NETCLIENT_H__

#ifndef NULL
#define NULL 	(void *)0
#endif
#include "llist.h"
#include <stdint.h>	
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#define TCP_BUFFER_SIZE		4096		/* 4 KB */
#define UDP_BUFFER_SIZE		(1048576L << 2)	/* 4 MB */
#define CLIENT_NAME_MAX_LEN	16	
#define CLIENT_DOMAIN_MAX_LEN	32
#define IP_SIZE			4
#define TYPE_TCP		3333	
#define TYPE_UDP		7777

#define SERVER_ID       0x0

enum infotype {
        IT_WHO = 1,
        IT_ME,
        IT_GETIDTABLE,
        IT_IDTABLESTART,
        IT_IDTABLECONTINUE,
        IT_IDTABLEFINISHED,
        IT_CREATEDID,
        IT_DELETEDID,
        IT_LOGIN,
        IT_AUTH,
        IT_PASS,
        IT_CMD,
        IT_CMDRSP,
        IT_DENIED,
        IT_MSG,
        IT_OK,
        IT_FAIL,
        IT_ITEMINFO,
        IT_ITEMSTART,
        IT_ITEMCONTINUE,
        IT_ITEMFINISHED,
        IT_VIDEOSTREAM
};


/*--------------------------
+----------+--------------+-----+-----------
|id(f/t)   |itype/page    |  len     | data
+----------+--------------+-----+-----------
----------------------------*/
typedef struct {
        int64_t id;
        union {
                struct {
                        uint64_t infotype:8;
                        uint64_t page:40;
                        uint64_t datalen:16;
                } tcp;

                struct {
                        uint64_t infotype:8;
                        uint64_t page:24;
                        uint64_t datalen:32;
                } udp;

        }  datainfo;

} header_t;


typedef struct {
		char name[CLIENT_NAME_MAX_LEN];		/*client $USER*/
		char domain[CLIENT_DOMAIN_MAX_LEN];	/*client $HOSTNAME*/
		char ip[IP_SIZE];			/*client ip*/
		uint32_t port;				/*client port*/
} clientinfo_t;

class netclient : public llist
{
	private:
		static pthread_mutex_t mutex;
		pthread_t thread;
	public:
		int fd;
		clientinfo_t info;
		unsigned char *buffer;

		netclient(int fd, clientinfo_t *info, unsigned int type);
		~netclient(void);
		static void *netClientThread(void *arg);
		static netclient* createTcpClient(int fd);
		static netclient* createUdpClient(int fd);
};

#endif


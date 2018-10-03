#ifndef __SCPROTOCOL_H__
#define __SCPROTOCOL_H__

#define SERVER_ID 	0x0

typedef enum {
	WHO = 1,
	ME,
	GETIDTABLE,
	IDTABLESTART,
	IDTABLECONTINUE,
	IDTABLEFINISHED,
	CREATEDID,
	DELETEDID,
	LOGIN,
	AUTH,
	PASS,	
	CMD,
	CMDRSP,
	DENIED,
	MSG,
	OK,
	FAIL,
	ITEMINFO,
	ITEMSTART,
	ITEMCONTINUE,
	ITEMFINISHED,
	VIDEOSTREAM
} infotype;

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


class scprotocol 
{
	private:
		int fd;
		unsigned char *buf;
		bool useudp;	
	public:
		scprotocol( int fd, char *buf, bool useudp);
		bool identify();
	 	bool sendidtable();

}

#endif

#include "netclient.h"

pthread_mutex_t netclient::mutex = PTHREAD_MUTEX_INITIALIZER;

void *netclient::netClientThread(void *arg)
{
	netclient *nc = (netclient *) arg;
	int len;
	while(1) {
		len = read(nc->fd,nc->buffer,(nc->type&0xffff == TYPE_TCP)?TCP_BUFFER_SIZE:UDP_BUFFER_SIZE);
		//idenfy data rotine

		//pthread_mutex_lock(&mutex);
		//write data
		//pthread_mutex_unlock(&mutex);
	}
}

netclient::netclient(int fd, clientinfo_t *info, unsigned int type):llist()
{
	this->info = *info;
	this->type = this->type | type;
	this->fd =fd;
	buffer = NULL;
	if (type == TYPE_TCP) {
		buffer = new unsigned char [TCP_BUFFER_SIZE];
	} else if (type == TYPE_UDP) {
		buffer = new unsigned char [UDP_BUFFER_SIZE];
	} else {
		delete this;
	}
	
	if (!buffer){
		int rc;
		rc = pthread_create(&thread, NULL, netClientThread, this);
		if (rc < 0) {
			delete this;
			std::cout<<this->info.name<<"@"
			<<this->info.domain
			<<": Client thread excution failed!!"<<std::endl; 	
		} else {
			rc = pthread_detach(thread);
			if (rc < 0)
				std::cout<<this->info.name<<"@"
				<<this->info.domain
				<<": Client thread is undetachable!!"<<std::endl; 	
		}		
	}
}

netclient::~netclient(void)
{
	delete[] buffer;
}


netclient* netclient::createTcpClient(int fd)
{
	unsigned char *tbuf = new unsigned char[sizeof(clientinfo_t) + sizeof(header_t)+4];
	netclient *nc;
	header_t *hdr = (header_t *)tbuf;
	clientinfo_t *ci = (clientinfo_t *)(tbuf+sizeof(header_t));
	int ret;
	hdr->id = SERVER_ID;
	hdr->datainfo.tcp.infotype = IT_WHO;
	hdr->datainfo.tcp.page = 0;
	hdr->datainfo.tcp.datalen = 0;
	ret = write(fd,tbuf,sizeof(header_t));	
	if ( ret <  0){
		std::cout<<"netclient: write failed!!\n";
		delete[] tbuf;
		close(fd);
		return NULL;
	}
	
	ret = read(fd, tbuf, sizeof(clientinfo_t) + sizeof(header_t));
	if ( ret < 0){	
		std::cout<<"netclient: read failed!!\n";
		delete[] tbuf;
		close(fd);
		return NULL;
	}
	
	if ( (hdr->id == SERVER_ID) && (hdr->datainfo.tcp.infotype == IT_ME) 
		&& (hdr->datainfo.tcp.datalen > 0)) {
		nc = new netclient(fd, ci, TYPE_TCP);
		delete[] tbuf;
	}
	return nc;
} 

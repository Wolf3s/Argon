#include "ps2ip.h"
#include "ioman.h"
#include "stdio.h"

stdio_IMPORTS_start
 I_printf
stdio_IMPORTS_end

ps2ip_IMPORTS_start
 I_inet_addr
 I_lwip_socket
 I_lwip_connect
 I_lwip_recv
 I_lwip_send
 I_lwip_close
ps2ip_IMPORTS_end

ioman_IMPORTS_start
 I_open
 I_write
 I_close
ioman_IMPORTS_end

static char buffer[1024];

int _start(void)
{
	int lSD = lwip_socket ( PF_INET, SOCK_STREAM, IPPROTO_TCP );
	int lFD = open ( "host:test.xml", O_CREAT | O_TRUNC | O_WRONLY );
	int temp;

	struct sockaddr_in lAddr;

	lAddr.sin_family      = AF_INET;
	lAddr.sin_addr.s_addr = inet_addr("65.207.183.49");
	lAddr.sin_port        = htons(80);

	//207.200.98.25/sbin/xmllister.phtml?limit=30&service=aol&no_compress=1
	//xoap.weather.com/65.207.183.49/weather/local/?RSXX0063&cc=*&dayf=7&unit=m

	lSD = lwip_socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	printf("lSD = %i\n", lSD);

	temp = lwip_connect(lSD, (struct sockaddr*) &lAddr, sizeof(struct sockaddr_in));
	printf("lwip_connect = %i\n", temp);

	//char* szGet = "GET /sbin/xmllister.phtml?limit=50&service=aol&no_compress=1 HTTP/1.0\r\nUser-Agent: Mozilla/2.0\r\nAccept: */*\r\n\r\n";
	char* szGet = "GET /weather/local/RSXX0063?&cc=*&dayf=7&unit=m HTTP/1.0\r\nHost: xoap.weather.com\r\nUser-Agent: Mozilla/2.0\r\nAccept: */*\r\n\r\n";
	temp = lwip_send(lSD, (void*) szGet, strlen(szGet), 0);
	printf("lwip_send = %i\n", temp);

	int pktlength = 0;
	int total = 0;
	while(1)
	{
		pktlength = lwip_recv(lSD, buffer, 1024, 0);
		printf("pktlength = %i\n", pktlength);

		write(lFD, buffer, pktlength);
		//printf("buffer=%s\n", buffer);
		total += pktlength;

		if(!pktlength)
			break;

	}

	printf("total = %i\n", total);

	close ( lFD );
	lwip_close ( lSD );

	return 1;
}





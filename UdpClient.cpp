#include "UdpClient.h"


UdpClient::UdpClient(void)
{
	hIOMutex = CreateMutex(NULL, FALSE, NULL);
	kill = 0;
    bMessageSet = false;

    memset(message, '\0', BUFLEN);

	_beginthread(UdpClient::staticUdpClientCallback, 0, this);
}

UdpClient::~UdpClient(void)
{
    bMessageSet = false;
    kill = 1;
    closesocket(s);
    WSACleanup();
}

void UdpClient::udpClientCallback(void)
{
    //Initialization
    slen =sizeof(si_other);
    //Initialise winsock
    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Failed. Error Code : %d",WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("Initialised.\n");
     
    //create socket
    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
    {
        printf("socket() failed with error code : %d" , WSAGetLastError());
        exit(EXIT_FAILURE);
    }
     
    //setup address structure
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
    si_other.sin_addr.S_un.S_addr = inet_addr(SERVER);

    while(1)
    {
        if (bMessageSet) {
            if (sendto(s, message, strlen(message) , 0 , (struct sockaddr *) &si_other, slen) == SOCKET_ERROR)
            {
                printf("sendto() failed with error code : %d" , WSAGetLastError());
                exit(EXIT_FAILURE);
            }
            memset(buf,'\0', BUFLEN);
            //try to receive some data, this is a blocking call
            if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == SOCKET_ERROR)
            {
                printf("recvfrom() failed with error code : %d" , WSAGetLastError());
                exit(EXIT_FAILURE);
            }
            bMessageSet = false;
        }
    }
}

void UdpClient::staticUdpClientCallback(void *a) 
{
    ((UdpClient *)a)->udpClientCallback();
}

int UdpClient::sendMessageToServer(char *msg)
{
    memset(message, '\0', BUFLEN);
    strcpy(message, msg);
    bMessageSet = true;
    return 0;
}
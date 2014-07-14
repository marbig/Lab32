#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

int cs;

void wyslij_znak(char c)
{
    send(cs, &c, 1, 0);
}

void wyslij_napis(const char *str)
{
    while(*str)wyslij_znak(*str++);
}

enum State { EnteringUsername, EnteringPassword, EnteringCommand };
State state = EnteringUsername;
string command("");
string password("");
string username("");
void odebrano_znak(char c)
{
    if(c == 13)
    {
        switch(state)
        {
            case EnteringUsername: state = EnteringPassword; break;
            case EnteringPassword: state = EnteringCommand; break;
            case EnteringCommand:
                command = "";
            break;
        }
    }
    else
    {
        if(isalpha(c))
        {
            switch(state)
            {
                case EnteringUsername: username+=c; wyslij_znak(c); break;
                case EnteringPassword: password+=c; wyslij_znak('*'); break;
                case EnteringCommand: command+=c; wyslij_znak(c); break;
            }
        }
    }
}

void putline(int cs, string str)
{
    send(cs, str.c_str(), str.length(), 0);
}
string getline(int cs, bool costam = false)
{
    string line("");
    char c;
    ssize_t rxbytes;
    do {
        rxbytes = recv(cs, &c, 1, 0);
    if(rxbytes == 1 && c>=32)
    {
        line +=c;
        if(costam) c = '*';
        send(cs, &c, 1, 0);
    }
        }while (rxbytes==1 && c!='\r');
    return line;
}

//try{
//    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
//    local.sin_addr.s_addr = htonl(INADDR_ANY);
//    local.sin_port = htons(1333);
//    bind(s, (sockaddr*)&local, sizeof(local));
//    listen(s, 10);
//    int cs = accept(s, (sockaddr*)&remote, &sockaddrlen);
//    send(cs, "test", 4, 0);
//    shutdown(CS, SHUT_RDWR);
//} catch (SocketException e)
//{
//    //...........
//
//}

int main() {
    const int BUFLEN = 512;
    sockaddr_in local, remote;
    socklen_t sockaddrlen = sizeof(remote);
    int s;
    char buf[BUFLEN];

    if( (s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        cerr << "problem: socket" << endl;
        return -1;
    }
    memset((char*)&local, 0, sizeof(local));
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_port = htons(1333);
    if(bind(s, (sockaddr*)&local, sizeof(local)) == -1)
    {
        cerr << "problem: bind" << endl;
        return -1;
    }

    if(0 > listen(s, 10)) {
        cerr << "problem: listen" << endl;
        return -1;
    }
    do {
        string a, b, c;
        cs = accept(s, (sockaddr*)&remote, &sockaddrlen);
        putline(cs, "User: ");
        a = getline(cs);
        cout << a << endl;
        putline (cs, "\r\nPass: ");
        b = getline(cs, true);
        cout << b << endl;
        do{
            putline(cs, "\r\nCmd; ");
            c = getline(cs);
            cout << c << endl;
        }while(c != "exit");
            shutdown(cs, SHUT_RDWR);
    }while (true);

    putline(cs, "czesc, dziubacku 1\r\n");
    cout << getline(cs) << endl;
    putline(cs, "czesc, dziubacku 2\r\n");
    cout << getline(cs) << endl;
    putline(cs, "czesc, dziubacku 3\r\n");
    cout << getline(cs) << endl;

    shutdown(s, SHUT_RDWR);
    return 0;
}

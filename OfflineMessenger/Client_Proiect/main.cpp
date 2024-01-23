
#include <sys/socket.h>
#include <netinet/in.h>
#include <cerrno>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <arpa/inet.h>
#include <cstring>
#include<iostream>

using namespace std;
extern int errno;
int port;

class Client {
public:
    string Name="Client";
    string Chatter="Server";
    void ChangeName(const string& received_message) {
        string searchWord1 = "ONLINE";
        string searchWord2 = "REGISTERED";
        string searchWord3 = "ALREADY";
        string searchWord4 = "OFFLINE";
        size_t found = received_message.find(searchWord1);

        if (found != string::npos  && received_message.find(searchWord3) == string::npos) {
            string NewName = received_message.substr(1, found-2);
            Name = NewName;
        }
        found= received_message.find(searchWord2);
        if(found != string::npos  && received_message.find(searchWord3) == string::npos){
            string NewName = received_message.substr(1, found-2);
            Name = NewName;
        }
        found= received_message.find(searchWord4);
        if(found != string::npos){
            Name = "Client";
        }
    }
};

class GUI{
public:
    static void Draw() {
        cout<<"|-------------------------------------|\n";
        cout<<"|-------------[MESSENGER]-------------|\n";
        cout<<"|-------------------------------------|\n";
        cout<<"|--------------[COMMANDS]-------------|\n";
        cout<<"|  [Check]->[Afiseaza Useri]          |\n";
        cout<<"|  [Message]->[Incepe o conversatie]  |\n";
        cout<<"|  [End]->[Incheie o conversatie]     |\n";
        cout<<"|  [Quit]->[Iesire]                   |\n";
        cout<<"|  [Reply]->[Raspunde]                |\n";
        cout<<"|  [Edit]->[Editeaza un mesaj]        |\n";
        cout<<"|  [Erase]->[Sterge un mesaj]         |\n";
        cout<<"|  [Group]->[Creeaza un grup]         |\n";
        cout<<"|  [Open]->[Afiseaza conversatia]     |\n";
        cout<<"|-------------------------------------|\n";
    }
};

int main(int argc, char *argv[])
{
    int sd;
    struct sockaddr_in server{};
    char message[100];
    char ServerMesaje[1024];

    Client CLient;
    GUI::Draw();

    if (argc != 3)
    {
        printf("| [Sintaxa]: %s <adresa_server> <port>\n", argv[0]);
        return -1;
    }

    port = static_cast<int> (strtol(argv[2], nullptr, 10));

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("| [Error]:Eroare la socket().\n");
        return errno;
    }


    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(port);

    if (connect(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        perror("| [Client]Eroare la connect().\n");
        return errno;
    }
    while (true)
    {
        printf("| [%s]:", CLient.Name.c_str());
        fflush(stdout);
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = '\0';
        if (write(sd, message, sizeof(message)) <= 0)
        {
            perror("[Client]Eroare la write() spre server.\n");
            return errno;
        }

        if (read(sd, ServerMesaje, sizeof(ServerMesaje)) < 0)
        {
            perror("| [Client]Eroare la read() de la server.\n");
            return errno;
        }

        CLient.ChangeName(ServerMesaje);

        printf("| [%s]:%s\n",CLient.Chatter.c_str(),ServerMesaje);
        if(strcmp(ServerMesaje, "[QUIT]") == 0) {
            break;
        }
    }

    close(sd);
    return 0;
}
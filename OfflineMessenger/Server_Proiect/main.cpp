#include "MessImplement.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
static void *Treat(void *);
void Respond(void *,string&);

extern int errno;

Messenger Messenger;

int main ()
{
    Messenger.Set_Users();
    Messenger.Set_Conversations();
    Messenger.Set_Groups();
    struct sockaddr_in server{};	// structura folosita de server
    struct sockaddr_in from{};
    int sd;		//descriptorul de socket
    pthread_t th[100];    //Identificatorii thread-urilor care se vor crea
    int i=0;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror ("[server]Eroare la socket().\n");
        return errno;
    }
    int on=1;
    setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

    bzero (&server, sizeof (server));
    bzero (&from, sizeof (from));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl (INADDR_ANY);
    server.sin_port = htons (PORT);

    if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
        perror ("[server]Eroare la bind().\n");
        return errno;
    }

    if (listen (sd, 5) == -1)
    {
        perror ("[server]Eroare la listen().\n");
        return errno;
    }

    /* servim in mod concurent clientii...folosind thread-uri */
    while (true)
    {
        int client;
        thData* td; //parametru functia executata de thread
        int length = sizeof (from);

        printf ("[server]Asteptam la portul %d...\n",PORT);
        fflush (stdout);

        if ((client = accept (sd, (struct sockaddr *) &from, reinterpret_cast<socklen_t *>(&length))) < 0)
        {
            perror ("[server]Eroare la accept().\n");
            continue;
        }



        td=(struct thData*)malloc(sizeof(struct thData));
        pthread_mutex_lock(&mutex);
        td->idThread = i++;
        pthread_mutex_unlock(&mutex);
        td->cl=client;

        pthread_create(&th[td->idThread], nullptr, &Treat, td);
    }
}

static void *Treat(void * arg)
{
    struct thData tdL{};
    tdL= *((struct thData*)arg);
    string Command;
    string Message;
    while(true) {
        printf("[Thread]- %d - Asteptam mesajul...\n", tdL.idThread);
        fflush(stdout);
        Respond((struct thData *) arg ,Message);
        if(Message == "Quit"){
            close((intptr_t)arg);
            break;
        }
    }
    return(nullptr);
}

void Respond(void *arg,string& Response)
{
    Response = "";
    struct thData tdL{};
    tdL = *((struct thData *)arg);

    char message[MAX_LENGTH];
    memset(message, 0, MAX_LENGTH);

    string Message;
    vector<string> Messages;
    vector<string> Words;

    if (read(tdL.cl, message, sizeof(message)) <= 0) { printf("[Thread %d]\n", tdL.idThread); perror("Eroare la read() de la client.\n"); }


    Messenger.Check_Users();
    Message=message;
    int type = Messenger.Verify_Command(Message, tdL.cl, Words);
    int i;
    switch (type) {
        case 1:
            Messenger.Login(tdL, Words[1]);
            break;
        case 2:
            Messenger.Register(tdL, Words[1]);
            break;
        case 3:
            Message = Messenger.Logout(tdL);
            break;
        case 4:
            Message =  Messenger.Message(tdL, Words[1]);
            if(Message.find("DOES NOT EXIST") == string::npos) Message += Messenger.AttachConv(tdL);
            break;
        case 5:
            Message = Messenger.Quit(tdL);
            Response = "Quit";
            break;
        case 6:
            Message.clear();
            for(i=2;i<Words.size();i++){
                Message += Words[i];
                Message += " ";
            }
            Messenger.Reply(tdL,Message,Words[1]);
            Message = Messenger.AttachConv(tdL);
            break;
        case 7:
            Message = Messenger.GroupUsers(tdL,Words);
            break;
        case 8:
            Message = Messenger.End(tdL);
            break;
        case 9:
            Message = message;
            Messenger.Send(tdL,Message);
            Message = Messenger.AttachConv(tdL);
            break;
        case 10:
            Message.clear();
            Message = "[USERS]";
            Messages = Messenger.Check_Users();
            for(auto &it:Messages){
                Message += it;
            }
            break;
        case 11:
            Message = Messenger.AttachConv(tdL);
            break;
        case 12:
            Message = Messenger.Erase(tdL,Words[1]);
            Message += "\n\n[-] [CONV] \n";
            Messages = Messenger.Open(tdL);
            for(auto &it:Messages){
                Message += it;
                Message += "\n";
            }
            Message += "[-] [OPEN]\n";
            break;
        case 13:
            Message.clear();
            for(i=2;i<Words.size();i++){
                Message += Words[i];
                Message += " ";
            }
            Message = Messenger.Edit(tdL,Words[1],Message);
            Message += "\n\n[-] [CONV] \n";
            Messages = Messenger.Open(tdL);
            for(auto &it:Messages){
                Message += it;
                Message += "\n";
            }
            Message += "[-] [OPEN]\n";
            break;
        default:
            Message = "Invalid Command";
            break;
    }


    strcpy(message,Message.c_str());

    if(type != 1 && type != 2)
        if (write(tdL.cl, message, sizeof(message)) <= 0) {
            printf("[Thread %d] ", tdL.idThread);
            perror("[Thread]Eroare la write() catre client.\n");
        }
}
#pragma clang diagnostic pop
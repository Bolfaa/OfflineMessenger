//
// Created by bolfa on 17.12.2023.
//

#ifndef SERVER_PROIECT_MESSENGER_H
#define SERVER_PROIECT_MESSENGER_H

#include"Library.h"
typedef struct thData{
    int idThread;
    int cl;
}thData;

class Messenger{
public:
    struct User{
        int id = -1;
        bool online;
        string Username;
        string Chatter;
        User(const string& Username,int id){
            this->id = id;
            this->online = false;
            this->Username = Username;
            this->Chatter = "";
        }
    };
    struct Conversation{
        string Client1;
        string Client2;
        string Name;
        int index=0;
        Conversation(string &Client1, string &Client2){
            this->Client1 = Client1;
            this->Client2 = Client2;
            this->Name = Create_Name(Client1,Client2);
            this->index = 0;
            Create_Conversation(Name);
        }
    };
    struct Group{
        string Name;
        vector<string> UserNames;
        explicit Group(string Name, vector<string> Names) {
            this->Name = std::move(Name);
            UserNames = std::move(Names);
        }
        int index=0;
    };
    vector<Group> Groups;
    vector<User> Users;
    vector<Conversation> Conversations;

    int Verify_Command(string& Command , int &client_id , vector<string> &Words);
    void Set_Users();
    void Set_Conversations();
    void Set_Groups();
    static void Create_Conversation(string& Name);
    static string Create_Name(string& Name1,string& Name2);
    bool Add_Conversation(string& path,string&,string&);
    static bool Add_Name(string &path,string& Name);
    static vector<string> ReadLastLines(string& filePath, int numLines);
    static void WriteInFile(string& filePath,string& message);
    static std::string ReadLineFromFile(const string& Path, int index);
    static bool ChangeLineFromFile(const string& Path, int lineNumber,string Message);
    string AttachConv(thData tdL);
    void CheckUserGroup(thData tdL,bool& user,bool& group);
    static string GetPathForConv(User& Client, bool& exist);

    void Login(thData tdL,string& Name);
    string GroupUsers(thData tdL,vector<string> Words);
    void Verify_Password(thData tdL,string& Name);
    void Set_Password(thData tdL);
    void Register(thData tdL,string& Name);
    string Logout(thData tdL);
    string Quit(thData tdL);
    string Message(thData tdL,string &Name);
    string End(thData tdL);
    vector<string> Check_Users();
    vector<string> Open(thData tdL);
    void Send(thData tdL,string& Message);
    void Reply(thData tdL,string& Message, string& index);
    string Erase(thData tdL,string& index);
    string Edit(thData tdL,string& index,const string& Message);
};


#endif //SERVER_PROIECT_MESSENGER_H

//
// Created by bolfa on 17.12.2023.
//

#ifndef SERVER_PROIECT_MESSIMPLEMENT_H
#define SERVER_PROIECT_MESSIMPLEMENT_H
#include"Files.h"
#include "Messenger.h"

void Messenger::Set_Groups() {
    ifstream inputFile("/home/bolfa/CLionProjects/GROUPS.txt");

    if (!inputFile.is_open()) {
        std::cerr << "Eroare la deschiderea fisierului\n";
    }
    char separator = ' ';
    vector<string> Words;
    string Word;
    string line;
    string Name;
    string line1;
    int index;
    while (getline(inputFile, line)) {
        Words.clear();
        Word.clear();
        Name.clear();
        index=0;
        istringstream parser(line);
        while (getline(parser, Word, separator)) {
            Words.push_back(Word);
        }
        Name = Words[0];
        string Path = "/home/bolfa/CLionProjects/CONVERSATIONS/" + Name + ".txt";
        ifstream inputFile1(Path);
        if (!inputFile1.is_open()) {
            cerr << "Eroare la deschiderea fisierului\n";
        }
        line1.clear();
        while (getline(inputFile1, line1)) {
            index++;
        }
        inputFile1.close();
        Words.erase(Words.begin());
        Group New_Group(Name,Words);
        New_Group.index = index;
        Groups.emplace_back(New_Group);
    }
    inputFile.close();
}

void Messenger::Set_Users() {
    ifstream inputFile("/home/bolfa/CLionProjects/USERS.txt");

    if (!inputFile.is_open()) {
        std::cerr << "Eroare la deschiderea fisierului\n";
    }

    string line;
    while (std::getline(inputFile, line)) {
        Users.emplace_back(line, -1);
    }
    inputFile.close();
}

void Messenger::Set_Conversations() {
    ifstream inputFile("/home/bolfa/CLionProjects/CONVERSATIONS.txt");

    if (!inputFile.is_open()) {
        cerr << "Eroare la deschiderea fisierului\n";
    }
    string line;
    string Nume1, Nume2;
    while (getline(inputFile, line)) {
        size_t pozitieSubliniere = line.find('_');

        if (pozitieSubliniere != string::npos) {
            Nume1 = line.substr(0, pozitieSubliniere);
            Nume2 = line.substr(pozitieSubliniere + 1);
            Conversations.emplace_back(Nume1, Nume2);
        }
    }
    inputFile.close();
    for (auto &it: Conversations) {
        string Path = "/home/bolfa/CLionProjects/CONVERSATIONS/" + it.Name + ".txt";
        ifstream inputFile1(Path);
        if (!inputFile1.is_open()) {
            cerr << "Eroare la deschiderea fisierului\n";
        }
        while (getline(inputFile1, line)) {
            it.index++;
        }
        inputFile1.close();
    }
}

void Messenger::Create_Conversation(string& Name) {
    string Path = "/home/bolfa/CLionProjects/CONVERSATIONS/" + Name + ".txt";

    ifstream file(Path);

    if (file.is_open()) {
        cout << "Conversatia "<< Name <<" exista deja.\n";
    } else {
        ofstream outputFile(Path, ios::app);
        outputFile.close();
        cout << "Conversatia a fost creata.\n";
    }
}

string Messenger::Create_Name(string& Name1,string& Name2) {
    return Name1 + "_" + Name2;
}

bool Messenger::Add_Name(string& path,string& Name) {
    ifstream input(path);
    string line;

    while (getline(input, line)) {
        if (line == Name) {
            input.close();
            return false;
        }
    }
    input.close();

    ofstream output(path, ios::app);
    if (!output.is_open()) {
        cerr << "Eroare la deschiderea fisierului\n";
        return false;
    }

    output << Name << '\n';

    output.close();
    return true;
}

bool Messenger::Add_Conversation(string& path,  string& Client1, string& Client2) {

    ifstream input(path);
    string line;

    for(auto &it:Conversations){
        if(it.Client1 == Client1 && it.Client2 == Client2 || it.Client1 == Client2 && it.Client2 == Client1){
            return false;
        }
    }

    string Name1 = Create_Name(Client1,Client2);
    string Name2 = Create_Name(Client2,Client1);
    while (getline(input, line)) {
        if (line == Name1 || line == Name2) {
            input.close();
            return false;
        }
    }

    input.close();

    ofstream output(path, ios::app);
    if (!output.is_open()) {
        cerr << "Eroare la deschiderea fisierului\n";
        return false;
    }
    output << Name1 << '\n';
    output.close();

    return true;
}

int Messenger::Verify_Command(string& Command,int &client_id,vector<string> &Words) {

    char separator = ' ';
    istringstream parser(Command);
    string Word;
    while (getline(parser, Word, separator)) {
        Words.push_back(Word);
    }
    string VerifyConversation = "Reply Group End Quit Check Open Erase Edit";
    static const std::unordered_map<std::string, int> Commands = {
            {"Login", 1},
            {"Register", 2},
            {"Logout", 3},
            {"Message", 4},
            {"Quit", 5},
            {"Reply", 6},
            {"Group", 7},
            {"End", 8},
            {"Check", 10},
            {"Open", 11},
            {"Erase", 12},
            {"Edit", 13}
    };

    for (const auto& it: Users) {
        if (it.id == client_id && it.online && !it.Chatter.empty() && VerifyConversation.find(Words[0]) == string::npos) {
            return 9;
        }
    }

    auto it = Commands.find(Words[0]);
    if (it != Commands.end())
        return it->second;

    return -1;
}

void Messenger::Login(thData tdL,string& Name) {

    string Message;
    bool ok = true;
    for (auto &i: Users) {
        if (i.Username == Name) {
            ok = false;
            if (i.online) {
                Message = "[" + Name +" ALREADY LOGGED IN]";
                break;
            } else {
                i.online = true;
                i.id = tdL.cl;
                Message = "[ENTER PASSWORD]:";
                break;
            }
        }
    }
    if (ok) {
        for (auto &i: Users)
            if(i.id == tdL.cl) {
                Message = "[YOU ARE ALREADY LOGGED IN]";
                break;
            } else Message = Name +  " [DOES NOT EXIST] [PLZ REGISTER] \n";
    }
    char message[MAX_LENGTH];
    strcpy(message,Message.c_str());

    if( write(tdL.cl, message,sizeof(message) ) <= 0) {
        perror("[Thread]Eroare la write() catre client.\n");
    }
    if(!ok)
    Verify_Password(tdL,Name);
}

void Messenger::Register(thData tdL,string& Name) {
    string Message;
    bool ok = true;
    for (auto &i: Users)
        if(i.id == tdL.cl) {
            Message += "[YOU ARE ALREADY LOGGED IN]";
        }
    for (auto &it: Users) {
        if (it.Username == Name) {
            ok = false;
            Message += "[" + Name + " ALREADY EXISTS]";
        }
    }
    if (ok) {
        User New_User(Name, tdL.cl);
        New_User.online = true;
        Users.emplace_back(New_User);
        Message += "["+ Name + "] [SET PASSWORD]:";
        string path = "/home/bolfa/CLionProjects/USERS.txt";
        Add_Name(path,Name);
    }
    char message[MAX_LENGTH];
    strcpy(message,Message.c_str());
    if(write(tdL.cl, message, sizeof(message)) <= 0) {
        perror("[Thread]Eroare la write() catre client.\n");
    }
    Set_Password(tdL);
}

string Messenger::Logout(thData tdL) {
    string Message;
    bool ok = true;
    for (auto &i: Users)
        if(i.id == tdL.cl) {
            ok=false;
        }
    if(ok){
        Message += "[YOU ARE NOT LOGGED IN]";
        return Message;
    }
    for (auto &it: Users) {
        if (it.id == tdL.cl && it.online){
            it.online = false;
            it.id = -1;
            Message += "[" + it.Username + " OFFLINE]";
            break;
        }
        else if(it.id == tdL.cl && !it.online){
            Message += "[YOU ARE NOT LOGGED IN]";
            break;
        }
    }
    return Message;
}

string Messenger::Quit(thData tdL) {
    string Message;
    for(auto &it:Users){
        if(it.id == tdL.cl){
            it.Chatter = "";
            it.online = false;
            break;
        }
    }
    Message += "[QUIT]";
    return Message;
}

string Messenger::Message(thData tdL, string &Name) {
    string Message;
    bool exist = false,group = false,ok = true;
    string Path = "/home/bolfa/CLionProjects/CONVERSATIONS.txt";
    if (!Users.empty()) {
        for (auto &it: Users) {
            if (it.Username == Name) { exist = true; break;}
        }
    }
    if (!Groups.empty()) {
        for (auto &it: Groups) {
            if (it.Name == Name) { group = true; break;}
        }
    }
    if (exist && !group) {
        for (auto &i: Users) {
            if (i.id == tdL.cl && i.online) {
                ok=false;
                i.Chatter = Name;
                if (Add_Conversation(Path, i.Username, Name)) {
                    Conversation New_Conversation(i.Username, Name);
                    Conversations.push_back(New_Conversation);
                }
                break;
            }
        }
    }

    if(group && !exist){
        for (auto &i: Users) { if (i.id == tdL.cl && i.online) {
                for(auto &it:Groups) for(auto &j:it.UserNames){ if(j == i.Username){ ok =false;i.Chatter = it.Name; break;} }
            }
        }
    }



    if(!group && !exist) {
        Message += "[USER/GROUP " + Name + " DOES NOT EXIST]";
    }
    if(ok){
        Message += "[CANNOT SEND MESSAGE]";
    }
    return Message;
}

string Messenger::End(thData tdL) {
    string Message;
    bool ok=true;
    for(auto &it:Users){
        if(it.id == tdL.cl && it.online && !it.Chatter.empty()){
            ok=false;
            it.Chatter = "";
            Message += "[END]";
            break;
        }
    }
    if(ok){
        Message += "[YOU ARE NOT IN A CONVERSATION]";
    }
    return Message;
}

vector<string> Messenger::Check_Users() {
    vector<string> Messages;
    Messages.clear();
    Messages.emplace_back("\n|-------------------------------------|\n");
    string Message;
    int index=0;

    for(auto &it:Users){
        Message.clear();
        index++;
        Message += " [" + to_string(index) + "] ";

        if(it.online){ Message += it.Username + " IS ONLINE\n"; }
        else{ Message += it.Username + " IS OFFLINE\n"; }

        Messages.emplace_back(Message);
    }
    for(auto &it:Groups){
        Message.clear();
        index++;
        Message += " [" + to_string(index) + "] [GROUP] " + "[" + it.Name +"]:";
        for(auto &i:it.UserNames){
            Message += " [" + i + "]";
        }
        Message += "\n";
        Messages.emplace_back(Message);
    }
    Messages.emplace_back("|-------------------------------------|");
    return Messages;
}

vector<string> Messenger::Open(thData tdL) {

    vector<string> Message;
    string Path= "/home/bolfa/CLionProjects/CONVERSATIONS/";
    string ConvName1,ConvName2;

    bool user = false,group = false,ok = false;
    CheckUserGroup(tdL,user,group);

    if(user) {
        for (auto &it: Users) {
            if (it.id == tdL.cl && it.online && !it.Chatter.empty()) {
                Path = GetPathForConv(it, ok);
            }
        }
        if(ok) Message = ReadLastLines(Path, 20);
    }
    else if(group) {
        Path = "/home/bolfa/CLionProjects/CONVERSATIONS/";
        for (auto &it: Users)
            if (it.id == tdL.cl) {
                for (auto &i: Groups)
                    if (it.Chatter == i.Name) {
                        Path += i.Name + ".txt";
                        Message = ReadLastLines(Path, 20);
                        break;
                    }
            }
    }
    if(!user && !group) {
        Message.emplace_back("YOU ARE NOT IN A CONVERSATION");
    }
    return Message;
}

void Messenger::Send(thData tdL,string& Message) {
    bool exist = false;
    string Path = "/home/bolfa/CLionProjects/CONVERSATIONS/";
    string ConvName1, ConvName2, ConvName;
    string NewMessage;

    bool user = false, group = false;
    CheckUserGroup(tdL, user, group);

    if (user) {
        for (auto &i: Users) {
            if (i.id == tdL.cl && i.online && !i.Chatter.empty()) {
                Path = GetPathForConv(i, exist);
                if (exist) {
                    for (auto &it: Conversations) {
                        if (it.Name == Create_Name(i.Username, i.Chatter) ||
                            it.Name == Create_Name(i.Chatter, i.Username)) {
                            NewMessage += "[" + to_string(it.index) + "] " + "[" + i.Username + "]: " + Message;
                            it.index++;
                            WriteInFile(Path, NewMessage);
                            break;
                        }
                    }
                }
            }
        }
    } else if (group) {
        Path = "/home/bolfa/CLionProjects/CONVERSATIONS/";
        for (auto &it: Users) {
            if (it.id == tdL.cl && it.online && !it.Chatter.empty()) {
                for (auto &i: Groups)
                    if (it.Chatter == i.Name) {
                        Path += i.Name + ".txt";
                        NewMessage = "[" + to_string(i.index) + "] " + "[" + it.Username + "]: " + Message;
                        i.index++;
                        WriteInFile(Path, NewMessage);
                        break;
                    }

            }
        }
    }
}

void Messenger::Reply(thData tdL, string &Message,string& index){
    bool exist=false;
    string Path = "/home/bolfa/CLionProjects/CONVERSATIONS/";
    string ConvName1,ConvName2,ConvName;
    string NewMessage;

    bool user = false,group = false;
    CheckUserGroup(tdL,user,group);

    if(user) {
        for (auto &i: Users) {
            if (i.id == tdL.cl && i.online && !i.Chatter.empty()) {
                Path = GetPathForConv(i, exist);
                if (exist) {
                    for (auto &it: Conversations) {
                        if (it.Name == Create_Name(i.Username, i.Chatter) ||
                            it.Name == Create_Name(i.Chatter, i.Username) && it.index > stoi(index)){
                            NewMessage +="[" + to_string(it.index) + "] " + "[" + i.Username + "] [REPLY] [" + index + "]: ";
                            NewMessage += Message;
                            it.index++;
                            WriteInFile(Path, NewMessage);
                            break;
                        }
                    }
                }
            }
        }
    }
    else if(group){
        Path = "/home/bolfa/CLionProjects/CONVERSATIONS/";
        for (auto &it: Users) {
            if (it.id == tdL.cl && it.online && !it.Chatter.empty()) {
                for (auto &i: Groups)
                    if (it.Chatter == i.Name && i.index > stoi(index)) {
                        Path += i.Name + ".txt";
                        NewMessage = "[" + to_string(i.index) + "] " + "[" + it.Username + "] [REPLY] [" + index + "]: ";
                        NewMessage += Message;
                        i.index++;
                        WriteInFile(Path, NewMessage);
                        break;
                    }
            }
        }
    }
}

string Messenger::Erase(thData tdL, string &index) {
    string Message;
    Message = Edit(tdL,index,"[DELETED]");
    return Message;
}

string Messenger::Edit(thData tdL, string &index, const string& Message) {
    string MessageUser;
    string NewMessage;

    bool user = false,group = false;
    CheckUserGroup(tdL,user,group);

    if(user) {
        for (auto &it: Users) {
            if (tdL.cl == it.id && it.online && !it.Chatter.empty()) {
                for (auto &i: Conversations) {
                    if (i.Name == Create_Name(it.Username, it.Chatter) ||
                        i.Name == Create_Name(it.Chatter, it.Username)) {
                        string Path = "/home/bolfa/CLionProjects/CONVERSATIONS/" + i.Name + ".txt";
                        string Line = ReadLineFromFile(Path, stoi(index) + 1);
                        size_t found = Line.find(it.Username);
                        if (found != string::npos) {

                            size_t start = Line.find('[');
                            size_t end = Line.find(']', start);
                            string number;
                            if (start != string::npos && end != string::npos && start < end) {
                                number = Line.substr(start + 1, end - start - 1);
                            }

                            if (Message == "[DELETED]") {
                                NewMessage = "[" + number + "] " + "[" + it.Username + "] ";
                                NewMessage += Message;
                            } else {
                                NewMessage = "[" + number + "] " + "[" + it.Username + "] " + "[EDITED]:";
                                NewMessage += Message;
                            }
                            if (ChangeLineFromFile(Path, stoi(index) + 1, NewMessage)) {
                                if (Message == "[DELETED]") MessageUser += "[MESSAGE DELETED]";
                                else MessageUser += "[MESSAGE EDITED]";
                            }
                        } else {
                            MessageUser += "[NOT YOUR MESSAGE]";
                        }
                    }
                }
            }
        }
    } else {
        for (auto &it: Users) {
            if (tdL.cl == it.id && it.online && !it.Chatter.empty()) {
                for (auto &i: Groups)
                    if (it.Chatter == i.Name) {
                        string Path = "/home/bolfa/CLionProjects/CONVERSATIONS/" + i.Name + ".txt";
                        string Line = ReadLineFromFile(Path, stoi(index) + 1);
                        size_t found = Line.find(it.Username);
                        if (found != string::npos) {

                            size_t start = Line.find('[');
                            size_t end = Line.find(']', start);
                            string number;
                            if (start != string::npos && end != string::npos && start < end) {
                                number = Line.substr(start + 1, end - start - 1);
                            }

                            if (Message == "[DELETED]") {
                                NewMessage = "[" + number + "] " + "[" + it.Username + "] ";
                                NewMessage += Message;
                            } else {
                                NewMessage = "[" + number + "] " + "[" + it.Username + "] " + "[EDITED]:";
                                NewMessage += Message;
                            }
                            if (ChangeLineFromFile(Path, stoi(index) + 1, NewMessage)) {
                                if (Message == "[DELETED]") MessageUser += "[MESSAGE DELETED]";
                                else MessageUser += "[MESSAGE EDITED]";
                            }
                        } else {
                            MessageUser += "[NOT YOUR MESSAGE]";
                        }
                    }

            }
        }
    }
    if (!user && !group) {
        MessageUser = "YOU ARE NOT IN A CONVERSATION";
    }
    return MessageUser;
}

string Messenger::AttachConv(thData tdL) {
    string Message;
    vector<string> Messages;
    Message = "[CONVERSATION] \n";
    Message += "\n[-] [CONV] \n";
    Messages = Open(tdL);
    for(auto &it:Messages){
        Message += it;
        Message += "\n";
    }
    Message += "[-] [OPEN]\n";
    return Message;
}

void Messenger::Verify_Password(thData tdL, string &Name) {
    string Message;
    char message[MAX_LENGTH];
    if (read(tdL.cl, message, sizeof(message)) <= 0)
    {
        printf("[Thread %d]\n", tdL.idThread);
        perror("Eroare la read() de la client.\n");
    }
    string Password = message;
    string path = "/home/bolfa/CLionProjects/USERS.txt";
    ifstream inputFile(path);
    string line;
    int index = 0;
    while (getline(inputFile, line)) {
        if (line == Name) break;
        index++;
    }
    inputFile.close();
    path = "/home/bolfa/CLionProjects/PASSWORD.txt";
    string TruePassword = ReadLineFromFile(path, index+1);
    if(TruePassword == Password){
        Message += "[" + Name + " ONLINE] [PASSWORD OK]";
        vector<string> Messages = Check_Users();
        for(auto &it:Messages){
            Message += it;
        }
    }
    else{
        Message += "[PASSWORD WRONG]";
        for(auto &it : Users){
            if(it.Username == Name){
                it.online = false;
                it.id = -1;
                break;
            }
        }
    }
    strcpy(message,Message.c_str());

    if(write(tdL.cl, message, sizeof(message)) <= 0)
    {
        printf("[Thread %d] ", tdL.idThread);
        perror("[Thread]Eroare la write() catre client.\n");
    }
    else
        printf("[Thread %d]Mesajul a fost trasmis cu succes.\n", tdL.idThread);
}

void Messenger::Set_Password(thData tdL) {
    string Message;
    char message[MAX_LENGTH];
    if (read(tdL.cl, message, sizeof(message)) <= 0)
    {
        printf("[Thread %d]\n", tdL.idThread);
        perror("Eroare la read() de la client.\n");
    }
    string Password = message;
    string path = "/home/bolfa/CLionProjects/PASSWORD.txt";
    WriteInFile(path,Password);
    Message += "[PASSWORD SET]";

    vector<string> Messages = Check_Users();
    for(auto &it:Messages){
        Message += it;
    }
    strcpy(message,Message.c_str());

    if(write(tdL.cl, message, sizeof(message)) <= 0)
    {
        printf("[Thread %d] ", tdL.idThread);
        perror("[Thread]Eroare la write() catre client.\n");
    }
    else
        printf("[Thread %d]Mesajul a fost trasmis cu succes.\n", tdL.idThread);
}

string Messenger::GroupUsers(thData tdL, vector<string> Words) {

    for(auto &it: Users)
        if(it.id == tdL.cl && it.online) Words.push_back(it.Username);

    string Name= Words[1];
    string Path = "/home/bolfa/CLionProjects/GROUPS.txt";
    string Message;
    bool ok = true;
    for (auto &it: Groups) {
        if (it.Name == Name) {
            ok = false;
            Message += "[GROUP " + Name + " ALREADY EXISTS]";
            break;
        }
    }
    if(ok)
    {
        Words.erase(Words.begin());
        Words.erase(Words.begin());
        Group New_Group(Name,Words);
        Groups.emplace_back(New_Group);

        ofstream outputFile(Path, ios::app);
        outputFile << Name;

        for(auto &it:Words)
            outputFile << " " << it;
            outputFile<<"\n";
        outputFile.close();
        Path = "/home/bolfa/CLionProjects/CONVERSATIONS.txt";
        WriteInFile(Path,Name);
        Create_Conversation(Name);
        Message += "[GROUP " + Name + " CREATED]";
    }
    return Message;
}

void Messenger::CheckUserGroup(thData tdL, bool& user, bool& group) {

    for(auto &i: Users) {
        for (auto &it: Users) {
            if (i.Chatter == it.Username && i.id == tdL.cl) {
                user = true;
                break;
            }
        }
        for (auto &it: Groups) {
            if (it.Name == i.Chatter && i.id == tdL.cl) {
                group = true;
                break;
            }
        }
    }
}

string Messenger::GetPathForConv(Messenger::User &Client,bool& exist) {
    string Path = "/home/bolfa/CLionProjects/CONVERSATIONS/";
    Path += Client.Username + "_" + Client.Chatter + ".txt";
    string ConvName;
    if (filesystem::exists(Path)) {
        exist = true;
        ConvName = Client.Username + "_" + Client.Chatter + ".txt";
    } else {
        Path = "/home/bolfa/CLionProjects/CONVERSATIONS/";
        Path += Client.Chatter + "_" + Client.Username + ".txt";
        if (filesystem::exists(Path)) {
            exist = true;
            ConvName = Client.Chatter + "_" + Client.Username + ".txt";
        }
    }
    Path = "/home/bolfa/CLionProjects/CONVERSATIONS/";
    Path += ConvName;
    return Path;
}

#endif //SERVER_PROIECT_MESSIMPLEMENT_H

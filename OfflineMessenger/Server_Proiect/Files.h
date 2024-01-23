//
// Created by bolfa on 17.12.2023.
//

#ifndef SERVER_PROIECT_FILES_H
#define SERVER_PROIECT_FILES_H
#include "Messenger.h"

void Messenger::WriteInFile(string& Path,string& message) {

    ofstream File(Path, ios::app);
    if (!File.is_open()) {
        cerr << "Eroare la deschiderea fisierului\n";
        return;
    }
    File << message << '\n';
    File.close();
}

vector<string> Messenger::ReadLastLines(string &Path, int numLines) {
    ifstream File(Path);

    if (!File.is_open()) {
        cerr << "Eroare la deschiderea fisierului\n";
        return {};
    }

    vector<string> lines;
    string line;

    while (getline(File, line)) {
        lines.push_back(line);
    }

    int start = max(0, static_cast<int>(lines.size()) - numLines);
    vector<string> lastLines(lines.begin() + start, lines.end());

    return lastLines;
}

string Messenger::ReadLineFromFile(const string &Path, int index) {

    ifstream File(Path);
    string Line;

    if (File.is_open()) {
        for (int currentLine = 0; currentLine < index; currentLine++) {
            getline(File, Line);
        }
        File.close();
    }

    return Line;
}

bool Messenger::ChangeLineFromFile(const string &Path, int Number ,string Message) {
    ifstream input(Path);
    vector<string> lines;

    if (!input.is_open()) {
        return false;
    }

    string line;
    while (std::getline(input, line)) {
        lines.push_back(line);
    }

    input.close();

    if (Number < 1 || Number > lines.size()) {
        return false;
    }

    lines[Number-1] = std::move(Message);

    ofstream output(Path);
    if (!output.is_open()) {
        return false;
    }

    for (const auto& updatedLine : lines) {
        output << updatedLine << endl;
    }

    output.close();

    return true;
}

#endif //SERVER_PROIECT_FILES_H

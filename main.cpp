#include <ctime>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

const string FILE_NAME = "tasks.json";

string nowTime() {
    time_t now = time(nullptr);
    tm localTime{};

#ifdef _WIN32
    localtime_s(&localTime, &now);
#else
    localtime_r(&now, &localTime);
#endif

    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &localTime);
    return buffer;
}

string escapeJson(const string& text) {
    string result;

    for (char ch : text) {
        if (ch == '\\') {
            result += "\\\\";
        } else if (ch == '"') {
            result += "\\\"";
        } else if (ch == '\n') {
            result += "\\n";
        } else {
            result += ch;
        }
    }

    return result;
}

string unescapeJson(const string& text) {
    string result;

    for (size_t i = 0; i < text.length(); i++) {
        if (text[i] == '\\' && i + 1 < text.length()) {
            i++;

            if (text[i] == 'n') {
                result += '\n';
            } else {
                result += text[i];
            }
        } else {
            result += text[i];
        }
    }

    return result;
}

void createFileIfMissing() {
    if (!filesystem::exists(FILE_NAME)) {
        ofstream file(FILE_NAME);
        file << "[]\n";
    }
}

string getStringValue(const string& object, const string& key) {
    string search = "\"" + key + "\": \"";
    size_t start = object.find(search);

    if (start == string::npos) {
        return "";
    }

    start += search.length();
    string value;
    bool escaping = false;

    for (size_t i = start; i < object.length(); i++) {
        char ch = object[i];

        if (escaping) {
            value += '\\';
            value += ch;
            escaping = false;
        } else if (ch == '\\') {
            escaping = true;
        } else if (ch == '"') {
            break;
        } else {
            value += ch;
        }
    }

    return unescapeJson(value);
}

int getIntValue(const string& object, const string& key) {
    string search = "\"" + key + "\": ";
    size_t start = object.find(search);

    if (start == string::npos) {
        return 0;
    }

    start += search.length();
    size_t end = object.find_first_of(",\n", start);
    return stoi(object.substr(start, end - start));
}

void loadTasks(vector<int>& ids, vector<string>& descriptions, vector<string>& statuses,
               vector<string>& createdTimes, vector<string>& updatedTimes) {
    createFileIfMissing();

    ifstream file(FILE_NAME);
    stringstream buffer;
    buffer << file.rdbuf();
    string content = buffer.str();

    size_t position = 0;

    while (true) {
        size_t start = content.find('{', position);
        if (start == string::npos) {
            break;
        }

        size_t end = content.find('}', start);
        if (end == string::npos) {
            break;
        }

        string object = content.substr(start, end - start + 1);

        ids.push_back(getIntValue(object, "id"));
        descriptions.push_back(getStringValue(object, "description"));
        statuses.push_back(getStringValue(object, "status"));
        createdTimes.push_back(getStringValue(object, "createdAt"));
        updatedTimes.push_back(getStringValue(object, "updatedAt"));

        position = end + 1;
    }
}

void saveTasks(const vector<int>& ids, const vector<string>& descriptions,
               const vector<string>& statuses, const vector<string>& createdTimes,
               const vector<string>& updatedTimes) {
    ofstream file(FILE_NAME);
    file << "[\n";

    for (size_t i = 0; i < ids.size(); i++) {
        file << "  {\n";
        file << "    \"id\": " << ids[i] << ",\n";
        file << "    \"description\": \"" << escapeJson(descriptions[i]) << "\",\n";
        file << "    \"status\": \"" << statuses[i] << "\",\n";
        file << "    \"createdAt\": \"" << createdTimes[i] << "\",\n";
        file << "    \"updatedAt\": \"" << updatedTimes[i] << "\"\n";
        file << "  }";

        if (i + 1 < ids.size()) {
            file << ",";
        }

        file << "\n";
    }

    file << "]\n";
}

int findTaskIndex(const vector<int>& ids, int id) {
    for (size_t i = 0; i < ids.size(); i++) {
        if (ids[i] == id) {
            return static_cast<int>(i);
        }
    }

    return -1;
}

bool isNumber(const string& text) {
    if (text.empty()) {
        return false;
    }

    for (char ch : text) {
        if (!isdigit(static_cast<unsigned char>(ch))) {
            return false;
        }
    }

    return true;
}

bool validStatus(const string& status) {
    return status == "todo" || status == "in-progress" || status == "done";
}

void printTask(int id, const string& description, const string& status,
               const string& createdAt, const string& updatedAt) {
    cout << "[" << id << "] " << description << "\n";
    cout << "    Status: " << status << "\n";
    cout << "    Created: " << createdAt << "\n";
    cout << "    Updated: " << updatedAt << "\n";
}

void printHelp() {
    cout << "Usage:\n";
    cout << "  task-cli add \"task description\"\n";
    cout << "  task-cli update <id> \"new description\"\n";
    cout << "  task-cli delete <id>\n";
    cout << "  task-cli mark-in-progress <id>\n";
    cout << "  task-cli mark-done <id>\n";
    cout << "  task-cli list\n";
    cout << "  task-cli list <todo|in-progress|done>\n";
}

int main(int argc, char* argv[]) {
    vector<int> ids;
    vector<string> descriptions;
    vector<string> statuses;
    vector<string> createdTimes;
    vector<string> updatedTimes;

    loadTasks(ids, descriptions, statuses, createdTimes, updatedTimes);

    if (argc < 2) {
        printHelp();
        return 1;
    }

    string command = argv[1];

    if (command == "add") {
        if (argc < 3) {
            cout << "Usage: task-cli add \"task description\"\n";
            return 1;
        }

        int nextId = 1;
        for (int id : ids) {
            if (id >= nextId) {
                nextId = id + 1;
            }
        }

        string time = nowTime();
        ids.push_back(nextId);
        descriptions.push_back(argv[2]);
        statuses.push_back("todo");
        createdTimes.push_back(time);
        updatedTimes.push_back(time);

        saveTasks(ids, descriptions, statuses, createdTimes, updatedTimes);
        cout << "Task added successfully (ID: " << nextId << ")\n";
    } else if (command == "update") {
        if (argc < 4 || !isNumber(argv[2])) {
            cout << "Usage: task-cli update <id> \"new description\"\n";
            return 1;
        }

        int id = stoi(argv[2]);
        int index = findTaskIndex(ids, id);

        if (index == -1) {
            cout << "Task not found.\n";
            return 1;
        }

        descriptions[index] = argv[3];
        updatedTimes[index] = nowTime();
        saveTasks(ids, descriptions, statuses, createdTimes, updatedTimes);
        cout << "Task updated successfully.\n";
    } else if (command == "delete") {
        if (argc < 3 || !isNumber(argv[2])) {
            cout << "Usage: task-cli delete <id>\n";
            return 1;
        }

        int id = stoi(argv[2]);
        int index = findTaskIndex(ids, id);

        if (index == -1) {
            cout << "Task not found.\n";
            return 1;
        }

        ids.erase(ids.begin() + index);
        descriptions.erase(descriptions.begin() + index);
        statuses.erase(statuses.begin() + index);
        createdTimes.erase(createdTimes.begin() + index);
        updatedTimes.erase(updatedTimes.begin() + index);

        saveTasks(ids, descriptions, statuses, createdTimes, updatedTimes);
        cout << "Task deleted successfully.\n";
    } else if (command == "mark-in-progress" || command == "mark-done") {
        if (argc < 3 || !isNumber(argv[2])) {
            cout << "Usage: task-cli " << command << " <id>\n";
            return 1;
        }

        int id = stoi(argv[2]);
        int index = findTaskIndex(ids, id);

        if (index == -1) {
            cout << "Task not found.\n";
            return 1;
        }

        statuses[index] = command == "mark-done" ? "done" : "in-progress";
        updatedTimes[index] = nowTime();
        saveTasks(ids, descriptions, statuses, createdTimes, updatedTimes);
        cout << "Task marked as " << statuses[index] << ".\n";
    } else if (command == "list") {
        string filter = "";

        if (argc >= 3) {
            filter = argv[2];

            if (!validStatus(filter)) {
                cout << "Invalid status. Use: todo, in-progress, done\n";
                return 1;
            }
        }

        bool found = false;

        for (size_t i = 0; i < ids.size(); i++) {
            if (filter.empty() || statuses[i] == filter) {
                printTask(ids[i], descriptions[i], statuses[i], createdTimes[i], updatedTimes[i]);
                found = true;
            }
        }

        if (!found) {
            cout << "No tasks found.\n";
        }
    } else {
        cout << "Unknown command.\n";
        printHelp();
        return 1;
    }

    return 0;
}

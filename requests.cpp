#include <iostream>
#include <string>
#include <map>
#include <cstdlib> 

void addTask(const std::string& description);
std::string listTasks();
void completeTask(int taskId);

// Parse CGI-style form data (e.g., action=add&description=New+Task)
std::map<std::string, std::string> parseFormData(const std::string& query) {
    std::map<std::string, std::string> formData;
    size_t pos = 0;
    std::string token;
    std::string queryCopy = query; 


    while ((pos = queryCopy.find("&")) != std::string::npos) {
        token = queryCopy.substr(0, pos);
        size_t delimiterPos = token.find("=");
        if (delimiterPos != std::string::npos) {
            formData[token.substr(0, delimiterPos)] = token.substr(delimiterPos + 1);
        }
        queryCopy.erase(0, pos + 1); 
    }

    size_t delimiterPos = queryCopy.find("=");
    if (delimiterPos != std::string::npos) {
        formData[queryCopy.substr(0, delimiterPos)] = queryCopy.substr(delimiterPos + 1);
    }

    return formData;
}

int main() {
    std::cout << "Content-type: text/html\n\n";

   
    std::string queryString = getenv("QUERY_STRING") ? getenv("QUERY_STRING") : "";

    if (!queryString.empty()) {
        std::map<std::string, std::string> formData = parseFormData(queryString);
        if (formData.find("action") != formData.end() && formData["action"] == "add" && formData.find("description") != formData.end()) {
            std::string description = formData["description"];
            addTask(description); 
            std::cout << "<p>Task added: " << description << "</p>";
        }

        if (formData.find("action") != formData.end() && formData["action"] == "complete" && formData.find("id") != formData.end()) {
            int taskId = std::stoi(formData["id"]);
            completeTask(taskId); 
            std::cout << "<p>Task ID " << taskId << " marked as completed.</p>";
        }
    }

    
    std::cout << "<h1>Task List</h1>";
    std::cout << listTasks(); 

    std::cout << "<form method='get'>";
    std::cout << "<input type='text' name='description' placeholder='New Task'>";
    std::cout << "<input type='hidden' name='action' value='add'>";
    std::cout << "<input type='submit' value='Add Task'>";
    std::cout << "</form>";

    std::cout << "<form method='get'>";
    std::cout << "<input type='number' name='id' placeholder='Task ID'>";
    std::cout << "<input type='hidden' name='action' value='complete'>";
    std::cout << "<input type='submit' value='Mark Completed'>";
    std::cout << "</form>";

    return 0;
}

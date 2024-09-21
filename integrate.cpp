#include <iostream>
#include <SQLiteCpp/SQLiteCpp.h>
#include <string>
#include <map>

// Task Struct
struct Task {
    int id;
    std::string description;
    bool completed;
};

// Initialize database: creates table if it doesn't exist
void initDatabase() {
    try {
        SQLite::Database db("tasks.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
        db.exec("CREATE TABLE IF NOT EXISTS tasks (id INTEGER PRIMARY KEY, description TEXT, completed INTEGER);");
    } catch (std::exception& e) {
        std::cerr << "Error initializing database: " << e.what() << std::endl;
    }
}

// Add a task to the database
void addTask(const std::string& description) {
    try {
        SQLite::Database db("tasks.db", SQLite::OPEN_READWRITE);
        SQLite::Statement query(db, "INSERT INTO tasks (description, completed) VALUES (?, 0)");
        query.bind(1, description);
        query.exec();
        std::cout << "Task added: " << description << std::endl;
    } catch (std::exception& e) {
        std::cerr << "Error adding task: " << e.what() << std::endl;
    }
}

// List all tasks in the database
std::string listTasks() {
    std::string response;
    try {
        SQLite::Database db("tasks.db", SQLite::OPEN_READONLY);
        SQLite::Statement query(db, "SELECT id, description, completed FROM tasks");

        response = "<ul>";
        while (query.executeStep()) {
            int id = query.getColumn(0);
            std::string description = query.getColumn(1);
            bool completed = query.getColumn(2);
            response += "<li>" + std::to_string(id) + ": " + description + (completed ? " (Completed)" : " (Pending)") + "</li>";
        }
        response += "</ul>";
    } catch (std::exception& e) {
        std::cerr << "Error listing tasks: " << e.what() << std::endl;
    }
    return response;
}

// Mark task as completed in the database
void completeTask(int taskId) {
    try {
        SQLite::Database db("tasks.db", SQLite::OPEN_READWRITE);
        SQLite::Statement query(db, "UPDATE tasks SET completed = 1 WHERE id = ?");
        query.bind(1, taskId);
        query.exec();
        std::cout << "Task ID " << taskId << " marked as completed." << std::endl;
    } catch (std::exception& e) {
        std::cerr << "Error updating task: " << e.what() << std::endl;
    }
}

// Parse CGI-style form data (e.g., action=add&description=Test+Task)
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
    return formData;
}

int main() {

    initDatabase();

    std::cout << "Content-type: text/html\n\n";
    std::string queryString = getenv("QUERY_STRING") ? getenv("QUERY_STRING") : "";

    if (!queryString.empty()) {
        std::map<std::string, std::string> formData = parseFormData(queryString);

        if (formData["action"] == "add" && formData.find("description") != formData.end()) {
            addTask(formData["description"]);
        } else if (formData["action"] == "complete" && formData.find("id") != formData.end()) {
            int taskId = std::stoi(formData["id"]);
            completeTask(taskId);
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

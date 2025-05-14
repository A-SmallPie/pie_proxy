class Connection;

enum class TaskType {
    ADD_CONNECTION,
    DEL_CONNECTION
};

struct Task{
    Connection* connection;
    TaskType type;
    Task(Connection* conn, TaskType type);
};
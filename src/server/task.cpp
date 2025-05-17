#include "server/task.hpp"

Task::Task(Connection* conn, TaskType t){
    connection = conn;
    type=t;
}
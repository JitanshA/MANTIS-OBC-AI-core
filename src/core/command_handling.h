#ifndef COMMAND_HANDLING_H
#define COMMAND_HANDLING_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include "protocol.pb.h"

extern std::queue<command::Command> commandQueue;
extern std::mutex queueMutex;
extern std::condition_variable queueCondVar;

void processCommand();

#endif // COMMAND_HANDLING_H
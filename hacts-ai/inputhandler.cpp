#include "inputhandler.h"

InputHandler::InputHandler() :
    inputReadingThread(onInputThread)
{
}

InputHandler::~InputHandler()
{
    // std::thread kills the whole program if we don't either .detach() its thread
    // from it or wait for its completion using .join()
    inputReadingThread.detach();

    std::lock_guard<std::mutex> lock(mutex);

    // tell onInputThread() to return
    if(end_thread)
        *end_thread = true;

    // If onInputThread() is waiting on processed_command until getAvailableInput()
    // is called, make it advance to if(end_thread)
    processed_command.notify_all();
}

bool InputHandler::getAvailableInput(std::string &command) {
    std::lock_guard<std::mutex> lock(mutex);

    // Has anything been read since last call?
    if(! has_command)
        return false;

    command = this->command;

    has_command = false;

    // tell inputThread to read next line
    processed_command.notify_all();

    return true;
}

void InputHandler::onInputThread() {
    // Give InputHandler a way to notify us of destruction that doesn't go away on end of object's lifetime
    bool end_thread = false;
    this->end_thread = &end_thread;

    std::string command;
    while(std::getline(cin, command)) {

        // TODO: parse command here...

        // check if we are still alive and can actually take a lock
        if(end_thread) return;

        // take lock
        std::unique_lock<std::mutex> lock(mutex);

        // check again with the lock taken
        if(end_thread) return;

        // Give the read command
        this->command = command;

        // wait for the main thread to process the command, unblocking comman_mutex
        processed_command.wait(lock);

        // Check again, maybe we were destroyed
        if(end_thread) return;
    }
}

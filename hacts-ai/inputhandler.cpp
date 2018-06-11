#include <iostream>

#include "inputhandler.h"


InputHandler::InputHandler()
{
    threadObject = new InputHandlerThread;

    // Assign shared pointers
    threadObject->command = command;
    threadObject->end_thread = end_thread;
    threadObject->has_command = has_command;
    threadObject->mutex = mutex;
    threadObject->processed_command = processed_command;

    // start the thread
    inputReadingThread = new std::thread(*threadObject);
}

InputHandler::~InputHandler()
{
    // std::thread kills the whole program if we don't either .detach() its thread
    // from it or wait for its completion using .join()
    inputReadingThread->detach();
    delete inputReadingThread;

    std::lock_guard<std::mutex> lock(*mutex);

    // tell onInputThread() to return and delete itself
    *end_thread = true;

    // If onInputThread() is waiting on processed_command until getAvailableInput()
    // is called, make it advance to if(end_thread)
    processed_command->notify_all();
}

bool InputHandler::getAvailableInput(std::string &command) {
    std::lock_guard<std::mutex> lock(*mutex);

    // Has anything been read since last call?
    if(*has_command == false)
        return false;

    command = *this->command;

    *has_command = false;

    // tell inputThread to read next line
    processed_command->notify_all();

    return true;
}


// InputHandlerThread:

void InputHandler::InputHandlerThread::operator()() {
    run();

    // the thread has ended, we aren't needed anymore
    delete this;
}

void InputHandler::InputHandlerThread::run() {
    std::string command;
    while(std::getline(std::cin, command)) {

        // TODO: parse command here...

        // take lock
        std::unique_lock<std::mutex> lock(*mutex);

        // check again with the lock taken
        if(*end_thread) return;

        // Give the read command
        *this->command = command;
        *has_command = true;

        // wait for the main thread to process the command, unblocking comman_mutex
        processed_command->wait(lock);

        // Check again, maybe we were destroyed
        if(*end_thread) return;
    }
}

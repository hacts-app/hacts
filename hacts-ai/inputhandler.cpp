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
    threadObject->received_input = received_input;

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

    return getAvailableInputWithoutLock(command);
}

bool InputHandler::waitForInput(std::string &command)
{
    std::unique_lock<std::mutex> lock(*mutex);

    // Do we have some input data queued?
    if(getAvailableInputWithoutLock(command))
        return true;

    // Wait until some input data is received
    received_input->wait(lock);

    return getAvailableInputWithoutLock(command);
}

bool InputHandler::getAvailableInputWithoutLock(std::string &command) {
    // Never call this method without a lock active!

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

        // Check if we were destroyed and should quit
        if(*end_thread) return;

        // Give the read data to the main thread
        *this->command = command;
        *has_command = true;

        // If waitForInput is waiting for data, notify it
        received_input->notify_all();

        // wait for the main thread to process the command, unblocking comman_mutex
        processed_command->wait(lock);

        // Check again if we were destroyed
        if(*end_thread) return;
    }
}

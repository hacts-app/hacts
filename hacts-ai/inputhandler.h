#pragma once

#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <istream>
#include <atomic>
#include <memory>

/**
 * InputHandler starts a thread which reads line by line from std::cin in the background
 * and provides a way to a asynchronously read available input
 */
class InputHandler
{
public:
    explicit InputHandler();
    ~InputHandler();

    /**
     * Disallow copying
     */
    InputHandler(const InputHandler &) = delete;

    /**
     * @brief getAvailableInput gets input which have been read and saved from std::cin
     * @param command - saved input
     * @return true if there was some input
     */
    bool getAvailableInput(std::string &command);

    /**
     * behaves like getAvailableInput but waits for input if there isn't any
     */
    bool waitForInput(std::string &command);

private:
    class InputHandlerThread
    {
    public:
        void operator()();

        /*
         * Variables shared with InputHandler, documentation in InputHandler
         */
        std::shared_ptr<std::mutex> mutex;
        std::shared_ptr<bool> end_thread;
        std::shared_ptr<std::string> command;
        std::shared_ptr<bool> has_command;
        std::shared_ptr<std::condition_variable> processed_command;
        std::shared_ptr<std::condition_variable> received_input;
    private:

        void run();
    };

    bool getAvailableInputWithoutLock(std::string &command);

    /**
     * The thread which runs InputHandlerThread
     */
    std::thread *inputReadingThread;
    InputHandlerThread *threadObject;

    /*
     * The variables below are owned by both InputHandler and InputHandlerThread
     */

    /**
     * This mutex is owned by both InputHandler and onInputThread() and guards access to
     *  - end_thread
     *  - has_command
     *  - command
     *  - processed_command
     */
    std::shared_ptr<std::mutex> mutex = std::make_shared<std::mutex>();

    /**
     * Variable owned by both InputHandler and onInputThread() indicating wherether onInputThread() should exit
     */
    std::shared_ptr<bool> end_thread = std::make_shared<bool>(false);

    /**
     * The data read from std::cin by `inputReadingThread` and returned by getAvailableInput()
     */
    std::shared_ptr<std::string> command = std::make_shared<std::string>();

    /**
     * Is true when a line was read and we are waiting for it to be read by getAvailableInput()
     */
    std::shared_ptr<bool> has_command = std::make_shared<bool>(false);

    /**
     * Needed for inputReadingThread to wait until has_command becomes true.
     */
    std::shared_ptr<std::condition_variable> processed_command = std::make_shared<std::condition_variable>();

    /**
     * Notify waitForInput when input is available
     */
    std::shared_ptr<std::condition_variable> received_input = std::make_shared<std::condition_variable>();

};

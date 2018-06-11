#pragma once

#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <istream>
#include <atomic>

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
    InputHandler(const InputHandler &) = 0;

    /**
     * @brief getAvailableInput gets input which have been read and saved from std::cin
     * @param command - saved input
     * @return true if there was some input
     */
    bool getAvailableInput(std::string &command);

private:
    /**
     * The thread which runs getline on std::cin
     */
    std::thread inputReadingThread;
    /**
     * onInputThread - the background thread loop
     */
    void onInputThread();

    /**
     * This mutex guards access to
     *  - end_thread
     *  - has_command
     *  - command
     *  - processed_command
     */
    std::mutex mutex;

    /**
     * Variable owned by onInputThread() indicating wherether onInputThread() should exit
     */
    bool end_thread;

    /**
     * The data read from std::cin by `inputReadingThread` and returned by getAvailableInput()
     */
    std::string command;

    /**
     * Is true when a line was read and we are waiting for it to be read by getAvailableInput()
     */
    bool has_command = false;

    /**
     * Needed for inputReadingThread to wait until has_command becomes true.
     */
    std::condition_variable processed_command;
};

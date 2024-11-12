#include <boost/asio.hpp>
#include "uart_async_receiver.h"
#include "command_dispatcher.h"
#include "event_loop.h"
#include "logger.h"
#include "constants.h"
#include "protocol.pb.h"

int main()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // Set up the Boost ASIO context that will manage the main event loop
    boost::asio::io_context ioContext;

    // Initialize UART Async Receiver
    UartAsyncReceiver uartReceiver(ioContext, UART_DEVICE_PATH);

    // Initialize Command Dispatcher
    CommandDispatcher dispatcher;

    // Link the UART receiver to the command dispatcher
    uartReceiver.setCommandCallback([&dispatcher](const command::Command &cmd)
                                    {
                                        dispatcher.dispatch(cmd); // Pass the command to the dispatcher for handling
                                    });

    // Initialize Event Loop (timer-based tasks)
    EventLoop eventLoop(ioContext);

    // Set up periodic health check in Event Loop
    eventLoop.addPeriodicTask(HEALTH_CHECK_INTERVAL, []()
                              {
                                  Logger::log("Performing system health check...");
                                  // TODO: Implement health check logic
                              });

    // Start the asynchronous read operation on UART
    uartReceiver.startAsyncRead();

    Logger::log("Starting obc_ai event loop...");

    try
    {
        // Run the event loop; blocks until ioContext.stop() is called
        ioContext.run();
    }
    catch (const std::exception &e)
    {
        Logger::log(std::string("Exception in event loop: ") + e.what());
    }

    // Cleanup before exit
    google::protobuf::ShutdownProtobufLibrary();
    Logger::log("obc_ai shutting down.");

    return 0;
}
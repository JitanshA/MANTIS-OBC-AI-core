#include "uart_init.h"
#include "constants.h"
#include "utils.h"
#include "uart.h"

UARTPort uartInit()
{
    UARTPort uart(OBC_AI_DEV);
    if (!uart.configurePort())
    {
        logMessage("Error: Failed to configure UART port.");
        exit(1); // Exit immediately if configuration fails
    }
    logMessage("UART port configured successfully.");
    return uart; // Return the configured UARTPort on success
}
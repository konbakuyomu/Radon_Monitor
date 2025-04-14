#include "globalConfig.h"

static USART1Driver_t usart1_driver_instance;
MsgBusSystemMessage usart1DriverMessage;

void USART1Driver_Init(void)
{
    USART1Driver_t* drv = &usart1_driver_instance;

    drv->txMessageBuffer = xMessageBufferCreateStatic(
        USART1_MAX_SEND_BUFFER_SIZE, drv->txMessageBufferMemory, &drv->txMessageBufferStorage);

    drv->rxMessageBuffer = xMessageBufferCreateStatic(
        USART1_MAX_RECEIVE_BUFFER_SIZE, drv->rxMessageBufferMemory, &drv->rxMessageBufferStorage);

    drv->txCompleteSemaphore = xSemaphoreCreateBinaryStatic(&drv->txCompleteSemaphoreBuffer);

    drv->testData = 0;

    usart1DriverMessage.message = 0;
    usart1DriverMessage.payload.usartData = pdFALSE;
}

size_t USART1Driver_Send(void)
{
    USART1Driver_t* drv = &usart1_driver_instance;
    size_t bytesSent = 0;

    bytesSent = xMessageBufferReceive(
        drv->txMessageBuffer, drv->sendBuffer, USART1_MAX_SEND_BUFFER_SIZE, portMAX_DELAY);

    if (bytesSent > 0) {
        xSemaphoreTake(drv->txCompleteSemaphore, 0);

        hal_uart_data_t uartData;
        uartData.data = drv->sendBuffer;
        uartData.length = bytesSent;
        uartData.channel = UART_CHANNEL_1;
        HAL_uartSend(&uartData);

        if (xSemaphoreTake(drv->txCompleteSemaphore, pdMS_TO_TICKS(1000)) != pdTRUE) {
            // 超时处理
        }
    }

    memset(drv->sendBuffer, 0, sizeof(drv->sendBuffer));
    return bytesSent;
}

size_t USART1Driver_SendTxDataMessage(const uint8_t* data, size_t size)
{
    USART1Driver_t* drv = &usart1_driver_instance;
    if (data == NULL || size == 0)
        return 0;

    size_t bytesWritten = xMessageBufferSend(drv->txMessageBuffer, data, size, portMAX_DELAY);

    if (bytesWritten == size) {
        usart1DriverMessage.message = MSGBUS_MSG_UART1_TRANSMIT;
        usart1DriverMessage.payload.usartData = 0x02;
        msgbus_publish(&usart1DriverMessage);
    }

    return bytesWritten;
}

UartData_t USART1Driver_Receive(void)
{
    USART1Driver_t* drv = &usart1_driver_instance;
    size_t bytesReceived = 0;

    bytesReceived = xMessageBufferReceive(
        drv->rxMessageBuffer, drv->procBuffer, USART1_MAX_RECEIVE_BUFFER_SIZE, portMAX_DELAY);

    UartData_t uartData;
    if (bytesReceived > 0) {
        uartData.data = drv->procBuffer;
        uartData.size = bytesReceived;
    } else {
        uartData.data = NULL;
        uartData.size = 0;
    }
    return uartData;
}

void USART1Driver_SendRxDataMessage(size_t bytesReceived)
{
    USART1Driver_t* drv = &usart1_driver_instance;
    BaseType_t higherPriorityTaskWoken = pdFALSE;

    if (bytesReceived > 0) {
        xMessageBufferSendFromISR(
            drv->rxMessageBuffer, drv->dmaBuffer, bytesReceived, &higherPriorityTaskWoken);

        memset(drv->dmaBuffer, 0, sizeof(drv->dmaBuffer));

        usart1DriverMessage.message = MSGBUS_MSG_UART1_RECEIVE;
        usart1DriverMessage.payload.usartData = 0x03;
        msgbus_publish_from_isr(&usart1DriverMessage, &higherPriorityTaskWoken);

        portYIELD_FROM_ISR(higherPriorityTaskWoken);
    }
}

uint8_t* USART1_GetDmaBufferPtr_use_c(void) { return usart1_driver_instance.dmaBuffer; }

uint32_t USART1_GetDmaBufferSize_use_c(void) { return USART1_MAX_RECEIVE_BUFFER_SIZE; }

void USART1_ProcessReceivedData_use_c(size_t bytesReceived)
{
    USART1Driver_SendRxDataMessage(bytesReceived);
}

void USART1_NotifyTxComplete_use_c(void)
{
    USART1Driver_t* drv = &usart1_driver_instance;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (drv->txCompleteSemaphore != NULL) {
        xSemaphoreGiveFromISR(drv->txCompleteSemaphore, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void USART1Driver_SaveTestData(uint32_t data)
{
    USART1Driver_t* drv = &usart1_driver_instance;
    drv->testData = data;
}

uint32_t USART1Driver_GetTestData(void)
{
    USART1Driver_t* drv = &usart1_driver_instance;
    return drv->testData;
}

void USART1Driver_SendTestDataMessage(uint32_t data)
{
    USART1Driver_t* drv = &usart1_driver_instance;
    drv->testData = data;
    usart1DriverMessage.message = MSGBUS_MSG_TEST_MESSAGE;
    usart1DriverMessage.payload.testData = data;
    msgbus_publish(&usart1DriverMessage);
}
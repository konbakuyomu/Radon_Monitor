#include "command_processor_c.h"
#include <stdlib.h>
#include <string.h>

/* ------------------ 基类方法实现 ------------------ */

static void registerHandler_impl(CommandProcessorBaseC* self, uint32_t cmdType, CommandHandlerC handler, void* context) {
    for (uint16_t i = 0; i < self->count; ++i) {
        if (self->cmdTypes[i] == cmdType) {
            self->handlers[i] = handler;
            self->contexts[i] = context;
            return;
        }
    }
    if (self->count < self->capacity) {
        self->cmdTypes[self->count] = cmdType;
        self->handlers[self->count] = handler;
        self->contexts[self->count] = context;
        self->count++;
    }
}

static bool executeCommand_impl(CommandProcessorBaseC* self, const void* message) {
    uint32_t cmd = self->getCommandFromMessage(self, message);
    for (uint16_t i = 0; i < self->count; ++i) {
        if (self->cmdTypes[i] == cmd) {
            if (self->handlers[i]) {
                return self->handlers[i](self->contexts[i], message);
            }
        }
    }
    return false;
}

void CommandProcessorBaseC_init(CommandProcessorBaseC* base, uint16_t capacity,
                                uint32_t* cmdTypes, CommandHandlerC* handlers, void** contexts) {
    base->capacity = capacity;
    base->count = 0;
    base->cmdTypes = cmdTypes;
    base->handlers = handlers;
    base->contexts = contexts;
    base->registerHandler = registerHandler_impl;
    base->executeCommand = executeCommand_impl;
    base->getCommandFromMessage = NULL;  // 由派生类赋值
}

/* ------------------ 消息总线命令处理器 ------------------ */

static uint32_t MessageBus_getCommandFromMessage(void* self, const void* message) {
    // 假设SystemMessage结构体第一个字段是uint32_t message
    return *((const uint32_t*)message);
}

static MessageBusProcessorC instance_msgbus;
static uint32_t msgbus_cmdTypes[16];
static CommandHandlerC msgbus_handlers[16];
static void* msgbus_contexts[16];

void MessageBusProcessorC_init(MessageBusProcessorC* proc, uint16_t capacity,
                               uint32_t* cmdTypes, CommandHandlerC* handlers, void** contexts) {
    CommandProcessorBaseC_init(&proc->base, capacity, cmdTypes, handlers, contexts);
    proc->base.getCommandFromMessage = MessageBus_getCommandFromMessage;
}

MessageBusProcessorC* MessageBusProcessorC_getInstance(void) {
    static bool initialized = false;
    if (!initialized) {
        MessageBusProcessorC_init(&instance_msgbus, 16, msgbus_cmdTypes, msgbus_handlers, msgbus_contexts);
        initialized = true;
    }
    return &instance_msgbus;
}

/* ------------------ 串口协议命令处理器 ------------------ */

static uint32_t UartProtocol_getCommandFromMessage(void* self, const void* message) {
    // 假设UartMessage结构体第一个字段是uint32_t command
    return *((const uint32_t*)message);
}

static UartProtocolProcessorC instance_uart;
static uint32_t uart_cmdTypes[16];
static CommandHandlerC uart_handlers[16];
static void* uart_contexts[16];

void UartProtocolProcessorC_init(UartProtocolProcessorC* proc, uint16_t capacity,
                                 uint32_t* cmdTypes, CommandHandlerC* handlers, void** contexts) {
    CommandProcessorBaseC_init(&proc->base, capacity, cmdTypes, handlers, contexts);
    proc->base.getCommandFromMessage = UartProtocol_getCommandFromMessage;
}

UartProtocolProcessorC* UartProtocolProcessorC_getInstance(void) {
    static bool initialized = false;
    if (!initialized) {
        UartProtocolProcessorC_init(&instance_uart, 16, uart_cmdTypes, uart_handlers, uart_contexts);
        initialized = true;
    }
    return &instance_uart;
}
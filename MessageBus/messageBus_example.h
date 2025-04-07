/**
 * @file messageBus_example.h
 * @brief 消息总线使用示例头文件
 * @date 2025-03-16
 */

#ifndef MESSAGE_BUS_EXAMPLE_H
#define MESSAGE_BUS_EXAMPLE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化消息总线示例
 */
void messageBus_example_init(void);

/**
 * @brief 清理消息总线示例资源
 */
void messageBus_example_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif /* MESSAGE_BUS_EXAMPLE_H */ 
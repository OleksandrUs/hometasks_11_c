/*
 * task2, main.c
 * Purpose: create project to demonstrate dynamic memory allocation.
 *
 * @author Oleksandr Ushkarenko
 * @version 1.0 27/09/2021
 */

#include "stm32f3xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "uart_driver.h"

/*
 * These identifiers are used to determine the microcontroller pins
 * the eight color LEDs connected to.
 */
#define BLUE_LED_1		GPIO_PIN_8
#define RED_LED_1 		GPIO_PIN_9
#define ORANGE_LED_1 	GPIO_PIN_10
#define GREEN_LED_1		GPIO_PIN_11
#define BLUE_LED_2 		GPIO_PIN_12
#define RED_LED_2 		GPIO_PIN_13
#define ORANGE_LED_2	GPIO_PIN_14
#define GREEN_LED_2 	GPIO_PIN_15

/*
 * The sizes of the stack (in 4-byte words) for created tasks.
 */
#define TASK_STACK_SIZE 32U

/*
 * The number of LEDs on the board.
 */
#define LEDS_NUM 8

/*
 * Declaration of the function prototypes.
 */
void GPIO_Init(void);
void receive_data_task(void *param);
void led_controller_task(void *param);
void error_handler(void);
void change_led_state(int cmd);

/*
 * The variables are used to store task handles.
 */
TaskHandle_t task1_handle;
TaskHandle_t task2_handle;

/*
 * The array contais pin numbers the LEDs connected to.
 */
uint16_t led_pins[LEDS_NUM] = {BLUE_LED_1, RED_LED_1, ORANGE_LED_1, GREEN_LED_1,
															 BLUE_LED_2, RED_LED_2, ORANGE_LED_2, GREEN_LED_2};

int cmd;

/*
 * The main function of the program (the entry point).
 * Two tasks are created.
 */
int main()
{
	GPIO_Init();
	uart_init();
	uart_open();

	BaseType_t result;
	
	result = xTaskCreate(receive_data_task, "Receive data task", TASK_STACK_SIZE, NULL, 1, &task1_handle);
	if(result != pdPASS){
		error_handler();
	}
	
	result = xTaskCreate(led_controller_task, "LED controller task",  TASK_STACK_SIZE, NULL, 1, &task2_handle);
	if(result != pdPASS){
		error_handler();
	}
	
	vTaskStartScheduler();
	while(1) {}
}

/*
 * The function is used to initialize I/O pins of port E (GPIOE). 
 * All microcontroller pins the LEDs connected to configured to output.
 * The push-pull mode is used, no pull-ups.
 */
void GPIO_Init()
{
	__HAL_RCC_GPIOE_CLK_ENABLE();

	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 
	| GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_RESET);

	GPIO_InitTypeDef gpio_init_struct;
	gpio_init_struct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 |
												 GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
	gpio_init_struct.Pull = GPIO_NOPULL;
	gpio_init_struct.Speed = GPIO_SPEED_LOW;
	
	HAL_GPIO_Init(GPIOE, &gpio_init_struct);
}

/*
 * This is a task function (thread) that reads data received from UART and puts it into the queue.
 * The data is the command: 'a' - 'h' to switch off a LED, 'A' - 'H' to switch on a LED.
 *
 * @param a value that is passed as the parameter to the created task.
 */
void receive_data_task(void * param)
{
	while(1) {
			cmd=uart_read();
			vTaskDelay(pdMS_TO_TICKS(100));
		}
}

/*
 * This is a task function that gets the data from the queue and changes the LEDs state
 * depending on the data.
 *
 * @param a value that is passed as the parameter to the created task.
 */
void led_controller_task(void * param)
{
	while(1) {
		change_led_state(cmd);
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}

/*
 * The function is used as an error handler: if an error occures, this function
 * is invoked and two red LEDs on board will be switched on.
 */
void error_handler(void)
{
	HAL_GPIO_WritePin(GPIOE, RED_LED_1 | RED_LED_2, GPIO_PIN_SET);
	while(1){	}
}

void change_led_state(int cmd)
{
	if(cmd >= 'a' && cmd <= 'h'){
		HAL_GPIO_WritePin(GPIOE, led_pins[cmd-'a'], GPIO_PIN_RESET);
	} else if(cmd >= 'A' && cmd <= 'H'){
			HAL_GPIO_WritePin(GPIOE, led_pins[cmd-'A'], GPIO_PIN_SET);
	}
}

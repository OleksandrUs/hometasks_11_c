/*
 * task1, main.c
 * Purpose: create project to demonstrate static memory allocation.
 * 
 * In the program the use of static memory allocation for tasks is demonstrated. The first task switches on the 
 * LEDs on the board in series. The second task switches off the LEDs. The time delays used
 * in each task are sligthly different and it creates nice visual effect ('variable length caterpillar'). 
 *
 * @author Oleksandr Ushkarenko
 * @version 1.0 25/09/2021
 */

#include "stm32f3xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

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
 * The size of the stack (in 4-byte words) for created static tasks.
 */
#define STACK_SIZE 32U

/*
 * These identifiers are used to determine time delays - pauses between
 * the LEDs state changes (LED switches on in one task and switches off in the other task).
 */
#define LED_ON_DELAY 250U
#define LED_OFF_DELAY 170U

/*
 * The number of LEDs on the board.
 */
#define LEDS_NUM 8

/*
 * Declaration of the function prototypes.
 */
void GPIO_Init(void);
void led_on_controller_task(void *param);
void led_off_controller_task(void *param);
void error_handler(void);

/*
 * These variables are used to index arrays in tasks and create visual effects on LEDs.
 */
uint32_t led_on_index = 0;
uint32_t led_off_index = 0;

/*
 * These arrays are used as the task's stacks.
 */
StackType_t task1_stack[STACK_SIZE];
StackType_t task2_stack[STACK_SIZE];

/*
 * The variables are used to hold the new task's data structures.
 */
StaticTask_t task1_buff;
StaticTask_t task2_buff;

/*
 * The variables are used to store task's handles.
 */
TaskHandle_t task1_handle;
TaskHandle_t task2_handle;

/*
 * The array contais pin numbers the LEDs connected to.
 */
uint16_t led_pins[LEDS_NUM] = {BLUE_LED_1, RED_LED_1, ORANGE_LED_1, GREEN_LED_1,
															 BLUE_LED_2, RED_LED_2, ORANGE_LED_2, GREEN_LED_2};

/*
 * The main function of the program (the entry point).
 * Demonstration of using the static memory allocation for tasks. Two tasks are created.
 * The first task switches on the LEDs on the board in series. The second task switches off the LEDs.
 * The time delays used in each task are sligthly different and it creates nice visual effect.
 */
int main()
{
	GPIO_Init();

	task1_handle = xTaskCreateStatic(led_on_controller_task, "LED ON Controller Task", STACK_SIZE, NULL, 1, task1_stack, &task1_buff);
	if(task1_handle == NULL){
		error_handler();
	}
	
	task2_handle = xTaskCreateStatic(led_off_controller_task, "LED OFF Controller Task", STACK_SIZE, NULL, 1, task2_stack, &task2_buff);
	if(task2_handle == NULL){
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
 * This is a task function (thread) in which the LEDs on the board are switched on in series.
 *
 * @param a value that is passed as the parameter to the created task.
 */
void led_on_controller_task(void *param)
{
	while(1) {
		HAL_GPIO_WritePin(GPIOE, led_pins[led_on_index], GPIO_PIN_SET);
		led_on_index = (led_on_index < (LEDS_NUM - 1)) ? led_on_index + 1 : 0;
		vTaskDelay(pdMS_TO_TICKS(LED_ON_DELAY));
	}
}

/*
 * This is a task function (thread) in which the LEDs on the board are switched off in series.
 *
 * @param a value that is passed as the parameter to the created task.
 */
void led_off_controller_task(void *param)
{
	while(1) {
		HAL_GPIO_WritePin(GPIOE, led_pins[led_off_index], GPIO_PIN_RESET);
		led_off_index = (led_off_index < (LEDS_NUM - 1)) ? led_off_index + 1 : 0;
		vTaskDelay(pdMS_TO_TICKS(LED_OFF_DELAY));
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


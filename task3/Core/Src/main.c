/*
 * task3, main.c
 * Purpose: create function to check task stack overflow (for dynamic and static memory allocation).
 * 
 * In the program two tasks are created. For one task the static memory allocation is used, for the other task
 * the dynamic memory allocation is used. In each task the recursive function for the factorial computation
 * is called. The argument of the factorial() function increases gradually and after that the functions for 
 * stack overflow check are called in each task. If it is detected that that the task stack is filled more than the half,
 * the task is deleted (removed from the RTOS kernels management) and two LEDs are switched on (green LEDs when half stack
 * overflow is detected for task 1, and blue LEDs when half stack overflow is detected for task 2). The stack sizes
 * for each task are different just to demonstrate that the stack overflow (strictly speaking half stack overflow
 * in this example) for both tasks occurs not simultaneously.Green blinking LEDs indicates that the tasks 
 * are in the running mode.
 *
 * @author Oleksandr Ushkarenko
 * @version 1.0 26/09/2021
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
 * The sizes of the stack (in 4-byte words) for created tasks.
 */
#define TASK1_STACK_SIZE 48U
#define TASK2_STACK_SIZE 96U

/*
 * The identifier is used to set LED blinking period.
 */
 #define BLINK_DELAY 200

/*
 * This identifier is used as a unique pattern to find how many elements in a task stack were used.
 */
 #define STACK_PATTERN 0xA5A5A5A5
 
 /*
  * This identifier is used as the maximum number the factorial value will be calculated for.
	* It is used in the stack overflow simulation algorithm.
  */
#define TEST_VALUE 20

/*
 * Declaration of the function prototypes.
 */
void GPIO_Init(void);
void computational_static_task(void *param);
void computational_dynamic_task(void *param);
void error_handler(void);
uint64_t factorial(uint32_t value);
uint32_t check_static_stack_overflow(const StackType_t *pStack, const uint32_t stack_size);
UBaseType_t check_dynamic_stack_overflow(const TaskHandle_t *pTaskHandle);

/*
 * This array is used as the task's stack.
 */
StackType_t task1_stack[TASK1_STACK_SIZE];

/*
 * The variable is used to hold the new task's data structure.
 */
StaticTask_t task1_buff;

/*
 * The variables are used to store task handles.
 */
TaskHandle_t task1_handle;
TaskHandle_t task2_handle;

/*
 * These variables are used to store the calculated values of the factorials.
 * The variables are used only for debugging purposes to see in the Watch window
 * the values when the stack overflow occures.
 */
 uint64_t profiler_1, profiler_2;
 
/*
 * The main function of the program (the entry point).
 * Two tasks are created. For one task the static memory allocation is used, for the other task
 * the dynamic memory allocation is used.
 */
int main()
{
	GPIO_Init();

	task1_handle = xTaskCreateStatic(computational_static_task, "LED ON Controller Task", TASK1_STACK_SIZE, NULL, 1, task1_stack, &task1_buff);
	if(task1_handle == NULL){
		error_handler();
	}
	
	BaseType_t result;
	result = xTaskCreate(computational_dynamic_task, "LED OFF Controller Task",  TASK2_STACK_SIZE, NULL, 1, &task2_handle);
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
 * This is a task function (thread) in which the first green LED blinking and 
 * factorial calculation function is called. The static memory allocation for the task is used.
 * The function to check stack overflow is also called. If more than the half of the stack is used,
 * the task will be deleted and removed from the RTOS kernels management.
 * In this case two orange LESs will be switched on and the first green LED will be switched off.
 *
 * @param a value that is passed as the parameter to the created task.
 */
void computational_static_task(void * param)
{
	uint32_t result;
	while(1) {
	
		for(uint32_t i = 0; i < TEST_VALUE; i++){
		
			profiler_1 = factorial(i);
		
			// The function check_stack_overflow() returns tne number of free elements in a stack.
			result = check_static_stack_overflow(task1_stack,  TASK1_STACK_SIZE);
		
			// The half stack overflow detection condition.
			if(result <  TASK1_STACK_SIZE / 2) {	 	
				HAL_GPIO_WritePin(GPIOE, GREEN_LED_1, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOE, ORANGE_LED_1 | ORANGE_LED_2, GPIO_PIN_SET);
				vTaskDelete(task1_handle);
			}
			
			HAL_GPIO_TogglePin(GPIOE, GREEN_LED_1);
			vTaskDelay(pdMS_TO_TICKS(BLINK_DELAY));
		}
	}
}

/*
 * This is a task function (thread) in which the second green LED blinking and 
 * factorial calculation function is called. The dynamic memory allocation for the task is used.
 * The function to check stack overflow is also called. If more than the half of the stack is used,
 * the task will be deleted and removed from the RTOS kernels management.
 * In this case two blue LESs will be switched on and the second green LED will be switched off.
 *
 * @param a value that is passed as the parameter to the created task.
 */
void computational_dynamic_task(void * param)
{
	UBaseType_t stack_high_water_mark;
	
	while(1) {
	
		for(uint32_t i = 0; i < TEST_VALUE; i++){
			profiler_2 = factorial(i);
		
			stack_high_water_mark = check_dynamic_stack_overflow(&task2_handle);
		
			// The half stack overflow detection condition.
			if(stack_high_water_mark < (TASK2_STACK_SIZE / 2)) {	 	
				HAL_GPIO_WritePin(GPIOE, GREEN_LED_2, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOE, BLUE_LED_1 | BLUE_LED_2, GPIO_PIN_SET);
				vTaskDelete(task2_handle);
			}
			
			HAL_GPIO_TogglePin(GPIOE, GREEN_LED_2);
			vTaskDelay(pdMS_TO_TICKS(BLINK_DELAY));
		}
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

/*
 * The function calculates the factorial value for a given number.
 * This is an recursive function. The return address will be put in stack and
 * the bigger the value, the deeper stack is needed.
 *
 * @param value the number the factorial value will be calculated for
 */
uint64_t factorial(uint32_t value)
{
	if(value > 1) {
		return value * factorial(value-1);
	}
	return 1;
}

/*
 * The function calculates the factorial value for a given number.
 * This is an recursive function. The return address will be put in stack and
 * the bigger the value, the deeper stack is needed.
 *
 * @param pStack the pointer to the buffer (array) that is used as a stack
 * @param stack_size the stack size (maximum number of elements in the stack)
 * @return the number of free elements in the stack (0 means stack overflow) 
 */
uint32_t check_static_stack_overflow(const StackType_t *pStack, const uint32_t stack_size)
{
	for(int i = 0; i < stack_size; i++){
		if(pStack[i] != STACK_PATTERN){
			return i;
		}
	}
	return stack_size - 1;
}

/*
 * This function is a wrapper for uxTaskGetStackHighWaterMark() function.
 * he value returned by the uxTaskGetStackHighWaterMark() function is the high water mark in words.
 * If the return value is zero then the task has likely overflowed its stack. 
 * If the return value is close to zero then the task has come close to overflowing its stack.
 *
 * @param pTaskHandle the pointer to the handle of the task being queried
 */
UBaseType_t check_dynamic_stack_overflow(const TaskHandle_t *pTaskHandle)
{
	return uxTaskGetStackHighWaterMark(*pTaskHandle);
}

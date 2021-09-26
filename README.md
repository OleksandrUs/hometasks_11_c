**Description**

*Home tasks are:*

• Create project to demonstrate static memory allocation.<br>
• Create project to demonstrate dynamic memory allocation.<br>
• Create function to check task stack overflow (for dynamic and static memory allocation).<br>

**Requirements**

Keil uVision v5.35.0.0<br>
STM32CubeMX v6.3.0<br>

**Task 1 notes**

In the program the use of static memory allocation for tasks is demonstrated. The first task switches on the 
LEDs on the board in series. The second task switches off the LEDs. The time delays used
in each task are sligthly different and it creates nice visual effect ('variable length caterpillar').<br>


**Task 1 demonstration**
<br>
![](task1.gif)



**Task 3 notes**

In the program two tasks are created. For one task the static memory allocation is used, for the other task
the dynamic memory allocation is used. In each task the recursive function for the factorial computation
is called. The argument of the factorial() function increases gradually and after that the functions for 
stack overflow are called in each task. If it is detected that that the task stack is filled more than the half,
the task is deleted (removed from the RTOS kernels management) and two LEDs are switched on (green LEDs when half stack
overflow is detected for task 1, and blue LEDs when half stack overflow is detected for task 2). The stack sizes
for each task are different just to demonstrate that the stack overflow (strictly speaking half stack overflow
in this example) for both tasks occurs not simultaneously.<br>

**Task 3 demonstration**
<br>
![](task3.gif)

#include "cy_pdl.h"
#include "cybsp.h"
#include "cycfg.h"
#include "cy_gpio.h"

/* Timer period in milliseconds */
#define TIMER_PERIOD_MSEC   1000U


void timer_interrupt_handler(void) {
	/* Clear the terminal count interrupt */
	Cy_TCPWM_ClearInterrupt(CYBSP_TIMER_HW, CYBSP_TIMER_NUM, CY_TCPWM_INT_ON_TC);
	/* Toggle the LED */
	Cy_GPIO_Inv(P1_2_PORT, P1_2_NUM);
}

void handle_error(void) {
	__disable_irq();
	while (1) {
	}
}

int main(void) {
	cy_rslt_t result;

	result = cybsp_init();
	if (result != CY_RSLT_SUCCESS) {
		CY_ASSERT(0);
	}
	__enable_irq();

	for (int i = 0; i < 10; i++) {
		Cy_GPIO_Write(P1_2_PORT, P1_2_NUM, 1);
		Cy_SysLib_Delay(25);
		Cy_GPIO_Write(P1_2_PORT, P1_2_NUM, 0);
		Cy_SysLib_Delay(50);
	}

	cy_stc_sysint_t intrCfg = {
			.intrSrc = CYBSP_TIMER_IRQ, /* Interrupt source is Timer interrupt */
			.intrPriority = 3UL /* Interrupt priority is 3 */
	};

	result = Cy_SysInt_Init(&intrCfg, timer_interrupt_handler);
	if (result != CY_SYSINT_SUCCESS) {
		CY_ASSERT(0);
	}
	/* Enable Interrupt */
	NVIC_EnableIRQ(intrCfg.intrSrc);


	result = Cy_TCPWM_Counter_Init(CYBSP_TIMER_HW, CYBSP_TIMER_NUM, &CYBSP_TIMER_config);
	if (result != CY_TCPWM_SUCCESS) {
		CY_ASSERT(0);
	}
	Cy_TCPWM_Counter_Enable(CYBSP_TIMER_HW, CYBSP_TIMER_NUM);

	if (0UL != (CY_TCPWM_INT_ON_TC & Cy_TCPWM_GetInterruptMask(CYBSP_TIMER_HW, CYBSP_TIMER_NUM))) {
		Cy_TCPWM_SetInterrupt(CYBSP_TIMER_HW, CYBSP_TIMER_NUM, CY_TCPWM_INT_ON_TC);
	}

	Cy_TCPWM_Counter_SetPeriod(CYBSP_TIMER_HW, CYBSP_TIMER_NUM, TIMER_PERIOD_MSEC - 1);
	Cy_TCPWM_TriggerStart(CYBSP_TIMER_HW, CYBSP_TIMER_MASK);

	for (;;) {
	}
}

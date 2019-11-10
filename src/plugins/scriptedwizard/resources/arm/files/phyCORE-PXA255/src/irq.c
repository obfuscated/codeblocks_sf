/*====================================================================
* Project: Board Support Package (BSP)
* Developed using:
* Function: Handling of IRQ interrupts on Intel XScale PXA255
*
* Copyright HighTec EDV-Systeme GmbH 1982-2006
*====================================================================*/

#include "pxa255regs.h"
#include "irq.h"

#define IRQ_MAX_NUM		ISR_MAX+1		/* first and second level */
#define IRQ_MAX_HW_NUM	ISR_MAX_HW+1	/* first level only */

/* IRQ dispatcher table for ISRs */
static PFV irqVector[IRQ_MAX_NUM];

static void IrqGPIOHandler(void);

/*
 * Priority resolution table
 * Index with a bit mask and you get the highest priority in the mask
 * which is set in the mask.  Least significant bit is highest priority.
 */
static const unsigned char unmapTbl[] =
{
	0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
};


/*---------------------------------------------------------------------
	Function:	IrqInit
	Purpose:	Initialization of IRQ handling
	Arguments:	void
	Return:		void
---------------------------------------------------------------------*/
void IrqInit(void)
{
	int i;

	*ICMR = 0;		/* disable all interrupts */
	*ICLR = 0;		/* all mapped to IRQ */

	/* clear all GPIO edge register */
	*GRER0 = 0;
	*GRER1 = 0;
	*GRER2 = 0;
	*GFER0 = 0;
	*GFER1 = 0;
	*GFER2 = 0;
	/* reset edge flags */
	*GEDR0 = *GEDR0;
	*GEDR1 = *GEDR1;
	*GEDR2 = *GEDR2;

	*ICCR = 1;		/* only enabled IRQs can wake up */

	/* Clear the interrupt vector table */
	for (i = 0; i < IRQ_MAX_NUM; ++i)
	{
		irqVector[i] = (PFV) 0;
	}

	/* Install dispatcher for GPIO[80:2] IRQs */
	irqVector[GPIO_INT_ID] = IrqGPIOHandler;
}

/*---------------------------------------------------------------------
	Function:	IrqEnable
	Purpose:	enable IRQ for an IRQ interrupt
	Arguments:	int irqNum - number of IRQ interrupt
	Return:		void
---------------------------------------------------------------------*/
void IrqEnable(int irqNum)
{
	if ((irqNum < IRQ_MAX_HW_NUM) && (irqNum >= ISR_MIN))
	{
		*ICMR |= (1 << irqNum);
	}
}

/*---------------------------------------------------------------------
	Function:	IrqDisable
	Purpose:	disable IRQ for an IRQ interrupt
	Arguments:	int irqNum - number of IRQ interrupt
	Return:		void
---------------------------------------------------------------------*/
void IrqDisable(int irqNum)
{
	if ((irqNum < IRQ_MAX_HW_NUM) && (irqNum >= ISR_MIN))
	{
		*ICMR &= ~(1 << irqNum);
	}
}

/*---------------------------------------------------------------------
	Function:	IrqInstall
	Purpose:	Install a service handler for an IRQ interrupt
	Arguments:	int irqNum  - number of IRQ interrupt
				PFV isrProc - pointer to service routine
	Return:		PFV         - pointer to old service routine
---------------------------------------------------------------------*/
PFV IrqInstall(int irqNum, PFV isrProc)
{
	PFV oldIsr;

	if (irqNum < ISR_MIN || IRQ_MAX_NUM <= irqNum)
	{
		return (PFV) 0;
	}

	oldIsr = irqVector[irqNum];
	irqVector[irqNum] = isrProc;

	if (irqNum < IRQ_MAX_HW_NUM)
	{
		IrqEnable(irqNum);
	}
	else if (irqNum >= GPIO2_INT_ID)
	{
		IrqEnable(GPIO_INT_ID);
	}

	return oldIsr;
}

/*---------------------------------------------------------------------
	Function:	IrqSetEdge
	Purpose:	Set edge type for GPIO IRQ interrupts
				This must be done before installing handler routine.
	Arguments:	int irqNum  - number of GPIO IRQ interrupt
				int edge    - egde type (falling or rising or both)
	Return:		void
---------------------------------------------------------------------*/
void IrqSetEdge(int irqNum, int edge)
{
	if (	irqNum == GPIO0_INT_ID
		 || irqNum == GPIO1_INT_ID
		 || (irqNum >= GPIO2_INT_ID && irqNum <= GPIO80_INT_ID))
	{
		int index, bit;

		if (irqNum >= GPIO2_INT_ID)
		{
			IrqDisable(GPIO_INT_ID);
			index = irqNum - GPIO2_INT_ID + 2;
		}
		else
		{
			index = irqNum - GPIO0_INT_ID;
		}
		bit = index & 31;

		/* set pin as GPIO input */
		if (index >= 64)
		{
			*GPDR2 &= ~(1 << bit);
			if (index >= 80)
			{
				*GAFR2_U &= ~(3 << (2*(bit-16)));
			}
			else
			{
				*GAFR2_L &= ~(3 << (2*bit));
			}
		}
		else if (index >= 32)
		{
			*GPDR1 &= ~(1 << bit);
			if (index >= 48)
			{
				*GAFR1_U &= ~(3 << (2*(bit-16)));
			}
			else
			{
				*GAFR1_L &= ~(3 << (2*bit));
			}
		}
		else
		{
			*GPDR0 &= ~(1 << bit);
			if (index >= 16)
			{
				*GAFR0_U &= ~(3 << (2*(bit-16)));
			}
			else
			{
				*GAFR0_L &= ~(3 << (2*bit));
			}
		}

		/* reset edge detect status */
		if (index >= 64)
		{
			*GEDR2 = 1 << bit;
		}
		else if (index >= 32)
		{
			*GEDR1 = 1 << bit;
		}
		else
		{
			*GEDR0 = 1 << bit;
		}

		/* set edge(s) */
		if (edge & GPIO_FALLING_EDGE)
		{
			if (index >= 64)
			{
				*GFER2 |= (1 << bit);
			}
			else if (index >= 32)
			{
				*GFER1 |= (1 << bit);
			}
			else
			{
				*GFER0 |= (1 << bit);
			}
		}
		if (edge & GPIO_RISING_EDGE)
		{
			if (index >= 64)
			{
				*GRER2 |= (1 << bit);
			}
			else if (index >= 32)
			{
				*GRER1 |= (1 << bit);
			}
			else
			{
				*GRER0 |= (1 << bit);
			}
		}
	}
}

/*---------------------------------------------------------------------
	Function:	IrqGPIOHandler
	Purpose:	dispatcher for GPIO IRQ interrupts
	Arguments:	void
	Return:		void
---------------------------------------------------------------------*/
static void IrqGPIOHandler(void)
{
	unsigned int i, gedr, spurious;
	PFV  isr;

	while (gedr = (*GEDR0 & ~3), gedr != 0)
	{
		spurious = gedr & ~(*GFER0 | *GRER0);
		if (spurious)
		{
			*GEDR0 = spurious;
			gedr ^= spurious;
			if (!gedr)
			{
				continue;
			}
		}
		for (i = 2; i < 32; ++i)
		{
			if (gedr & (1<<i))
			{
				isr = irqVector[i+32];
				if (isr)
				{
					(*(isr))();
				}
			}
		}
		*GEDR0 = gedr;
	}

	while (gedr = (*GEDR1), gedr != 0)
	{
		spurious = gedr & ~(*GFER1 | *GRER1);
		if (spurious)
		{
			*GEDR1 = spurious;
			gedr ^= spurious;
			if (!gedr)
			{
				continue;
			}
		}
		for (i = 0; i < 32; ++i)
		{
			if (gedr & (1<<i))
			{
				isr = irqVector[i+64];
				if (isr)
				{
					(*(isr))();
				}
			}
		}
		*GEDR1 = gedr;
	}

	while (gedr = (*GEDR2), gedr != 0)
	{
		spurious = gedr & ~(*GFER2 | *GRER2);
		if (spurious)
		{
			*GEDR2 = spurious;
			gedr ^= spurious;
			if (!gedr)
			{
				continue;
			}
		}
		for (i = 0; i < 32; ++i)
		{
			if (gedr & (1<<i))
			{
				isr = irqVector[i+96];
				if (isr)
				{
					(*(isr))();
				}
			}
		}
		*GEDR2 = gedr;
	}
}

/*---------------------------------------------------------------------
	Function:	IrqHandler
	Purpose:	dispatcher for IRQ interrupts
	Arguments:	void
	Return:		void
---------------------------------------------------------------------*/
void IrqHandler(void)
{
	unsigned int rq, rq1;
	PFV  isr;

	rq = *ICIP;		/* read pending IRQs */

	/* splitting in 4 bytes (size of unmapTbl) */
	rq1 = rq & 0xFF;
	if (rq1 != 0)
	{
		isr = irqVector[unmapTbl[rq1]];
		if (isr)
		{
			(*(isr))();
		}
		return;
	}

	rq >>= 8;
	rq1 = rq & 0xFF;
	if (rq1 != 0)
	{
		isr = irqVector[unmapTbl[rq1]+8];
		if (isr)
		{
			(*(isr))();
		}
		return;
	}

	rq >>= 8;
	rq1 = rq & 0xFF;
	if (rq1 != 0)
	{
		isr = irqVector[unmapTbl[rq1]+16];
		if (isr)
		{
			(*(isr))();
		}
		return;
	}

	rq >>= 8;
	if (rq != 0)
	{
		isr = irqVector[unmapTbl[rq]+24];
		if (isr)
		{
			(*(isr))();
		}
		return;
	}
}

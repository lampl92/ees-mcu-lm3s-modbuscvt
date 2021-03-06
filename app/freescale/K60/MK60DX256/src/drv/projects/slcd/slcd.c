/*
 * File:		slcd.c
 * Purpose:		Example showing the TWRPI-SLCD on the TWR-K40X256
 *
 *                      See twrpi_slcd.h for all the possible SLCD functions
 *                        to turn on and off different segments.
 *
 */

#include "common.h"
#include "twrpi_slcd.h"

void time_delay_ms(int count_val);

/********************************************************************/
void main (void)
{
  char ones, tens, hundreds;
  unsigned int counter=0;


  /* Initialize TWRPI-SLCD on TWR-K40X256 board*/
  init_twrpi_lcd();

  tlcd_all_on(); //Turn on all segments
  time_delay_ms(5000); //Delay so can see all segments on

  tlcd_all_off(); //Turn off all segments
  tlcd_fsl_on();  //Turn on FSL segment

  /* Increment counter and display value on SLCD */
  while(1)
  {
    //Parse counter values to get each digit
    ones=counter%10;
    tens=(counter/10)%10;
    hundreds=(counter/100)%10;

    //Update SLCD
    tlcd_set_numeric3(ones);
    tlcd_set_numeric2(tens);
    tlcd_set_numeric1(hundreds);

    //Increment counter and keep below 1000
    counter++;
    counter=counter%1000;

    //Delay for 250ms
    time_delay_ms(250);
  }

}
/********************************************************************/

/*
 * Delay function using the LPTMR module
 */
void time_delay_ms(int count_val)
{
  SIM_SCGC5|=SIM_SCGC5_LPTIMER_MASK; //Turn on clock to LPTMR module
  LPTMR0_CMR = count_val; //Set compare value
  LPTMR0_PSR = LPTMR_PSR_PCS(1)|LPTMR_PSR_PBYP_MASK; //Use 1Khz LPO clock and bypass prescaler
  LPTMR0_CSR |= LPTMR_CSR_TEN_MASK; //Start counting

  while (!(LPTMR0_CSR & LPTMR_CSR_TCF_MASK)) {} //Wait for counter to reach compare value

  LPTMR0_CSR &= ~LPTMR_CSR_TEN_MASK; //Clear Timer Compare Flag
  return;
}
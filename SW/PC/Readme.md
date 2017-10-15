# Using python script

Just run it, either from the terminal or from your Python IDE. 
In the code, change proper COM port before the run.

Use following commands:
  * `g` - read MAX2871 register content
  * `rt` - read MAX2871 ADC set to temperature readout
  * `rv` - read MAX2871 ADC set to VCO voltage readout
  * `e` - activate RF output
  * `d` - deactivate RF output
  * `px` - sets the power of RF output, where {x} can be 1,2,3 or 4.
  * `fx` - selects the filter, where {x} can be 1,2,3 or 4.

  To change frequency settings, use following commands:
   * `N x` - set integer divider where x is between 16 and 65535.
   * `M x` - set fractional modulus where x is between 2 and 4095.
   * `F x` - set fractional divider where x is between 1 and M.
   * `R x` - set reference divider where x is between 1 and 1023.
   * `D x` - set output divider where x is between 1 and 7.

 Frequency can be easily set by two functions
   * `FINT x` - set sets frequency x in integer mode
   * `FFRAC x` - set sets frequency x in fractional mode


 To set the attentuation, use command:
   * `ATT x` - where x is attenuation in dB from 0 to 32 in steps of 0.25 dB. 

 Special spice is function `sweep`, that enables sweep of every parameter (but will work only with F and N):
  * `sweep x a b dt` - x can be N or F, a is lower limit, b is the upper and dt is time resolution in ms.

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
   * `M x` - set fractional modulus where x is between 1 and 4096.
   * `F x` - set fractional divider where x is between 1 and M.
   * `D x` - set output divider where x is between 1 and 7.

 Special spice is function `sweep`, that enables sweep of every parameter:
  * `sweep x a b` - x can be N, F or M, a is lower limit and b is the upper.
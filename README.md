An RS232 sniffer for the STM32F0Discovery Board. 
It breaks out the serial data onto parallel lines so you can view it with
an old logic analyzer easily, like an HP16500b. Or, if the data is infrequent
enough, use some LEDs :)

You'll need to download my template too, if you wish to build it.
Feel free to modify anything. I did it this way because it works for me.

Based on szczys template which can be found at https://github.com/szczys/stm32f0-discovery-basic-template .
This changes the makefile so that the file structure looks like this:

  ../

  stm32f0-discovery-basic-template/ 

  yourProjectHere/  

  ./ 

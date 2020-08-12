### Stage 0:

At this point there's no ROOT involved, we're just going to compile a piece of C++ code and check the outputs.

This code is the well known Common Element Simulation Technique (CEST). We provide a recoil energy and we get back the number of ionisation electrons produced. The code prints out energy and number of electrons for the number of samples specified. The energy of each sample is drawn at random from the specified range, or we can specify the values we want.

We can also calculate the efficiency of cuts, e.g. require the number of electrons to be above a certain threshold and print it out.
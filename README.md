Floorplanner that is based on lp_solver API. 

The solver runs twice trying to reach area that is as close as possible for the area inputted for the soft modules (mostly over-estimated). 

Assumptions:
- The core area is considered to be a square with its side equal to the maximum of both width and height after the solving process.
- The chip is considered to be a square with its side equal to the side of the core area plus twice the maximum height of a pad (if any).
- The core area is surrounded with a fixed width frame from all sides (if any).
- The pad width is always in parallel with the core side, and the length is out of the core side. 
- All the inputted numbers are expected to be integers. 

Limitations:
- In the output, the soft block sizes are not exactly matches the input, mostly it is over-estimated. Still, it is advisible to input area bigger than the expected.


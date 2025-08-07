# (Performance)-Testing

## Usage:
We used various tests for our (brush-team) code. To test the performance of our functions, we decided to use gprof. A makefile which compiles the code and runs the testcase specified is included here. The visualize.py is meant to visualize the output gotten solely from the team-brush functionality, without any dependencies from other teams.

### Testing the output:
1. modify test.c to only run the desired function once (or however many times desired)
2. specify "1" as a parameter when calling the testcase
3. run `make`
4. run `./test`
5. Copy the output
6. run `python visualize.py`
7. Paste the contents
8. Type `END` and press enter

### Unit Testing:
3. run `make unit`

### Testing the performance:
1. modify test.c to run the desired function many times, to get an average runtime
2. specify "0" as a parameter when calling the testcase
3. run `make profile`
4. see report.txt

## Note:
Utilizing the performance test like this shows a high usage of the list_add_tail function! This is because after each new test case call, 1024 knots are instantiated. This is likely different from the day-to-day usage the program will experience, where only a few knots are changed with each lineto or similar! Still, we believe the timing (and especially the calltree) that gprof offers to be very useful for potentially optimizing the program.

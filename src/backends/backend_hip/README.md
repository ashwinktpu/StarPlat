## TODO

### Major stuff

1. Copy kernel for SSSP. Refer SSSP Manual Code impl for comments.  
2. Check other comment in manual code impl
3. Add another flag to indicate if timing stuff have to be generated
4. Remove all HIT_CHECKs
5. Make a proper README, with instructions of usage.
6. Inform limited usage of atomics
7. Add compile and launch guidelines


### Post Completion of Major Stuff

1. Correct the spacing between pointer types and varnames
1. All additionally generated method like isAnEdge (or even initArray?) to be made optional.
1. Re-order the functions into the appropriate implementation files i.e make sure all hip generator functions are in the file hip_gen_functions.cpp
1. Make use of return value of hip function to generate error messages
1. Look into [[nodiscard]]
1. In DSL i.e. generated code, add new lines after eaach multi line block beginings
2. Add documentation for all the functions.
    1. Add styluing and formating options
3. Add necessary assertions statements and exceptions throws
3. Make more readable code. Extract out declarations from big stuff
4. Use CapitlizeFirstLetter method wherever temp has been used
3. Remove the unused functions and merge the appropriate files
2. Remove commented pieces of code
3. Remove unused variables and reduce variable count
4. Add const, and related const qualifiers
4. Remove unnecessary implementation files and merge them into other implementation files
5. Optimize the code 
6. Add a user manual for hip.
7. Generatre documentaion from the docs
8. Refactor the make file and only add the necessary flags and stuff. eg. bin/hip_auxillary.o is not required (I think)
9. Remove the comments
10. Finish all the TODOs mentioned in the files
11. Make functions private
10. Check if all the fucntions in the header file are actually required in the header file
1. Add necessary comments in generation code.
2. Check if pointers can be changed to smart pointers wherever possible.
CC = mpic++
CFLAGS = -std=c++17 -g -Wall

# Source files for container
CONTAINER_SRCS = readFile.cc ../container.cc testForContainer.cpp 
CONTAINER_OBJS = $(CONTAINER_SRCS:.cpp=.o)
CONTAINER_EXEC = testForContainer

# Source files for vector
VECTOR_SRCS = readFile.cc testForVector.cpp
VECTOR_OBJS = $(VECTOR_SRCS:.cpp=.o)
VECTOR_EXEC = testForVector

# Source files for test case generator
TESTCASE_SRCS = testCaseGen.cpp
TESTCASE_OBJS = $(TESTCASE_SRCS:.cpp=.o)
TESTCASE_EXEC = testCaseGen

parallelArr: $(CONTAINER_EXEC) 
serialArr: $(VECTOR_EXEC) 
testCase: $(TESTCASE_EXEC)

$(CONTAINER_EXEC): $(CONTAINER_OBJS)
	$(CC) $(CFLAGS) -o $@ $(CONTAINER_OBJS)

$(VECTOR_EXEC): $(VECTOR_OBJS)
	$(CC) $(CFLAGS) -o $@ $(VECTOR_OBJS)

$(TESTCASE_EXEC): $(TESTCASE_OBJS)
	$(CC) $(CFLAGS) -o $@ $(TESTCASE_OBJS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(CONTAINER_OBJS) $(CONTAINER_EXEC) $(VECTOR_OBJS) $(VECTOR_EXEC) $(TESTCASE_OBJS) $(TESTCASE_EXEC)


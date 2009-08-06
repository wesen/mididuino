COMPILER = g++
CCFLAGS = -g -ansi -Wall -DLINUX
LIBFLAGS = cppunitlite.a
.SUFFIXES: .o .cpp

lib = cppunitlite
test = test

src = src/ExceptionHandler.cpp \
	src/Test.cpp \
    src/Failure.cpp \
	src/TestException.cpp \
	src/TestRegistry.cpp \
	src/TestResult.cpp \
	src/TestResultStdErr.cpp \
	src/Linux/SignalHandler.cpp

test_src = src/Test/main.cpp \
	src/Test/SampleTests.cpp \
	src/Test/TestFixtures.cpp \
	src/Test/TestMacros.cpp \
	src/Test/TestTestCase.cpp \
	src/Test/TestExceptionHandling.cpp \
      
      
objects = $(patsubst %.cpp, %.o, $(src))
test_objects = $(patsubst %.cpp, %.o, $(test_src))

all: $(test)

$(lib): $(objects) 
	ar cr $(lib).a $(objects)
    
	
$(test): $(lib) $(test_objects)
	${COMPILER} -o $(test) $(test_objects) ${LIBFLAGS}
	./$(test)
	
	
clean:
	rm $(objects) $(test_objects) $(test) $(lib).a

%.o : %.cpp
	$(COMPILER) $(CCFLAGS) -c $< -o $(patsubst %.cpp, %.o, $<)


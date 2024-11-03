CXX = g++
CXXFLAGS = -std=c++11 -I include/ -I ./
SRCS = $(shell find . -name '*.cpp' -not -path './include/*' -not -path '*/.ipynb_checkpoints/*')
TARGET = myprogram

$(TARGET):
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)
COMPILER  = g++
CFLAGS    = -g -MMD -MP -Wextra -Wno-format -Wno-sign-compare -Winit-self -Wno-missing-field-initializers -Wno-psabi -std=c++11
ifeq "$(shell getconf LONG_BIT)" "64"
  LDFLAGS = -L/usr/lib64 -L/usr/local/lib
else
  LDFLAGS = -L/usr/local/lib -L/home/pi/installed/boost_1_70_0/stage/lib
endif
LIBS      = -lv4l2 -lpthread -ljpeg -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lboost_system -lboost_thread -lboost_math_c99
INCLUDE   = -I./include 
TARGET    = ./a.out
SRCDIR    = ./src
ifeq "$(strip $(SRCDIR))" ""
  SRCDIR  = .
endif
SOURCES   = $(wildcard $(SRCDIR)/*.cpp)
OBJDIR    = ./obj
ifeq "$(strip $(OBJDIR))" ""
  OBJDIR  = .
endif
OBJECTS   = $(addprefix $(OBJDIR)/, $(notdir $(SOURCES:.cpp=.o)))
DEPENDS   = $(OBJECTS:.o=.d)

$(TARGET): $(OBJECTS)
	$(COMPILER) -o $@ $^ $(LDFLAGS) $(LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	-mkdir -p $(OBJDIR)
	$(COMPILER) $(CFLAGS) $(INCLUDE) $(LIBS) -o $@ -c $<

all: clean $(TARGET)

clean:
	-rm -f $(OBJECTS) $(DEPENDS) $(TARGET)

-include $(DEPENDS)

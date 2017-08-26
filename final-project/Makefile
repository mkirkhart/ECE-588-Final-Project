TARGET_EXEC ?= hand_detection_test

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src
INC_DIRS ?= ./include

SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# This does not work on Xubuntu 12.04, as it produces -I/usr/include/opencv instead of -I/usr/include/opencv2 :(
#CFLAGS = $(shell pkg-config --cflags opencv)
# NOTE: This may need to be changed if opencv was built and installed from source
# By default, the installer copies the headers to /usr/local/include/opencv2 (I believe)
CFLAGS = -I/usr/include/opencv2 -I/usr/local/include/opencv2 -I/usr/include/libfreenect -I/usr/local/include/libfreenect -I/usr/include/libusb-1.0
# enable all standard warnings, build for debug (gdb3 level)
CFLAGS += -Wall -ggdb3
# NOTE: This may not work if opencv was built and installed from source
LIBS = $(shell pkg-config --libs opencv) -L/usr/lib -lfreenect -lpthread
#LIBS = -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy -lopencv_flann  

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP $(CFLAGS)

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LIBS)

# assembly
$(BUILD_DIR)/%.s.o: %.s
	$(MKDIR_P) $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

# c source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# c++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p


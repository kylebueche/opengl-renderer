# Project Data
EXEC_NAME = Main
SRC_DIR = src
BUILD_DIR = build

# Flags, compiler stuff
CFLAGS = -Wall -Wextra -std=c++11 -pedantic -MMD -MP
# Optional: Treat warnings as errors
# CFLAGS += -Werror
LIBS = -lglfw3 -lopengl32 -lgdi32

# Obtain all source, object, and dependency files
SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEPS = $(SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.d)

# Link all object files into one executable
$(EXEC_NAME): $(OBJS)
	g++ -o $@ $(OBJS) $(CFLAGS) $(LIBS)

# Line by line compile all srcs into objs
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	g++ -c $< -o $@ $(CFLAGS) $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/glad/%.cpp
	@mkdir -p $(BUILD_DIR)
	g++ -c $< -o $@ $(CFLAGS) $(LIBS)

# Recompile c files if any included files are updated
-include $(DEPS)

clean:
	rm -rf $(BUILD_DIR) $(EXEC_NAME)
	@mkdir $(BUILD_DIR)

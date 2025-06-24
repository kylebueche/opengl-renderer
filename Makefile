# Project Data
EXEC_NAME = Renderer
SRC_DIR = src
BUILD_DIR = build

# Flags, compiler stuff
CPPFLAGS = -Wall -Wextra -std=c++20 -pedantic -MMD -MP
CFLAGS = -Wall -Wextra -std=c99 -MMD -MP
# Optional: Treat warnings as errors
# CFLAGS += -Werror
LIBS = -lglfw3 -lopengl32 -lgdi32

# Obtain all source, object, and dependency files
SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o) $(BUILD_DIR)/glad.o
DEPS = $(SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.d) $(BUILD_DIR)/glad.d

# Link all object files into one executable
$(EXEC_NAME): $(OBJS)
	g++ -o $@ $(OBJS) $(CPPFLAGS) $(LIBS)

# Line by line compile all srcs into objs
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	g++ -c $< -o $@ $(CPPFLAGS) $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	gcc -c $< -o $@ $(CFLAGS) $(LIBS)

# Recompile c files if any included files are updated
-include $(DEPS)

clean:
	rm -rf $(BUILD_DIR) $(EXEC_NAME) *.exe
	@mkdir $(BUILD_DIR)

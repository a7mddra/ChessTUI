CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -g -Iinclude -MMD -MP

OBJ_DIR  := build
BIN      := Game

SRCS := $(shell find . -type f -name '*.cpp' \
        -not -path "./$(OBJ_DIR)/*" \
        -not -path "./src/engine/binary/*")
OBJS := $(patsubst ./%, $(OBJ_DIR)/%, $(patsubst %.cpp, %.o, $(SRCS)))
DEPS := $(OBJS:.o=.d)

.PHONY: run clean

$(BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(BIN) $(LDLIBS)
	@chmod +x $(BIN)

run: $(BIN)
	@./$(BIN)

$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

clean:
	@rm -rf $(OBJ_DIR) $(BIN)
	@echo "Cleaned."

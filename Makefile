# Define the output directory
BIN_DIR = bin

# Ensure the bin directory exists before compiling
release: | $(BIN_DIR)
	gcc -O2 -o $(BIN_DIR)/scheduler main.c

debug: | $(BIN_DIR)
	gcc -g -o $(BIN_DIR)/debug_main main.c

clean:
	rm -rf $(BIN_DIR)

# Create the bin directory if it doesn't exist
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

CC = gcc
CFLAGS = -Iinclude -Wall
OBJDIR = obj
SRCDIR = src

OBJECTS = $(OBJDIR)/main.o $(OBJDIR)/process.o $(OBJDIR)/scheduler.o $(OBJDIR)/io.o
TARGET = $(OBJDIR)/simulator

all: $(TARGET)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(OBJDIR)/*.o $(TARGET)

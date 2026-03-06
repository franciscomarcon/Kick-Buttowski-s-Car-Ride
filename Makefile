TARGET = project

# --- DETECÇÃO AUTOMÁTICA DE SISTEMA ---
ifeq ($(OS),Windows_NT)
    # ---------------- WINDOWS ----------------
    SYSTEM_NAME := Windows
    TARGET_EXEC = $(TARGET).exe
    CC = gcc
    
    # Caminho padrão da Raylib no Windows
    RAYLIB_PATH ?= C:/raylib/raylib
    
    INCLUDES = -I$(RAYLIB_PATH)/include
    LDFLAGS = -L$(RAYLIB_PATH)/lib
    LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm -static
    
    CLEAN_CMD = del /Q $(TARGET_EXEC) 2>NUL || echo ""
else
    # ---------------- LINUX ----------------
    SYSTEM_NAME := Linux
    TARGET_EXEC = $(TARGET)
    CC = gcc
    
    INCLUDES = 
    LDFLAGS = 
    LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    
    CLEAN_CMD = rm -f $(TARGET_EXEC)
endif

# --- CONFIGURAÇÕES COMUNS ---
CFLAGS = -Wall -std=c99 -Wno-missing-braces -O2

# --- REGRAS ---

all:
	@echo "Detectado sistema: $(SYSTEM_NAME)"
	$(CC) project.c -o $(TARGET_EXEC) $(CFLAGS) $(INCLUDES) $(LDFLAGS) $(LIBS)
	@echo "Compilacao concluida! Executavel: $(TARGET_EXEC)"

clean:
	$(CLEAN_CMD)
	@echo "Limpeza concluida."
CROSS_PREFIX = arm-none-eabi-

CC = $(CROSS_PREFIX)gcc
AS = $(CROSS_PREFIX)as
LD = $(CROSS_PREFIX)ld
OBJCOPY = $(CROSS_PREFIX)objcopy

BUILD_DIR = build
TARGET = kernel.elf

# -nostdlib: Não linke com a biblioteca padrão do C. Nós não temos um SO!
# -ffreestanding: O código não depende de um ambiente padrão.
# -Wall: Habilita todos os warnings. -Wextra: Habilita mais warnings.
# -g: Inclui símbolos de debug para o GDB.
# -Iinclude: Adiciona o diretório 'include' aos caminhos de busca por headers.
CFLAGS = -nostdlib -ffreestanding -Wall -Wextra -g -Iinclude

ARCH_FLAGS = -mcpu=cortex-a15 -mthumb

CFLAGS += $(ARCH_FLAGS)

ASM_SOURCES = $(wildcard arch/arm/boot/*.S)
C_SOURCES = $(wildcard kernel/*.c)

OBJ_FILES = $(patsubst arch/arm/boot/%.S, $(BUILD_DIR)/%.o, $(ASM_SOURCES))
OBJ_FILES += $(patsubst kernel/%.c, $(BUILD_DIR)/%.o, $(C_SOURCES))

LINKER_SCRIPT = arch/arm/linker.ld

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	@echo "LD	$@"
	@$(LD) -T $(LINKER_SCRIPT) -o $@ $(OBJ_FILES)

$(BUILD_DIR)/%.o: kernel/%.c
	@mkdir -p $(@D)
	@echo "CC	$<"
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: arch/arm/boot/%.S
	@mkdir -p $(@D)
	@echo "AS	$<"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "CLEAN"
	@rm -rf $(BUILD_DIR) $(TARGET)

run: all
	@qemu-system-arm -M virt -cpu cortex-a15 -nographic -kernel $(TARGET)

debug: all
	@qemu-system-arm -M virt -cpu cortex-a15 -nographic -kernel $(TARGET) -s -S

.PHONY: all clean run debug

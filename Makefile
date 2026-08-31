CC = arm-none-eabi-gcc
# Додаємо прапорець -I. щоб компілятор шукав .h файли в поточній папці та її підпапках
CFLAGS = -mcpu=cortex-m0plus -mthumb -nostartfiles -T linker.ld -I.

# Створюємо список усіх файлів з кодом. 
# Знак \ дозволяє перенести рядок для краси.
SRCS = $(wildcard ./src/*.c) \
       $(wildcard ./src/drivers/*.c) \
       $(wildcard ./src/*.S) \
	   $(wildcard ./src/core/*.S) \
	   $(wildcard ./src/core/*c)

# Створюємо папку bin, якщо її ще немає
all:
	mkdir -p ./bin
	# Тепер компілятор бере весь список $(SRCS) автоматично!
	$(CC) $(CFLAGS) $(SRCS) -o ./bin/firmware.elf	-lm
	arm-none-eabi-objcopy -O binary ./bin/firmware.elf ./bin/firmware.bin
	picotool uf2 convert ./bin/firmware.elf ./bin/firmware.uf2	--family	rp2040
	@echo "Збірка успішна! Файл firmware.uf2 готовий у папці ./bin/"

flash: all
	@echo "Копіюю на Raspberry Pi Pico..."
	sudo picotool load ./bin/firmware.uf2 -x
	@echo "Прошито!"

clean:
	@echo "Очищення..."
	rm -f ./bin/*.elf ./bin/*.bin ./bin/*.uf2
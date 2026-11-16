CC      = gcc
CFLAGS  = -std=c11 -g -O0 -Wall -Wextra
SRC     = src/contacts.c
STAGE  ?= before

.PHONY: all asan analyze clean

all: contacts

contacts: $(SRC)
	$(CC) $(CFLAGS) -o $@ $(SRC)

# збірка з AddressSanitizer і UndefinedBehaviorSanitizer для динамічного аналізу
asan: contacts-asan

contacts-asan: $(SRC)
	$(CC) $(CFLAGS) -fsanitize=address,undefined -fno-omit-frame-pointer -o $@ $(SRC)

# повний аналіз: make analyze STAGE=before | STAGE=after
analyze: contacts contacts-asan
	bash scripts/analyze.sh $(STAGE)

clean:
	rm -f contacts contacts-asan

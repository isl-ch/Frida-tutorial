CC=gcc
CFLAGS=-O0 -fno-stack-protector -no-pie -rdynamic
SRC=challenges_source
OUT=challenges

all: easy medium hard

easy: $(OUT)/01_easy_return $(OUT)/02_easy_args $(OUT)/03_easy_call
medium: $(OUT)/04_medium_rpg $(OUT)/05_medium_struct $(OUT)/06_medium_memscan
hard: $(OUT)/07_hard_antidebug $(OUT)/08_hard_integrity $(OUT)/09_hard_stalker

$(OUT)/01_easy_return: $(SRC)/01_easy_return.c
	$(CC) $(CFLAGS) -o $@ $^

$(OUT)/02_easy_args: $(SRC)/02_easy_args.c
	$(CC) $(CFLAGS) -o $@ $^

$(OUT)/03_easy_call: $(SRC)/03_easy_call.c
	$(CC) $(CFLAGS) -o $@ $^

$(OUT)/04_medium_rpg: $(SRC)/04_medium_rpg.c
	$(CC) $(CFLAGS) -o $@ $^

$(OUT)/05_medium_struct: $(SRC)/05_medium_struct.c
	$(CC) $(CFLAGS) -o $@ $^

$(OUT)/06_medium_memscan: $(SRC)/06_medium_memscan.c
	$(CC) $(CFLAGS) -o $@ $^

$(OUT)/07_hard_antidebug: $(SRC)/07_hard_antidebug.c
	$(CC) $(CFLAGS) -o $@ $^

$(OUT)/08_hard_integrity: $(SRC)/08_hard_integrity.c
	$(CC) $(CFLAGS) -o $@ $^

$(OUT)/09_hard_stalker: $(SRC)/09_hard_stalker.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(OUT)/01_easy_return $(OUT)/02_easy_args $(OUT)/03_easy_call \
	       $(OUT)/04_medium_rpg $(OUT)/05_medium_struct $(OUT)/06_medium_memscan \
	       $(OUT)/07_hard_antidebug $(OUT)/08_hard_integrity $(OUT)/09_hard_stalker

CC=gcc
CFLAGS=-O0 -fno-stack-protector -no-pie

all: easy medium hard

easy: 01_easy_return 02_easy_args 03_easy_call
medium: 04_medium_rpg 05_medium_struct 06_medium_memscan
hard: 07_hard_antidebug 08_hard_integrity 09_hard_stalker

01_easy_return: 01_easy_return.c
	$(CC) $(CFLAGS) -o $@ $^

02_easy_args: 02_easy_args.c
	$(CC) $(CFLAGS) -o $@ $^

03_easy_call: 03_easy_call.c
	$(CC) $(CFLAGS) -o $@ $^

04_medium_rpg: 04_medium_rpg.c
	$(CC) $(CFLAGS) -o $@ $^

05_medium_struct: 05_medium_struct.c
	$(CC) $(CFLAGS) -o $@ $^

06_medium_memscan: 06_medium_memscan.c
	$(CC) $(CFLAGS) -o $@ $^

07_hard_antidebug: 07_hard_antidebug.c
	$(CC) $(CFLAGS) -o $@ $^

08_hard_integrity: 08_hard_integrity.c
	$(CC) $(CFLAGS) -o $@ $^

09_hard_stalker: 09_hard_stalker.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f 01_easy_return 02_easy_args 03_easy_call 04_medium_rpg 05_medium_struct 06_medium_memscan 07_hard_antidebug 08_hard_integrity 09_hard_stalker

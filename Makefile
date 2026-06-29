CC=gcc
CFLAGS=-O0 -fno-stack-protector -no-pie -rdynamic
SRC=challenges_source
OUT=challenges

# JNI Include Paths
JNI_INC=-I/usr/lib/jvm/java-21-openjdk-amd64/include -I/usr/lib/jvm/java-21-openjdk-amd64/include/linux

all: easy medium hard advanced

easy: $(OUT)/01_easy_return $(OUT)/02_easy_args $(OUT)/03_easy_call
medium: $(OUT)/04_medium_rpg $(OUT)/05_medium_struct $(OUT)/06_medium_memscan
hard: $(OUT)/07_hard_antidebug $(OUT)/08_hard_integrity $(OUT)/09_hard_stalker
advanced: $(OUT)/Challenge13_JavaCrypto.class $(OUT)/Challenge15_JNIBridge.class $(OUT)/libnative_challenge.so $(OUT)/16_advanced_root_detect

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

$(OUT)/Challenge13_JavaCrypto.class: $(SRC)/Challenge13_JavaCrypto.java
	javac $< -d $(OUT)

$(OUT)/Challenge15_JNIBridge.class: $(SRC)/15_jni_bridge/Challenge15_JNIBridge.java
	javac $< -d $(OUT)

$(OUT)/libnative_challenge.so: $(SRC)/15_jni_bridge/native_lib.c
	$(CC) -shared -fPIC $(JNI_INC) -o $@ $^

$(OUT)/16_advanced_root_detect: $(SRC)/16_advanced_root_detect.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(OUT)/01_easy_return $(OUT)/02_easy_args $(OUT)/03_easy_call \
	       $(OUT)/04_medium_rpg $(OUT)/05_medium_struct $(OUT)/06_medium_memscan \
	       $(OUT)/07_hard_antidebug $(OUT)/08_hard_integrity $(OUT)/09_hard_stalker \
	       $(OUT)/Challenge13_JavaCrypto.class $(OUT)/Challenge15_JNIBridge.class \
	       $(OUT)/libnative_challenge.so $(OUT)/16_advanced_root_detect


#include <jni.h>
#include <string.h>
#include <stdio.h>

// Obfuscated key: "jni_secure_password_2026"
// XORed with 90 (0x5A)
static const unsigned char obfuscated_pass[] = {
    48, 52, 51, 5, 41, 63, 57, 47, 40, 63, 5, 42, 59, 41, 41, 45, 53, 40, 62, 5, 104, 106, 104, 108, 0
};

static int verify_internal(const char* input) {
    char decrypted[25];
    for (int i = 0; i < 24; i++) {
        decrypted[i] = obfuscated_pass[i] ^ 90;
    }
    decrypted[24] = '\0';
    return strcmp(input, decrypted) == 0;
}

JNIEXPORT jboolean JNICALL Java_Challenge15_1JNIBridge_verifyKey(JNIEnv *env, jobject obj, jstring key) {
    if (key == NULL) return JNI_FALSE;
    
    const char *nativeKey = (*env)->GetStringUTFChars(env, key, NULL);
    if (nativeKey == NULL) return JNI_FALSE;
    
    int result = verify_internal(nativeKey);
    
    (*env)->ReleaseStringUTFChars(env, key, nativeKey);
    
    return result ? JNI_TRUE : JNI_FALSE;
}

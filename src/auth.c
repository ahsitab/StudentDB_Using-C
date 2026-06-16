#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "auth.h"
#include "activity_log.h"
#include "utils.h"

int isLoggedIn = 0;
static const char ENCRYPT_KEY = 0x5A; // XOR key for securing credentials in file

static void encryptDecrypt(char* data, size_t length) {
    for (size_t i = 0; i < length; i++) {
        data[i] ^= ENCRYPT_KEY;
    }
}

int initAdminAccount(void) {
    FILE* file = fopen(ADMIN_FILE, "rb");
    if (file != NULL) {
        fclose(file);
        return 1; // Credentials file already initialized
    }
    
    // Create default administrator credentials
    AdminCredentials admin;
    strcpy(admin.username, "admin");
    
    char defaultPass[MAX_PASSWORD_LEN] = "password123";
    encryptDecrypt(defaultPass, strlen(defaultPass));
    strcpy(admin.password, defaultPass);
    
    file = fopen(ADMIN_FILE, "wb");
    if (file == NULL) {
        return 0; // Error opening credentials file for write
    }
    
    size_t written = fwrite(&admin, sizeof(AdminCredentials), 1, file);
    fclose(file);
    
    if (written == 1) {
        logActivity("Default Admin Account Initialized");
        return 1;
    }
    return 0;
}

int authenticateAdmin(const char* username, const char* password) {
    FILE* file = fopen(ADMIN_FILE, "rb");
    if (file == NULL) {
        return 0;
    }
    
    AdminCredentials storedAdmin;
    size_t read = fread(&storedAdmin, sizeof(AdminCredentials), 1, file);
    fclose(file);
    
    if (read != 1) {
        return 0;
    }
    
    // Decrypt the stored password to compare
    char decryptedPass[MAX_PASSWORD_LEN];
    strcpy(decryptedPass, storedAdmin.password);
    encryptDecrypt(decryptedPass, strlen(decryptedPass));
    
    if (strcmp(username, storedAdmin.username) == 0 && strcmp(password, decryptedPass) == 0) {
        isLoggedIn = 1;
        logActivity("Admin Logged In Successfully");
        return 1;
    }
    
    return 0;
}

int changeAdminPassword(const char* oldPass, const char* newPass) {
    FILE* file = fopen(ADMIN_FILE, "rb");
    if (file == NULL) {
        return 0;
    }
    
    AdminCredentials storedAdmin;
    size_t read = fread(&storedAdmin, sizeof(AdminCredentials), 1, file);
    fclose(file);
    
    if (read != 1) {
        return 0;
    }
    
    // Decrypt stored password
    char decryptedPass[MAX_PASSWORD_LEN];
    strcpy(decryptedPass, storedAdmin.password);
    encryptDecrypt(decryptedPass, strlen(decryptedPass));
    
    if (strcmp(oldPass, decryptedPass) != 0) {
        return 0; // Current password input incorrect
    }
    
    // Encrypt the new password and save it
    char encryptedNewPass[MAX_PASSWORD_LEN];
    strcpy(encryptedNewPass, newPass);
    encryptDecrypt(encryptedNewPass, strlen(encryptedNewPass));
    
    strcpy(storedAdmin.password, encryptedNewPass);
    
    file = fopen(ADMIN_FILE, "wb");
    if (file == NULL) {
        return 0;
    }
    
    size_t written = fwrite(&storedAdmin, sizeof(AdminCredentials), 1, file);
    fclose(file);
    
    if (written == 1) {
        logActivity("Admin Password Changed Successfully");
        return 1;
    }
    return 0;
}

void logoutAdmin(void) {
    isLoggedIn = 0;
    logActivity("Admin Logged Out");
}

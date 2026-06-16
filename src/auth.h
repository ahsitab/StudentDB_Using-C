#ifndef AUTH_H
#define AUTH_H

#define ADMIN_FILE "data/admin.bin"
#define MAX_USERNAME_LEN 50
#define MAX_PASSWORD_LEN 50

typedef struct {
    char username[MAX_USERNAME_LEN];
    char password[MAX_PASSWORD_LEN];
} AdminCredentials;

// External variable indicating if admin session is active
extern int isLoggedIn;

int initAdminAccount(void);
int authenticateAdmin(const char* username, const char* password);
int changeAdminPassword(const char* oldPass, const char* newPass);
void logoutAdmin(void);

#endif // AUTH_H

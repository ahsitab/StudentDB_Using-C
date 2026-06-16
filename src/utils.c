#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "utils.h"

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <unistd.h>
#endif

void setConsoleColor(int colorCode) {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, colorCode);
#else
    const char* ansi = "\033[0m";
    switch(colorCode) {
        case COLOR_PRIMARY: ansi = "\033[1;36m"; break;
        case COLOR_SUCCESS: ansi = "\033[1;32m"; break;
        case COLOR_WARNING: ansi = "\033[1;33m"; break;
        case COLOR_DANGER:  ansi = "\033[1;31m"; break;
        case COLOR_INFO:    ansi = "\033[1;34m"; break;
        case COLOR_HEADER:  ansi = "\033[1;35m"; break;
        case COLOR_WHITE:   ansi = "\033[1;37m"; break;
        default:            ansi = "\033[0m"; break;
    }
    printf("%s", ansi);
#endif
}

void resetConsoleColor(void) {
    setConsoleColor(COLOR_RESET);
}

void printColor(int colorCode, const char* format, ...) {
    setConsoleColor(colorCode);
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    resetConsoleColor();
}

void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void clearScreen(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void trimNewline(char* str) {
    size_t len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[len - 1] = '\0';
        len--;
    }
}

int isValidEmail(const char* email) {
    if (email == NULL || strlen(email) < 5) return 0;
    
    const char* at = strchr(email, '@');
    if (at == NULL || at == email || strchr(at + 1, '@') != NULL) {
        return 0; // Must have exactly one '@' and it cannot be the first character
    }
    
    const char* dot = strrchr(at, '.');
    if (dot == NULL || dot == at + 1 || strlen(dot) < 3) {
        return 0; // Must have at least one '.' after '@', not immediately adjacent, and with a domain suffix of at least 2 chars
    }
    
    // Spaces are strictly invalid in email addresses
    for (int i = 0; email[i] != '\0'; i++) {
        if (isspace((unsigned char)email[i])) return 0;
    }
    return 1;
}

int isValidPhone(const char* phone) {
    if (phone == NULL || strlen(phone) < 7 || strlen(phone) > 18) return 0;
    
    int digitCount = 0;
    for (int i = 0; phone[i] != '\0'; i++) {
        if (i == 0 && phone[i] == '+') continue;
        if (isdigit((unsigned char)phone[i])) {
            digitCount++;
        } else if (phone[i] != '-' && phone[i] != ' ' && phone[i] != '(' && phone[i] != ')') {
            return 0; // Contains non-phone character
        }
    }
    return (digitCount >= 7 && digitCount <= 15);
}

int isValidName(const char* name) {
    if (name == NULL || strlen(name) < 2) return 0;
    
    for (int i = 0; name[i] != '\0'; i++) {
        if (!isalpha((unsigned char)name[i]) && name[i] != ' ' && name[i] != '.' && name[i] != '-') {
            return 0;
        }
    }
    
    // Check it doesn't consist of only spaces
    int hasLetter = 0;
    for (int i = 0; name[i] != '\0'; i++) {
        if (isalpha((unsigned char)name[i])) {
            hasLetter = 1;
            break;
        }
    }
    return hasLetter;
}

int isValidID(const char* id) {
    if (id == NULL || strlen(id) < 1) return 0;
    
    for (int i = 0; id[i] != '\0'; i++) {
        if (!isalnum((unsigned char)id[i]) && id[i] != '-' && id[i] != '_') {
            return 0;
        }
    }
    return 1;
}

int getValidatedInt(const char* prompt, int minVal, int maxVal) {
    char input[100];
    int value;
    char extra;
    while (1) {
        printf("%s", prompt);
        if (fgets(input, sizeof(input), stdin) == NULL) {
            continue;
        }
        trimNewline(input);
        if (strlen(input) == 0) {
            printColor(COLOR_DANGER, "Error: Input cannot be empty.\n");
            continue;
        }
        if (sscanf(input, "%d%c", &value, &extra) != 1) {
            printColor(COLOR_DANGER, "Error: Invalid integer input.\n");
            continue;
        }
        if (value < minVal || value > maxVal) {
            printColor(COLOR_DANGER, "Error: Out of range (%d to %d).\n", minVal, maxVal);
            continue;
        }
        return value;
    }
}

float getValidatedFloat(const char* prompt, float minVal, float maxVal) {
    char input[100];
    float value;
    char extra;
    while (1) {
        printf("%s", prompt);
        if (fgets(input, sizeof(input), stdin) == NULL) {
            continue;
        }
        trimNewline(input);
        if (strlen(input) == 0) {
            printColor(COLOR_DANGER, "Error: Input cannot be empty.\n");
            continue;
        }
        if (sscanf(input, "%f%c", &value, &extra) != 1) {
            printColor(COLOR_DANGER, "Error: Invalid decimal number input.\n");
            continue;
        }
        if (value < minVal || value > maxVal) {
            printColor(COLOR_DANGER, "Error: Out of range (%.2f to %.2f).\n", minVal, maxVal);
            continue;
        }
        return value;
    }
}

void getValidatedString(const char* prompt, char* dest, int maxLength, int allowSpaces) {
    char* buffer = malloc(maxLength + 2);
    if (buffer == NULL) {
        printColor(COLOR_DANGER, "Fatal: Memory allocation error.\n");
        exit(1);
    }
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, maxLength + 2, stdin) == NULL) {
            continue;
        }
        trimNewline(buffer);
        
        size_t len = strlen(buffer);
        if (len > (size_t)maxLength) {
            printColor(COLOR_DANGER, "Error: Input exceeds maximum length of %d characters.\n", maxLength);
            clearInputBuffer();
            continue;
        }
        
        if (len == 0) {
            printColor(COLOR_DANGER, "Error: Input cannot be empty.\n");
            continue;
        }
        
        if (!allowSpaces) {
            int hasSpace = 0;
            for (size_t i = 0; i < len; i++) {
                if (isspace((unsigned char)buffer[i])) {
                    hasSpace = 1;
                    break;
                }
            }
            if (hasSpace) {
                printColor(COLOR_DANGER, "Error: Spaces are not allowed in this field.\n");
                continue;
            }
            strcpy(dest, buffer);
        } else {
            char* start = buffer;
            while (*start && isspace((unsigned char)*start)) {
                start++;
            }
            size_t startLen = strlen(start);
            while (startLen > 0 && isspace((unsigned char)start[startLen - 1])) {
                start[startLen - 1] = '\0';
                startLen--;
            }
            if (strlen(start) == 0) {
                printColor(COLOR_DANGER, "Error: Input cannot consist of only spaces.\n");
                continue;
            }
            strcpy(dest, start);
        }
        
        free(buffer);
        return;
    }
}

void getPasswordInput(const char* prompt, char* password, int maxLength) {
    printf("%s", prompt);
    int i = 0;
    char ch;
    while (1) {
#ifdef _WIN32
        ch = _getch();
#else
        ch = getchar();
#endif
        if (ch == '\r' || ch == '\n') {
            password[i] = '\0';
            printf("\n");
            break;
        } else if (ch == '\b') {
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else if (ch == 3) { // Ctrl+C
            password[0] = '\0';
            printf("\n");
            break;
        } else {
            if (i < maxLength) {
                password[i++] = ch;
                printf("*");
            }
        }
    }
}

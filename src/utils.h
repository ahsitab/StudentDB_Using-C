#ifndef UTILS_H
#define UTILS_H

// Console color definitions (Windows console attribute codes)
#define COLOR_RESET   7
#define COLOR_PRIMARY 11  // Cyan
#define COLOR_SUCCESS 10  // Green
#define COLOR_WARNING 14  // Yellow
#define COLOR_DANGER  12  // Red
#define COLOR_INFO    9   // Light Blue
#define COLOR_HEADER  13  // Purple/Magenta
#define COLOR_WHITE   15  // Bright White

// Color utilities
void setConsoleColor(int colorCode);
void resetConsoleColor(void);
void printColor(int colorCode, const char* format, ...);

// Input validation and cleanup utilities
void clearInputBuffer(void);
void clearScreen(void);
void trimNewline(char* str);

// Data format validators
int isValidEmail(const char* email);
int isValidPhone(const char* phone);
int isValidName(const char* name);
int isValidID(const char* id);

// Robust user input getters
int getValidatedInt(const char* prompt, int minVal, int maxVal);
float getValidatedFloat(const char* prompt, float minVal, float maxVal);
void getValidatedString(const char* prompt, char* dest, int maxLength, int allowSpaces);
void getPasswordInput(const char* prompt, char* password, int maxLength);

#endif // UTILS_H

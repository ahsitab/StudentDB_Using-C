#include <stdio.h>
#include <time.h>
#include "activity_log.h"
#include "utils.h"

void logActivity(const char* message) {
    FILE* file = fopen(LOG_FILE, "a");
    if (file == NULL) {
        return; 
    }
    
    time_t rawtime;
    struct tm* timeinfo;
    char timeStr[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    if (timeinfo != NULL) {
        strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", timeinfo);
        fprintf(file, "[%s] %s\n", timeStr, message);
    } else {
        fprintf(file, "[UNKNOWN TIME] %s\n", message);
    }
    
    fclose(file);
}

void viewActivityLog(void) {
    FILE* file = fopen(LOG_FILE, "r");
    if (file == NULL) {
        printColor(COLOR_WARNING, "No activity log found yet.\n");
        return;
    }
    
    char line[256];
    printColor(COLOR_HEADER, "\n==================== SYSTEM ACTIVITY LOG ====================\n");
    int count = 0;
    while (fgets(line, sizeof(line), file) != NULL) {
        printf("%s", line);
        count++;
    }
    if (count == 0) {
        printColor(COLOR_INFO, "Log file is empty.\n");
    }
    printColor(COLOR_HEADER, "=============================================================\n");
    fclose(file);
}

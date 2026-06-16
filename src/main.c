#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "student.h"
#include "auth.h"
#include "file_ops.h"
#include "activity_log.h"
#include "utils.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

// Platform independent directory creation helper
static void makeDataDirectory(void) {
#ifdef _WIN32
    CreateDirectory("data", NULL);
#else
    mkdir("data", 0777);
#endif
}

// Sub-menu controllers
static void handleLogin(void);
static void handleChangePassword(void);
static void handleBackupRestore(StudentDatabase* db);

int main(void) {
    // Initial setup
    makeDataDirectory();
    
    if (!initAdminAccount()) {
        printColor(COLOR_DANGER, "Fatal: Failed to initialize credentials file. Exiting...\n");
        return 1;
    }
    
    StudentDatabase db;
    initDatabase(&db);
    
    if (!loadDatabase(&db)) {
        printColor(COLOR_DANGER, "Fatal: Failed to load student records from disk. Exiting...\n");
        freeDatabase(&db);
        return 1;
    }
    
    int running = 1;
    while (running) {
        if (!isLoggedIn) {
            // Logged-out Menu
            printColor(COLOR_PRIMARY, "\n==================================================\n");
            printColor(COLOR_WHITE,   "            STUDENT MANAGEMENT SYSTEM             \n");
            printColor(COLOR_PRIMARY, "==================================================\n");
            printf("  1. Login (Administrator)\n");
            printf("  2. GPA Calculator (Guest Utility)\n");
            printf("  3. Exit Application\n");
            printColor(COLOR_PRIMARY, "==================================================\n");
            
            int choice = getValidatedInt("Choose an option (1-3): ", 1, 3);
            switch (choice) {
                case 1:
                    handleLogin();
                    break;
                case 2:
                    standaloneGPACalculator();
                    break;
                case 3:
                    running = 0;
                    break;
            }
        } else {
            // Logged-in Admin Menu
            printColor(COLOR_PRIMARY, "\n==================================================\n");
            printColor(COLOR_WHITE,   "         STUDENT MANAGEMENT SYSTEM (Admin)        \n");
            printColor(COLOR_PRIMARY, "==================================================\n");
            printf("  1. Add Student Record\n");
            printf("  2. View All Student Records\n");
            printf("  3. Search Student\n");
            printf("  4. Update Student Details\n");
            printf("  5. Delete Student Record\n");
            printf("  6. Standalone GPA Calculator\n");
            printf("  7. Statistics Dashboard\n");
            printf("  8. Export Student Data to CSV\n");
            printf("  9. Sort Records (by GPA or Alphabetical)\n");
            printf("  10. Backup & Restore Database\n");
            printf("  11. Change Administrator Password\n");
            printf("  12. View System Activity Logs\n");
            printf("  13. Logout Session\n");
            printf("  14. Exit Application\n");
            printColor(COLOR_PRIMARY, "==================================================\n");
            
            int choice = getValidatedInt("Choose an option (1-14): ", 1, 14);
            switch (choice) {
                case 1:
                    addStudent(&db);
                    break;
                case 2:
                    viewStudents(&db);
                    break;
                case 3:
                    searchStudent(&db);
                    break;
                case 4:
                    updateStudent(&db);
                    break;
                case 5:
                    deleteStudent(&db);
                    break;
                case 6:
                    standaloneGPACalculator();
                    break;
                case 7:
                    displayStatistics(&db);
                    break;
                case 8:
                    if (exportToCSV(&db)) {
                        printColor(COLOR_SUCCESS, "\nData exported to 'data/students_export.csv' successfully!\n\n");
                    } else {
                        printColor(COLOR_DANGER, "\nFailed to export records to CSV.\n\n");
                    }
                    break;
                case 9:
                    sortStudents(&db);
                    break;
                case 10:
                    handleBackupRestore(&db);
                    break;
                case 11:
                    handleChangePassword();
                    break;
                case 12:
                    viewActivityLog();
                    break;
                case 13:
                    logoutAdmin();
                    printColor(COLOR_SUCCESS, "\nLogged out successfully.\n\n");
                    break;
                case 14:
                    running = 0;
                    break;
            }
        }
    }
    
    // Clean up memory before program termination
    freeDatabase(&db);
    printColor(COLOR_SUCCESS, "\nThank you for using the Student Management System. Goodbye!\n");
    return 0;
}

static void handleLogin(void) {
    char username[MAX_USERNAME_LEN];
    char password[MAX_PASSWORD_LEN];
    int attempts = 0;
    
    while (attempts < 3) {
        printColor(COLOR_HEADER, "\n--- Admin Login Attempt ---\n");
        getValidatedString("Enter Username: ", username, MAX_USERNAME_LEN - 1, 0);
        getPasswordInput("Enter Password: ", password, MAX_PASSWORD_LEN - 1);
        
        if (authenticateAdmin(username, password)) {
            printColor(COLOR_SUCCESS, "\nLogin successful! Session initialized.\n\n");
            return;
        } else {
            attempts++;
            printColor(COLOR_DANGER, "\nError: Invalid credentials. Attempt %d of 3.\n", attempts);
        }
    }
    
    printColor(COLOR_DANGER, "\nMaximum login attempts exceeded. Access denied.\n\n");
    logActivity("Admin Login Blocked: 3 Failed Attempts");
}

static void handleChangePassword(void) {
    char oldPass[MAX_PASSWORD_LEN];
    char newPass[MAX_PASSWORD_LEN];
    char confirmPass[MAX_PASSWORD_LEN];
    
    printColor(COLOR_HEADER, "\n--- Change Password Security Settings ---\n");
    getPasswordInput("Enter Current Password: ", oldPass, MAX_PASSWORD_LEN - 1);
    getPasswordInput("Enter New Password: ", newPass, MAX_PASSWORD_LEN - 1);
    getPasswordInput("Confirm New Password: ", confirmPass, MAX_PASSWORD_LEN - 1);
    
    if (strcmp(newPass, confirmPass) != 0) {
        printColor(COLOR_DANGER, "\nError: New passwords do not match.\n\n");
        return;
    }
    
    if (strlen(newPass) < 6) {
        printColor(COLOR_DANGER, "\nError: Password must be at least 6 characters.\n\n");
        return;
    }
    
    if (changeAdminPassword(oldPass, newPass)) {
        printColor(COLOR_SUCCESS, "\nPassword updated successfully in secure database!\n\n");
    } else {
        printColor(COLOR_DANGER, "\nError: Invalid current password credentials. Reset aborted.\n\n");
    }
}

static void handleBackupRestore(StudentDatabase* db) {
    printColor(COLOR_HEADER, "\n--- Database Maintenance Options ---\n");
    printf("  1. Create Local Database Backup\n");
    printf("  2. Restore Database from Last Backup\n");
    printf("  3. Cancel Operation\n");
    
    int choice = getValidatedInt("Choose an action (1-3): ", 1, 3);
    if (choice == 1) {
        if (backupDatabase()) {
            printColor(COLOR_SUCCESS, "\nBackup file generated successfully at 'data/students_backup.bin'.\n\n");
        } else {
            printColor(COLOR_DANGER, "\nError: Backup process failed. Ensure students database exists.\n\n");
        }
    } else if (choice == 2) {
        char confirm[10];
        getValidatedString("Are you sure? This will overwrite the current database. (Y/N): ", confirm, 9, 0);
        if (confirm[0] == 'Y' || confirm[0] == 'y') {
            if (restoreDatabase(db)) {
                printColor(COLOR_SUCCESS, "\nDatabase restore completed successfully! Data reloaded.\n\n");
            } else {
                printColor(COLOR_DANGER, "\nError: Database restore failed. Ensure backup file exists.\n\n");
            }
        }
    }
}

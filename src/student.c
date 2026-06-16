#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "student.h"
#include "utils.h"
#include "activity_log.h"
#include "file_ops.h"

// Define global subject names
const char* DEFAULT_SUBJECTS[NUM_SUBJECTS] = {
    "Mathematics",
    "Physics",
    "Chemistry",
    "Computer Science",
    "English"
};

// Internal utility to find index by ID
static int findStudentIndexByID(const StudentDatabase* db, const char* id) {
    for (int i = 0; i < db->count; i++) {
        if (strcmp(db->data[i].id, id) == 0) {
            return i;
        }
    }
    return -1;
}

// Display single student record detail
static void printStudentCard(const Student* s) {
    printColor(COLOR_HEADER, "\n==================================================\n");
    printColor(COLOR_PRIMARY, "             STUDENT RECORD DETAILS               \n");
    printColor(COLOR_HEADER, "==================================================\n");
    printf("  ID           : %s\n", s->id);
    printf("  Name         : %s\n", s->name);
    printf("  Department   : %s\n", s->department);
    printf("  Semester     : %d\n", s->semester);
    printf("  Email        : %s\n", s->email);
    printf("  Phone        : %s\n", s->phone);
    printColor(COLOR_HEADER, "  ------------------------------------------------\n");
    printColor(COLOR_INFO, "  Academic Records:\n");
    for (int i = 0; i < NUM_SUBJECTS; i++) {
        printf("    %-18s: %3d  (Grade: %-2s, GP: %.2f)\n", 
               s->subjects[i].name, 
               s->subjects[i].marks, 
               s->subjects[i].grade, 
               s->subjects[i].gradePoint);
    }
    printColor(COLOR_HEADER, "  ------------------------------------------------\n");
    printColor(COLOR_SUCCESS, "  Semester GPA : %.2f\n", s->gpa);
    printColor(COLOR_SUCCESS, "  Cumulative CGPA : %.2f\n", s->cgpa);
    printColor(COLOR_HEADER, "==================================================\n\n");
}

void initDatabase(StudentDatabase* db) {
    db->data = NULL;
    db->count = 0;
    db->capacity = 0;
}

void freeDatabase(StudentDatabase* db) {
    if (db->data != NULL) {
        free(db->data);
        db->data = NULL;
    }
    db->count = 0;
    db->capacity = 0;
}

void getGradeAndPoint(int marks, char* grade, float* point) {
    if (marks >= 90 && marks <= 100) {
        strcpy(grade, "A+");
        *point = 4.00f;
    } else if (marks >= 85) {
        strcpy(grade, "A");
        *point = 3.75f;
    } else if (marks >= 80) {
        strcpy(grade, "A-");
        *point = 3.50f;
    } else if (marks >= 75) {
        strcpy(grade, "B+");
        *point = 3.25f;
    } else if (marks >= 70) {
        strcpy(grade, "B");
        *point = 3.00f;
    } else if (marks >= 65) {
        strcpy(grade, "B-");
        *point = 2.75f;
    } else if (marks >= 60) {
        strcpy(grade, "C+");
        *point = 2.50f;
    } else if (marks >= 55) {
        strcpy(grade, "C");
        *point = 2.25f;
    } else if (marks >= 50) {
        strcpy(grade, "D");
        *point = 2.00f;
    } else {
        strcpy(grade, "F");
        *point = 0.00f;
    }
}

int addStudent(StudentDatabase* db) {
    char id[20];
    printColor(COLOR_HEADER, "\n--- Add New Student Record ---\n");
    while (1) {
        getValidatedString("Enter Student ID: ", id, 19, 0);
        if (!isValidID(id)) {
            printColor(COLOR_DANGER, "Error: Invalid Student ID pattern. Alphanumeric/hyphens/underscores only.\n");
            continue;
        }
        if (findStudentIndexByID(db, id) != -1) {
            printColor(COLOR_DANGER, "Error: Student ID '%s' already exists in database.\n", id);
            continue;
        }
        break;
    }
    
    Student s;
    strcpy(s.id, id);
    
    while (1) {
        getValidatedString("Enter Full Name: ", s.name, MAX_NAME_LEN - 1, 1);
        if (isValidName(s.name)) {
            break;
        } else {
            printColor(COLOR_DANGER, "Error: Invalid character in name. Alphabetic, spaces, hyphens and dots only.\n");
        }
    }
    
    getValidatedString("Enter Department: ", s.department, MAX_DEPT_LEN - 1, 1);
    s.semester = getValidatedInt("Enter Semester (1-8): ", 1, 8);
    
    while (1) {
        getValidatedString("Enter Email Address: ", s.email, MAX_EMAIL_LEN - 1, 0);
        if (isValidEmail(s.email)) {
            break;
        } else {
            printColor(COLOR_DANGER, "Error: Invalid email format (must contain '@' and a domain name suffix).\n");
        }
    }
    
    while (1) {
        getValidatedString("Enter Phone Number: ", s.phone, MAX_PHONE_LEN - 1, 0);
        if (isValidPhone(s.phone)) {
            break;
        } else {
            printColor(COLOR_DANGER, "Error: Invalid phone number. Must contain 7-15 digits.\n");
        }
    }
    
    float totalGP = 0.0f;
    printColor(COLOR_INFO, "\nEnter academic marks for the 5 core courses:\n");
    for (int i = 0; i < NUM_SUBJECTS; i++) {
        char prompt[100];
        sprintf(prompt, "  Marks for %s (0-100): ", DEFAULT_SUBJECTS[i]);
        s.subjects[i].marks = getValidatedInt(prompt, 0, 100);
        strcpy(s.subjects[i].name, DEFAULT_SUBJECTS[i]);
        getGradeAndPoint(s.subjects[i].marks, s.subjects[i].grade, &s.subjects[i].gradePoint);
        totalGP += s.subjects[i].gradePoint;
    }
    s.gpa = totalGP / NUM_SUBJECTS;
    printColor(COLOR_SUCCESS, "  Semester GPA calculated: %.2f\n", s.gpa);
    
    char promptCGPA[100];
    sprintf(promptCGPA, "Enter Cumulative CGPA (0.00-4.00) [Suggested %.2f]: ", s.gpa);
    s.cgpa = getValidatedFloat(promptCGPA, 0.00f, 4.00f);
    
    // Append to dynamic array
    if (db->count >= db->capacity) {
        int newCapacity = db->capacity == 0 ? 10 : db->capacity * 2;
        Student* newData = realloc(db->data, newCapacity * sizeof(Student));
        if (newData == NULL) {
            printColor(COLOR_DANGER, "Fatal: Out of memory. Could not add student record.\n");
            return 0;
        }
        db->data = newData;
        db->capacity = newCapacity;
    }
    
    db->data[db->count] = s;
    db->count++;
    
    // Save database persistently
    if (saveDatabase(db)) {
        char logMsg[150];
        sprintf(logMsg, "Added student record for ID: %s", s.id);
        logActivity(logMsg);
        printColor(COLOR_SUCCESS, "\nStudent record added and saved successfully!\n\n");
        return 1;
    } else {
        printColor(COLOR_WARNING, "\nRecord added in session, but file save failed.\n\n");
        return 1;
    }
}

void viewStudents(const StudentDatabase* db) {
    if (db->count == 0) {
        printColor(COLOR_WARNING, "\nNo student records found in database.\n\n");
        return;
    }
    
    printColor(COLOR_HEADER, "\n+------------------+------------------------------+------------+------+------+------+\n");
    printColor(COLOR_HEADER, "| Student ID       | Full Name                    | Dept       | Sem  | GPA  | CGPA |\n");
    printColor(COLOR_HEADER, "+------------------+------------------------------+------------+------+------+------+\n");
    for (int i = 0; i < db->count; i++) {
        Student* s = &db->data[i];
        printf("| %-16s | %-28s | %-10s | %4d | %4.2f | %4.2f |\n",
               s->id, s->name, s->department, s->semester, s->gpa, s->cgpa);
    }
    printColor(COLOR_HEADER, "+------------------+------------------------------+------------+------+------+------+\n");
    printColor(COLOR_SUCCESS, "Total Students: %d\n\n", db->count);
}

void searchStudent(const StudentDatabase* db) {
    if (db->count == 0) {
        printColor(COLOR_WARNING, "\nNo student records in database to search.\n\n");
        return;
    }
    
    printf("\nSearch Student By:\n");
    printf("1. Student ID\n");
    printf("2. Student Name\n");
    printf("3. Department\n");
    int choice = getValidatedInt("Enter choice (1-3): ", 1, 3);
    
    char query[100];
    if (choice == 1) {
        getValidatedString("Enter Student ID: ", query, 19, 0);
        int idx = findStudentIndexByID(db, query);
        if (idx != -1) {
            printStudentCard(&db->data[idx]);
        } else {
            printColor(COLOR_DANGER, "\nStudent with ID '%s' not found.\n\n", query);
        }
    } else if (choice == 2) {
        getValidatedString("Enter Student Name (substring search): ", query, 99, 1);
        int found = 0;
        char lowerQuery[100];
        strcpy(lowerQuery, query);
        for (int j = 0; lowerQuery[j]; j++) lowerQuery[j] = tolower((unsigned char)lowerQuery[j]);
        
        for (int i = 0; i < db->count; i++) {
            char lowerName[MAX_NAME_LEN];
            strcpy(lowerName, db->data[i].name);
            for (int j = 0; lowerName[j]; j++) lowerName[j] = tolower((unsigned char)lowerName[j]);
            
            if (strstr(lowerName, lowerQuery) != NULL) {
                printStudentCard(&db->data[i]);
                found++;
            }
        }
        if (found == 0) {
            printColor(COLOR_DANGER, "\nNo student found with name matching '%s'.\n\n", query);
        } else {
            printColor(COLOR_SUCCESS, "Found %d matching student(s).\n\n", found);
        }
    } else {
        getValidatedString("Enter Department to query: ", query, 49, 1);
        int found = 0;
        char lowerQuery[100];
        strcpy(lowerQuery, query);
        for (int j = 0; lowerQuery[j]; j++) lowerQuery[j] = tolower((unsigned char)lowerQuery[j]);
        
        for (int i = 0; i < db->count; i++) {
            char lowerDept[MAX_DEPT_LEN];
            strcpy(lowerDept, db->data[i].department);
            for (int j = 0; lowerDept[j]; j++) lowerDept[j] = tolower((unsigned char)lowerDept[j]);
            
            if (strstr(lowerDept, lowerQuery) != NULL) {
                printStudentCard(&db->data[i]);
                found++;
            }
        }
        if (found == 0) {
            printColor(COLOR_DANGER, "\nNo student found in department '%s'.\n\n", query);
        } else {
            printColor(COLOR_SUCCESS, "Found %d matching student(s) in department '%s'.\n\n", found, query);
        }
    }
}

void updateStudent(StudentDatabase* db) {
    if (db->count == 0) {
        printColor(COLOR_WARNING, "\nNo student records in database to update.\n\n");
        return;
    }
    
    char searchID[20];
    getValidatedString("Enter Student ID to update: ", searchID, 19, 0);
    
    int idx = findStudentIndexByID(db, searchID);
    if (idx == -1) {
        printColor(COLOR_DANGER, "\nStudent with ID '%s' not found.\n\n", searchID);
        return;
    }
    
    Student* s = &db->data[idx];
    printColor(COLOR_INFO, "\nCurrent Student Record:\n");
    printStudentCard(s);
    
    printf("Select section to update:\n");
    printf("1. Personal Details (Name, Email, Phone)\n");
    printf("2. Academic Details (Department, Semester)\n");
    printf("3. Subject Marks & CGPA\n");
    printf("4. Update Everything\n");
    printf("5. Cancel\n");
    int choice = getValidatedInt("Enter choice (1-5): ", 1, 5);
    
    if (choice == 5) {
        printColor(COLOR_INFO, "Update cancelled.\n\n");
        return;
    }
    
    int changed = 0;
    char tempStr[100];
    
    if (choice == 1 || choice == 4) {
        while (1) {
            getValidatedString("Enter New Name: ", tempStr, MAX_NAME_LEN - 1, 1);
            if (isValidName(tempStr)) {
                strcpy(s->name, tempStr);
                break;
            } else {
                printColor(COLOR_DANGER, "Error: Invalid name format. Letters, spaces, hyphens and dots only.\n");
            }
        }
        while (1) {
            getValidatedString("Enter New Email: ", tempStr, MAX_EMAIL_LEN - 1, 0);
            if (isValidEmail(tempStr)) {
                strcpy(s->email, tempStr);
                break;
            } else {
                printColor(COLOR_DANGER, "Error: Invalid email format (must contain '@' and domain extension).\n");
            }
        }
        while (1) {
            getValidatedString("Enter New Phone: ", tempStr, MAX_PHONE_LEN - 1, 0);
            if (isValidPhone(tempStr)) {
                strcpy(s->phone, tempStr);
                break;
            } else {
                printColor(COLOR_DANGER, "Error: Invalid phone format (7-15 digits).\n");
            }
        }
        changed = 1;
    }
    
    if (choice == 2 || choice == 4) {
        getValidatedString("Enter New Department: ", s->department, MAX_DEPT_LEN - 1, 1);
        s->semester = getValidatedInt("Enter New Semester (1-8): ", 1, 8);
        changed = 1;
    }
    
    if (choice == 3 || choice == 4) {
        printColor(COLOR_INFO, "Updating marks for subjects:\n");
        float totalGP = 0.0f;
        for (int i = 0; i < NUM_SUBJECTS; i++) {
            char prompt[100];
            sprintf(prompt, "  Marks for %s (0-100): ", s->subjects[i].name);
            s->subjects[i].marks = getValidatedInt(prompt, 0, 100);
            getGradeAndPoint(s->subjects[i].marks, s->subjects[i].grade, &s->subjects[i].gradePoint);
            totalGP += s->subjects[i].gradePoint;
        }
        s->gpa = totalGP / NUM_SUBJECTS;
        printColor(COLOR_SUCCESS, "  Updated GPA: %.2f\n", s->gpa);
        
        char promptCGPA[100];
        sprintf(promptCGPA, "  Enter Cumulative CGPA (0.00-4.00) [Suggested %.2f]: ", s->gpa);
        s->cgpa = getValidatedFloat(promptCGPA, 0.00f, 4.00f);
        changed = 1;
    }
    
    if (changed) {
        if (saveDatabase(db)) {
            char logMsg[150];
            sprintf(logMsg, "Updated student record for ID: %s", s->id);
            logActivity(logMsg);
            printColor(COLOR_SUCCESS, "\nStudent record updated and database file saved successfully!\n\n");
        } else {
            printColor(COLOR_WARNING, "\nRecord updated in memory but file save failed.\n\n");
        }
    }
}

int deleteStudent(StudentDatabase* db) {
    if (db->count == 0) {
        printColor(COLOR_WARNING, "\nNo student records in database to delete.\n\n");
        return 0;
    }
    
    char searchID[20];
    getValidatedString("Enter Student ID to delete: ", searchID, 19, 0);
    
    int idx = findStudentIndexByID(db, searchID);
    if (idx == -1) {
        printColor(COLOR_DANGER, "\nStudent with ID '%s' not found.\n\n", searchID);
        return 0;
    }
    
    Student* s = &db->data[idx];
    printColor(COLOR_WARNING, "\nFound Student Record:\n");
    printStudentCard(s);
    
    char confirm[10];
    getValidatedString("Are you sure you want to permanently delete this student? (Y/N): ", confirm, 9, 0);
    
    if (confirm[0] == 'Y' || confirm[0] == 'y') {
        char deletedID[20];
        strcpy(deletedID, s->id);
        
        // Remove item by shifting memory
        for (int i = idx; i < db->count - 1; i++) {
            db->data[i] = db->data[i + 1];
        }
        db->count--;
        
        if (saveDatabase(db)) {
            char logMsg[150];
            sprintf(logMsg, "Deleted student record for ID: %s", deletedID);
            logActivity(logMsg);
            printColor(COLOR_SUCCESS, "\nStudent record deleted and database saved successfully!\n\n");
            return 1;
        } else {
            printColor(COLOR_DANGER, "\nRecord deleted in memory but database file write failed.\n\n");
            return 1;
        }
    } else {
        printColor(COLOR_INFO, "Deletion cancelled.\n\n");
        return 0;
    }
}

static int compareByGPA(const void* a, const void* b) {
    float gpaA = ((Student*)a)->gpa;
    float gpaB = ((Student*)b)->gpa;
    if (gpaA < gpaB) return 1;    // Descending order
    if (gpaA > gpaB) return -1;
    return 0;
}

static int compareByName(const void* a, const void* b) {
    return strcmp(((Student*)a)->name, ((Student*)b)->name); // Ascending order (alphabetical)
}

void sortStudents(StudentDatabase* db) {
    if (db->count == 0) {
        printColor(COLOR_WARNING, "\nNo student records to sort.\n\n");
        return;
    }
    
    printf("\nSort Student Records By:\n");
    printf("1. GPA (Highest to Lowest)\n");
    printf("2. Alphabetically (A to Z)\n");
    printf("3. Cancel\n");
    int choice = getValidatedInt("Enter choice (1-3): ", 1, 3);
    
    if (choice == 3) {
        return;
    }
    
    if (choice == 1) {
        qsort(db->data, db->count, sizeof(Student), compareByGPA);
        printColor(COLOR_SUCCESS, "\nDatabase sorted by GPA.\n");
        logActivity("Sorted database by GPA");
    } else {
        qsort(db->data, db->count, sizeof(Student), compareByName);
        printColor(COLOR_SUCCESS, "\nDatabase sorted alphabetically by name.\n");
        logActivity("Sorted database alphabetically");
    }
    
    // Auto save the sorted records to file
    saveDatabase(db);
    
    // View sorted results
    viewStudents(db);
}

void displayStatistics(const StudentDatabase* db) {
    if (db->count == 0) {
        printColor(COLOR_WARNING, "\nNo student records in database to run statistics.\n\n");
        return;
    }
    
    float maxGPA = db->data[0].gpa;
    float minGPA = db->data[0].gpa;
    float sumGPA = 0.0f;
    
    #define MAX_UNIQUE_DEPTS 100
    char depts[MAX_UNIQUE_DEPTS][MAX_DEPT_LEN];
    int deptCounts[MAX_UNIQUE_DEPTS] = {0};
    int numDepts = 0;
    
    for (int i = 0; i < db->count; i++) {
        Student* s = &db->data[i];
        
        if (s->gpa > maxGPA) maxGPA = s->gpa;
        if (s->gpa < minGPA) minGPA = s->gpa;
        sumGPA += s->gpa;
        
        int deptIdx = -1;
        for (int d = 0; d < numDepts; d++) {
            if (strcmp(depts[d], s->department) == 0) {
                deptIdx = d;
                break;
            }
        }
        
        if (deptIdx != -1) {
            deptCounts[deptIdx]++;
        } else {
            if (numDepts < MAX_UNIQUE_DEPTS) {
                strcpy(depts[numDepts], s->department);
                deptCounts[numDepts] = 1;
                numDepts++;
            }
        }
    }
    
    float avgGPA = sumGPA / db->count;
    
    printColor(COLOR_HEADER, "\n==================================================\n");
    printColor(COLOR_PRIMARY, "               STATISTICS DASHBOARD               \n");
    printColor(COLOR_HEADER, "==================================================\n");
    printf("  Total Registered Students  : %d\n", db->count);
    printf("  Highest Semester GPA       : %.2f\n", maxGPA);
    printf("  Lowest Semester GPA        : %.2f\n", minGPA);
    printf("  Average Semester GPA       : %.2f\n", avgGPA);
    printColor(COLOR_HEADER, "  ------------------------------------------------\n");
    printColor(COLOR_INFO, "  Department Enrollments:\n");
    for (int d = 0; d < numDepts; d++) {
        printf("    %-24s: %d\n", depts[d], deptCounts[d]);
    }
    printColor(COLOR_HEADER, "==================================================\n\n");
}

void standaloneGPACalculator(void) {
    printColor(COLOR_HEADER, "\n==================================================\n");
    printColor(COLOR_PRIMARY, "            STANDALONE GPA CALCULATOR             \n");
    printColor(COLOR_HEADER, "==================================================\n");
    printf("Enter grades dynamically to calculate GPA and grade metrics.\n\n");
    
    float totalGP = 0.0f;
    for (int i = 0; i < NUM_SUBJECTS; i++) {
        char prompt[100];
        sprintf(prompt, "  Enter marks for Subject %d (0-100): ", i + 1);
        int marks = getValidatedInt(prompt, 0, 100);
        char grade[5];
        float gp;
        getGradeAndPoint(marks, grade, &gp);
        printf("    Course Grade: %s | Grade Point: %.2f\n", grade, gp);
        totalGP += gp;
    }
    
    float finalGPA = totalGP / NUM_SUBJECTS;
    printColor(COLOR_SUCCESS, "\nCalculated GPA: %.2f\n", finalGPA);
    
    if (finalGPA >= 3.75) {
        printColor(COLOR_PRIMARY, "Honors Classification: First Class / Excellent\n");
    } else if (finalGPA >= 3.00) {
        printColor(COLOR_INFO, "Honors Classification: Second Class Upper / Good\n");
    } else if (finalGPA >= 2.00) {
        printColor(COLOR_WARNING, "Honors Classification: Second Class Lower / Satisfactory\n");
    } else {
        printColor(COLOR_DANGER, "Honors Classification: Fail / Academic Warning\n");
    }
    printColor(COLOR_HEADER, "==================================================\n\n");
}

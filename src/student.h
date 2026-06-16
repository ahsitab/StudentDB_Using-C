#ifndef STUDENT_H
#define STUDENT_H

#define MAX_NAME_LEN 100
#define MAX_DEPT_LEN 50
#define MAX_EMAIL_LEN 100
#define MAX_PHONE_LEN 20
#define NUM_SUBJECTS 5

typedef struct {
    char name[50];
    int marks;
    char grade[5];
    float gradePoint;
} Subject;

typedef struct {
    char id[20];
    char name[MAX_NAME_LEN];
    char department[MAX_DEPT_LEN];
    int semester;
    char email[MAX_EMAIL_LEN];
    char phone[MAX_PHONE_LEN];
    Subject subjects[NUM_SUBJECTS];
    float gpa;
    float cgpa;
} Student;

typedef struct {
    Student* data;
    int count;
    int capacity;
} StudentDatabase;

// Global list of default subject names
extern const char* DEFAULT_SUBJECTS[NUM_SUBJECTS];

// Database Lifecycle
void initDatabase(StudentDatabase* db);
void freeDatabase(StudentDatabase* db);

// Grade calculator utility
void getGradeAndPoint(int marks, char* grade, float* point);

// Core student operations
int addStudent(StudentDatabase* db);
void viewStudents(const StudentDatabase* db);
void searchStudent(const StudentDatabase* db);
void updateStudent(StudentDatabase* db);
int deleteStudent(StudentDatabase* db);

// Sorting operations
void sortStudents(StudentDatabase* db);

// Statistics Dashboard
void displayStatistics(const StudentDatabase* db);

// Standalone GPA calculator
void standaloneGPACalculator(void);

#endif // STUDENT_H

#ifndef FILE_OPS_H
#define FILE_OPS_H

#include "student.h"

#define DB_FILE "data/students.bin"
#define BACKUP_FILE "data/students_backup.bin"
#define CSV_FILE "data/students_export.csv"

int loadDatabase(StudentDatabase* db);
int saveDatabase(const StudentDatabase* db);
int exportToCSV(const StudentDatabase* db);
int backupDatabase(void);
int restoreDatabase(StudentDatabase* db);

#endif // FILE_OPS_H

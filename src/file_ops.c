#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_ops.h"
#include "activity_log.h"
#include "utils.h"

int loadDatabase(StudentDatabase* db) {
    FILE* file = fopen(DB_FILE, "rb");
    if (file == NULL) {
        // File not existing yet is normal on first run
        db->count = 0;
        return 1;
    }
    
    db->count = 0;
    Student temp;
    while (fread(&temp, sizeof(Student), 1, file) == 1) {
        if (db->count >= db->capacity) {
            int newCapacity = db->capacity == 0 ? 10 : db->capacity * 2;
            Student* newData = realloc(db->data, newCapacity * sizeof(Student));
            if (newData == NULL) {
                fclose(file);
                logActivity("Database Load Failed: Out of Memory");
                return 0;
            }
            db->data = newData;
            db->capacity = newCapacity;
        }
        db->data[db->count] = temp;
        db->count++;
    }
    
    fclose(file);
    logActivity("Database Loaded Successfully");
    return 1;
}

int saveDatabase(const StudentDatabase* db) {
    FILE* file = fopen(DB_FILE, "wb");
    if (file == NULL) {
        logActivity("Database Save Failed: Write Error");
        return 0;
    }
    
    if (db->count > 0) {
        size_t written = fwrite(db->data, sizeof(Student), db->count, file);
        if (written != (size_t)db->count) {
            fclose(file);
            logActivity("Database Save Failed: Disk Full/Write Interrupted");
            return 0;
        }
    }
    
    fclose(file);
    logActivity("Database Saved Successfully");
    return 1;
}

int exportToCSV(const StudentDatabase* db) {
    FILE* file = fopen(CSV_FILE, "w");
    if (file == NULL) {
        logActivity("CSV Export Failed: Write Error");
        return 0;
    }
    
    // Write headers
    fprintf(file, "Student ID,Full Name,Department,Semester,Email,Phone,");
    for (int i = 0; i < NUM_SUBJECTS; i++) {
        fprintf(file, "%s Marks,%s Grade,%s GP,", DEFAULT_SUBJECTS[i], DEFAULT_SUBJECTS[i], DEFAULT_SUBJECTS[i]);
    }
    fprintf(file, "GPA,CGPA\n");
    
    // Write student records
    for (int i = 0; i < db->count; i++) {
        Student* s = &db->data[i];
        fprintf(file, "%s,\"%s\",\"%s\",%d,%s,%s,", 
                s->id, s->name, s->department, s->semester, s->email, s->phone);
        for (int j = 0; j < NUM_SUBJECTS; j++) {
            fprintf(file, "%d,%s,%.2f,", 
                    s->subjects[j].marks, s->subjects[j].grade, s->subjects[j].gradePoint);
        }
        fprintf(file, "%.2f,%.2f\n", s->gpa, s->cgpa);
    }
    
    fclose(file);
    logActivity("Database Exported to CSV Successfully");
    return 1;
}

int backupDatabase(void) {
    FILE* src = fopen(DB_FILE, "rb");
    if (src == NULL) {
        // Source DB doesn't exist, cannot backup
        logActivity("Database Backup Failed: Source File Missing");
        return 0;
    }
    
    FILE* dest = fopen(BACKUP_FILE, "wb");
    if (dest == NULL) {
        fclose(src);
        logActivity("Database Backup Failed: Dest File Write Error");
        return 0;
    }
    
    char buffer[4096];
    size_t bytesRead;
    int success = 1;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        if (fwrite(buffer, 1, bytesRead, dest) != bytesRead) {
            success = 0;
            break;
        }
    }
    
    fclose(src);
    fclose(dest);
    
    if (success) {
        logActivity("Database Backup Created Successfully");
        return 1;
    } else {
        logActivity("Database Backup Failed: Write Interrupted");
        return 0;
    }
}

int restoreDatabase(StudentDatabase* db) {
    FILE* src = fopen(BACKUP_FILE, "rb");
    if (src == NULL) {
        logActivity("Database Restore Failed: Backup File Missing");
        return 0;
    }
    
    FILE* dest = fopen(DB_FILE, "wb");
    if (dest == NULL) {
        fclose(src);
        logActivity("Database Restore Failed: Destination Write Error");
        return 0;
    }
    
    char buffer[4096];
    size_t bytesRead;
    int success = 1;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        if (fwrite(buffer, 1, bytesRead, dest) != bytesRead) {
            success = 0;
            break;
        }
    }
    
    fclose(src);
    fclose(dest);
    
    if (!success) {
        logActivity("Database Restore Failed: Write Interrupted");
        return 0;
    }
    
    // Reload database into memory
    int reloaded = loadDatabase(db);
    if (reloaded) {
        logActivity("Database Restored from Backup Successfully");
        return 1;
    }
    
    return 0;
}

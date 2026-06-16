/**
 * Core Database Services using localStorage persistence.
 */

import { DEFAULT_SUBJECTS, getGradeAndPoint } from './utils.js';

// Pre-populated sample records (matching python generator)
const SAMPLE_STUDENTS = [
    {
        id: "S101",
        name: "John Doe",
        department: "CSE",
        semester: 3,
        email: "john.doe@email.com",
        phone: "+1234567890",
        subjects: [
            { name: "Mathematics", marks: 95, grade: "A+", gradePoint: 4.00 },
            { name: "Physics", marks: 88, grade: "A", gradePoint: 3.75 },
            { name: "Chemistry", marks: 82, grade: "A-", gradePoint: 3.50 },
            { name: "Computer Science", marks: 92, grade: "A+", gradePoint: 4.00 },
            { name: "English", marks: 76, grade: "B+", gradePoint: 3.25 }
        ],
        gpa: 3.70,
        cgpa: 3.82
    },
    {
        id: "S102",
        name: "Alice Smith",
        department: "EEE",
        semester: 1,
        email: "alice.smith@email.edu",
        phone: "+1987654321",
        subjects: [
            { name: "Mathematics", marks: 72, grade: "B", gradePoint: 3.00 },
            { name: "Physics", marks: 68, grade: "B-", gradePoint: 2.75 },
            { name: "Chemistry", marks: 80, grade: "A-", gradePoint: 3.50 },
            { name: "Computer Science", marks: 85, grade: "A", gradePoint: 3.75 },
            { name: "English", marks: 90, grade: "A+", gradePoint: 4.00 }
        ],
        gpa: 3.40,
        cgpa: 3.40
    },
    {
        id: "S103",
        name: "Bob Johnson",
        department: "ME",
        semester: 5,
        email: "bob.j@email.com",
        phone: "+15550199",
        subjects: [
            { name: "Mathematics", marks: 48, grade: "F", gradePoint: 0.00 },
            { name: "Physics", marks: 55, grade: "C", gradePoint: 2.25 },
            { name: "Chemistry", marks: 60, grade: "C+", gradePoint: 2.50 },
            { name: "Computer Science", marks: 71, grade: "B", gradePoint: 3.00 },
            { name: "English", marks: 66, grade: "B-", gradePoint: 2.75 }
        ],
        gpa: 2.10,
        cgpa: 2.45
    }
];

/**
 * Format date string matching standard C activity logs
 */
function getTimestamp() {
    const now = new Date();
    const pad = (n) => n.toString().padStart(2, '0');
    return `${now.getFullYear()}-${pad(now.getMonth() + 1)}-${pad(now.getDate())} ${pad(now.getHours())}:${pad(now.getMinutes())}:${pad(now.getSeconds())}`;
}

/**
 * Logs an administrative action to the system logs.
 * @param {string} action 
 */
export function logActivity(action) {
    const logs = JSON.parse(localStorage.getItem('sms_logs') || '[]');
    const newLog = `[${getTimestamp()}] ${action}`;
    logs.push(newLog);
    localStorage.setItem('sms_logs', JSON.stringify(logs));
}

/**
 * Fetch all activity logs.
 * @returns {string[]}
 */
export function getActivityLogs() {
    return JSON.parse(localStorage.getItem('sms_logs') || '[]');
}

/**
 * Clears activity logs.
 */
export function clearActivityLogs() {
    localStorage.setItem('sms_logs', '[]');
    logActivity('System Activity Logs Cleared');
}

/**
 * Fetch all student records.
 * @returns {object[]}
 */
export function getStudents() {
    if (!localStorage.getItem('sms_students')) {
        localStorage.setItem('sms_students', JSON.stringify(SAMPLE_STUDENTS));
        logActivity('Default Sample Database Initialized');
    }
    return JSON.parse(localStorage.getItem('sms_students'));
}

/**
 * Save student records.
 * @param {object[]} students 
 */
export function saveStudents(students) {
    localStorage.setItem('sms_students', JSON.stringify(students));
}

/**
 * Resets database to sample student records.
 */
export function resetToSampleDatabase() {
    saveStudents(SAMPLE_STUDENTS);
    logActivity('Database Reset to Sample Records');
}

/**
 * Add a new student record to the system.
 * @param {object} student 
 * @returns {boolean} True if added, false if ID duplicate
 */
export function addStudent(student) {
    const students = getStudents();
    if (students.some(s => s.id.toLowerCase() === student.id.toLowerCase())) {
        return false;
    }
    
    students.push(student);
    saveStudents(students);
    logActivity(`Added student record for ID: ${student.id}`);
    return true;
}

/**
 * Update an existing student record.
 * @param {string} id 
 * @param {object} updatedData 
 * @returns {boolean} True if updated, false if not found
 */
export function updateStudent(id, updatedData) {
    const students = getStudents();
    const idx = students.findIndex(s => s.id === id);
    if (idx === -1) return false;
    
    students[idx] = { ...students[idx], ...updatedData };
    saveStudents(students);
    logActivity(`Updated student record for ID: ${id}`);
    return true;
}

/**
 * Delete a student record from the system.
 * @param {string} id 
 * @returns {boolean} True if deleted, false if not found
 */
export function deleteStudent(id) {
    const students = getStudents();
    const idx = students.findIndex(s => s.id === id);
    if (idx === -1) return false;
    
    students.splice(idx, 1);
    saveStudents(students);
    logActivity(`Deleted student record for ID: ${id}`);
    return true;
}

/**
 * Sorts students list by name or GPA.
 * @param {'name'|'gpa'} field 
 * @param {'asc'|'desc'} order 
 * @returns {object[]}
 */
export function sortStudents(field, order = 'asc') {
    const students = getStudents();
    students.sort((a, b) => {
        if (field === 'gpa') {
            return order === 'asc' ? a.gpa - b.gpa : b.gpa - a.gpa;
        } else {
            return order === 'asc' 
                ? a.name.localeCompare(b.name) 
                : b.name.localeCompare(a.name);
        }
    });
    saveStudents(students);
    logActivity(`Sorted database by ${field.toUpperCase()} (${order.toUpperCase()})`);
    return students;
}

/**
 * Exports current student data to CSV format and prompts download.
 */
export function exportDatabaseCSV() {
    const students = getStudents();
    let csvContent = 'Student ID,Full Name,Department,Semester,Email,Phone,';
    
    DEFAULT_SUBJECTS.forEach(subject => {
        csvContent += `${subject} Marks,${subject} Grade,${subject} GP,`;
    });
    csvContent += 'GPA,CGPA\n';
    
    students.forEach(s => {
        csvContent += `${s.id},"${s.name}","${s.department}",${s.semester},${s.email},${s.phone},`;
        DEFAULT_SUBJECTS.forEach((subName) => {
            const subject = s.subjects.find(sub => sub.name === subName) || { marks: 0, grade: 'F', gradePoint: 0.0 };
            csvContent += `${subject.marks},${subject.grade},${subject.gradePoint.toFixed(2)},`;
        });
        csvContent += `${s.gpa.toFixed(2)},${s.cgpa.toFixed(2)}\n`;
    });
    
    const blob = new Blob([csvContent], { type: 'text/csv;charset=utf-8;' });
    const link = document.createElement('a');
    link.href = URL.createObjectURL(blob);
    link.setAttribute('download', 'students_export.csv');
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
    logActivity('Database Exported to CSV Successfully');
}

/**
 * Downloads a backup of current database as JSON.
 */
export function backupDatabaseJSON() {
    const backupData = {
        students: getStudents(),
        admin_username: localStorage.getItem('sms_admin_username') || 'admin',
        admin_password: localStorage.getItem('sms_admin_password') || ''
    };
    
    const blob = new Blob([JSON.stringify(backupData, null, 2)], { type: 'application/json' });
    const link = document.createElement('a');
    link.href = URL.createObjectURL(blob);
    link.setAttribute('download', 'students_backup.json');
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
    logActivity('Database Backup Created Successfully');
}

/**
 * Restores database from a JSON backup.
 * @param {string} jsonString 
 * @returns {boolean} True if restore succeeded, false if invalid file content
 */
export function restoreDatabaseJSON(jsonString) {
    try {
        const backupData = JSON.parse(jsonString);
        if (!backupData || !Array.isArray(backupData.students)) {
            return false;
        }
        
        saveStudents(backupData.students);
        if (backupData.admin_username) {
            localStorage.setItem('sms_admin_username', backupData.admin_username);
        }
        if (backupData.admin_password) {
            localStorage.setItem('sms_admin_password', backupData.admin_password);
        }
        
        logActivity('Database Restored from Backup Successfully');
        return true;
    } catch (e) {
        return false;
    }
}

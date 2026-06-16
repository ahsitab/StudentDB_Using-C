import os
import struct

# Struct formats matching standard GCC x86_64 alignment (no pack pragma)
# Subject: name(50s) + pad(2x) + marks(i) + grade(5s) + pad(3x) + gp(f) = 68 bytes
SUBJECT_FORMAT = "50s2xi5s3xf" 

# Student: id(20s) + name(100s) + dept(50s) + pad(2x) + sem(i) + email(100s) + phone(20s) + subjects(340s) + gpa(f) + cgpa(f) = 644 bytes
STUDENT_FORMAT = "20s100s50s2xi100s20s340sff"

def pack_subject(name, marks, grade, gp):
    return struct.pack(SUBJECT_FORMAT, name.encode('utf-8'), marks, grade.encode('utf-8'), gp)

def pack_student(sid, name, dept, sem, email, phone, subjects_data, gpa, cgpa):
    subjects_bin = b"".join([pack_subject(*s) for s in subjects_data])
    return struct.pack(STUDENT_FORMAT, 
                       sid.encode('utf-8'), 
                       name.encode('utf-8'), 
                       dept.encode('utf-8'), 
                       sem, 
                       email.encode('utf-8'), 
                       phone.encode('utf-8'), 
                       subjects_bin, 
                       gpa, 
                       cgpa)

# Sample students data
students = [
    (
        "S101", "John Doe", "CSE", 3, "john.doe@email.com", "+1234567890",
        [
            ("Mathematics", 95, "A+", 4.00),
            ("Physics", 88, "A", 3.75),
            ("Chemistry", 82, "A-", 3.50),
            ("Computer Science", 92, "A+", 4.00),
            ("English", 76, "B+", 3.25)
        ],
        3.70, 3.82
    ),
    (
        "S102", "Alice Smith", "EEE", 1, "alice.smith@email.edu", "+1987654321",
        [
            ("Mathematics", 72, "B", 3.00),
            ("Physics", 68, "B-", 2.75),
            ("Chemistry", 80, "A-", 3.50),
            ("Computer Science", 85, "A", 3.75),
            ("English", 90, "A+", 4.00)
        ],
        3.40, 3.40
    ),
    (
        "S103", "Bob Johnson", "ME", 5, "bob.j@email.com", "+15550199",
        [
            ("Mathematics", 48, "F", 0.00),
            ("Physics", 55, "C", 2.25),
            ("Chemistry", 60, "C+", 2.50),
            ("Computer Science", 71, "B", 3.00),
            ("English", 66, "B-", 2.75)
        ],
        2.10, 2.45
    )
]

def main():
    os.makedirs("data", exist_ok=True)
    db_path = os.path.join("data", "students.bin")
    
    with open(db_path, "wb") as f:
        for s in students:
            bin_data = pack_student(*s)
            f.write(bin_data)
            
    print(f"Generated sample database file with {len(students)} records at '{db_path}'")

if __name__ == "__main__":
    main()

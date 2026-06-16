# Student Management System in C

A complete, professional, console-based Student Management System (SMS) implemented in C. This project utilizes persistent binary file storage, secure administrator login with XOR-encrypted credentials, automated grade scales and GPA/CGPA calculations, dynamic memory allocation, input formatting validation, custom terminal coloring, and logging.

---

## Project Structure

```text
e:/Mini_Project_For_C/
├── build.ps1             # PowerShell compilation script (autodetects MinGW GCC)
├── Makefile              # Project Makefile for standard compilation
├── generate_sample_db.py # Python script used to populate the binary database
├── README.md             # Project documentation (this file)
├── data/                 # Data directory
│   ├── students.bin      # Binary database containing student records (loaded at startup)
│   ├── admin.bin         # Secure binary file containing XOR-encrypted admin credentials
│   └── activity.log      # Log file containing time-stamped system operations
└── src/                  # Source code directory
    ├── main.c            # Application entry point and main menu loop
    ├── student.h         # Student struct definitions and core function prototypes
    ├── student.c         # Implementation of CRUD, sorting, and statistics
    ├── auth.h            # Authentication structures and function prototypes
    ├── auth.c            # Admin credentials management and security hashing
    ├── file_ops.h        # File serialization and CSV export prototypes
    ├── file_ops.c        # PERSISTENCE: Save/load bin, export CSV, backup & restore
    ├── utils.h           # Utility validation and UI coloring prototypes
    └── utils.c           # Windows coloring API, input cleanup, regex-like checkers
```

---

## Getting Started

### 1. Requirements
- A Windows Operating System.
- GCC Compiler (included in standard MinGW or Code::Blocks distributions).
- *Optional*: PowerShell (for easy compilation via build script).

### 2. Compilation
You can compile the system using either of the following methods:

#### Method A: Using the PowerShell Build Script (Recommended)
1. Open a PowerShell terminal in the project directory.
2. Run the script:
   ```powershell
   ./build.ps1
   ```
   *Note: This script automatically searches for a local compiler in standard directories (e.g., Code::Blocks) if GCC is not in your environment variable PATH.*

#### Method B: Using Makefile
1. Open a terminal in the project directory.
2. Run:
   ```cmd
   make
   ```

Both methods will output a standalone executable: `sms.exe`.

### 3. Execution
Run the compiled system using:
```cmd
.\sms.exe
```

---

## User Credentials

On first run, the system automatically initializes the administrative database with default credentials.

* **Default Username**: `admin`
* **Default Password**: `password123`

> [!TIP]
> Use the **Change Administrator Password** option in the Admin Menu to replace the default password with a custom secure password of 6+ characters.

---

## Core System Features

### 1. Administrative Authentication
- Maximum of **3 login attempts** to protect administrative access.
- Stored password is secured with a byte-wise XOR cipher to prevent plain-text reading of `data/admin.bin`.

### 2. Full Student CRUD Support
- **Add**: Creates unique student records. Incorporates validators for ID, phone number format, email syntax, and range limits.
- **Read**: Displays registered students in an aligned tabular list, including counts. Select search options to view deep academic details in card formats.
- **Update**: Offers a menu to update parts of a record (Personal info, Academics, Marks) or everything. Saves to file automatically.
- **Delete**: Permanently removes student records from memory and files after user confirmation.

### 3. GPA & Grade Scale Calculator
When entering marks for the 5 subjects, the system automatically maps each subject score to the corresponding grade points and calculates the final Semester GPA as an average:

| Score Bounds | Grade | Grade Point |
| :--- | :---: | :---: |
| 90 - 100 | A+ | 4.00 |
| 85 - 89 | A | 3.75 |
| 80 - 84 | A- | 3.50 |
| 75 - 79 | B+ | 3.25 |
| 70 - 74 | B | 3.00 |
| 65 - 69 | B- | 2.75 |
| 60 - 64 | C+ | 2.50 |
| 55 - 59 | C | 2.25 |
| 50 - 54 | D | 2.00 |
| Below 50 | F | 0.00 |

### 4. Advanced Statistics Dashboard
Displays analytical metrics:
- Total registered student count.
- Highest, lowest, and average class GPA.
- Dynamic department-wise count (automatically groups unique departments and displays headcount).

### 5. Bonus Enhancements
- **Export to CSV**: Exports database records into a spreadsheet-ready file at `data/students_export.csv`.
- **Database Sorting**: Sorts students dynamically by GPA (highest to lowest) or alphabetically (A to Z).
- **Backup & Restore**: Generates local backups at `data/students_backup.bin` and restores them in-session.
- **Activity Log**: Appends system action logs with calendar timestamps into `data/activity.log` for audit trials.
- **Aesthetic UI**: Custom output color schemes (cyan, green, yellow, red) via the Windows API.

---

## Technical Details

- **Dynamic Memory Allocation**: All students are loaded at runtime into a dynamically resizing array of structures (`Student* data`). As new students are added, `realloc` is invoked to expand the database capacity.
- **Input Validation**: Custom routines verify syntax (e.g., ensuring emails have both `@` and domain extensions, and phone numbers consist only of digits/dashes).
- **Memory Safety**: Clean destructuring frees all allocated blocks (`freeDatabase`) prior to shell exit.

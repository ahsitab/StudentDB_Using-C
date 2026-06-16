/**
 * Main Web UI Orchestrator
 */

import { isValidID, isValidName, isValidEmail, isValidPhone, getGradeAndPoint, DEFAULT_SUBJECTS } from './utils.js';
import { authenticateAdmin, changeAdminPassword, initAdminAccount } from './auth.js';
import { 
    getStudents, addStudent, updateStudent, deleteStudent, sortStudents,
    exportDatabaseCSV, backupDatabaseJSON, restoreDatabaseJSON,
    resetToSampleDatabase, logActivity, getActivityLogs, clearActivityLogs 
} from './database.js';

// Global application state variables
let loginAttempts = 0;
const MAX_LOGIN_ATTEMPTS = 3;

document.addEventListener('DOMContentLoaded', () => {
    // Initialize admin system
    initAdminAccount();
    
    // Check Session Auth
    checkAuthentication();
    
    // Set up nav event handlers
    setupNavigation();
    
    // Set up form submission handlers
    setupForms();
    
    // Setup actions in the dashboard/settings
    setupSystemActions();
    
    // Listen to Standalone Calculator inputs
    setupStandaloneCalculator();
    
    // Initialize directory tables
    updateUI();
});

/**
 * Validates login state and hides/shows overlay
 */
function checkAuthentication() {
    const isSessionActive = sessionStorage.getItem('sms_logged_in') === 'true';
    const loginOverlay = document.getElementById('loginOverlay');
    
    if (isSessionActive) {
        loginOverlay.classList.add('hidden');
        const username = localStorage.getItem('sms_admin_username') || 'admin';
        document.getElementById('profileUsername').innerText = username;
        document.getElementById('avatarLetter').innerText = username.charAt(0).toUpperCase();
    } else {
        loginOverlay.classList.remove('hidden');
    }
}

/**
 * Handle notification toasts
 * @param {string} message 
 * @param {'success'|'warning'|'danger'|'info'} type 
 */
function showToast(message, type = 'success') {
    const container = document.getElementById('toastContainer');
    const toast = document.createElement('div');
    toast.className = `toast ${type}`;
    
    let icon = '';
    if (type === 'success') icon = '<svg viewBox="0 0 24 24" width="20" height="20" stroke="currentColor" stroke-width="2.5" fill="none"><path d="M22 11.08V12a10 10 0 1 1-5.93-9.14"/><polyline points="22 4 12 14.01 9 11.01"/></svg>';
    else if (type === 'danger') icon = '<svg viewBox="0 0 24 24" width="20" height="20" stroke="currentColor" stroke-width="2.5" fill="none"><circle cx="12" cy="12" r="10"/><line x1="15" y1="9" x2="9" y2="15"/><line x1="9" y1="9" x2="15" y2="15"/></svg>';
    else if (type === 'warning') icon = '<svg viewBox="0 0 24 24" width="20" height="20" stroke="currentColor" stroke-width="2.5" fill="none"><path d="M10.29 3.86L1.82 18a2 2 0 0 0 1.71 3h16.94a2 2 0 0 0 1.71-3L13.71 3.86a2 2 0 0 0-3.42 0z"/><line x1="12" y1="9" x2="12" y2="13"/><line x1="12" y1="17" x2="12.01" y2="17"/></svg>';
    else icon = '<svg viewBox="0 0 24 24" width="20" height="20" stroke="currentColor" stroke-width="2.5" fill="none"><circle cx="12" cy="12" r="10"/><line x1="12" y1="16" x2="12" y2="12"/><line x1="12" y1="8" x2="12.01" y2="8"/></svg>';
    
    toast.innerHTML = `${icon}<span>${message}</span>`;
    container.appendChild(toast);
    
    // Auto-remove after 4 seconds
    setTimeout(() => {
        toast.style.animation = 'slide-in-toast 0.3s ease reverse forwards';
        setTimeout(() => {
            container.removeChild(toast);
        }, 300);
    }, 4000);
}

/**
 * Configure Sidebar View Switching
 */
function setupNavigation() {
    const navItems = document.querySelectorAll('.nav-menu .nav-item');
    const viewPanels = document.querySelectorAll('.view-panel');
    
    navItems.forEach(item => {
        item.addEventListener('click', () => {
            const target = item.getAttribute('data-target');
            
            navItems.forEach(i => i.classList.remove('active'));
            item.classList.add('active');
            
            viewPanels.forEach(panel => {
                if (panel.id === target) {
                    panel.classList.add('active');
                } else {
                    panel.classList.remove('active');
                }
            });
            
            // Re-render logs if settings view opened
            if (target === 'settingsView') {
                renderLogs();
            }
        });
    });
}

/**
 * Initialize form submit listeners
 */
function setupForms() {
    // 1. Admin Login Form
    const loginForm = document.getElementById('loginForm');
    const loginError = document.getElementById('loginError');
    
    loginForm.addEventListener('submit', (e) => {
        e.preventDefault();
        const user = document.getElementById('loginUser').value.trim();
        const pass = document.getElementById('loginPass').value;
        
        if (authenticateAdmin(user, pass)) {
            sessionStorage.setItem('sms_logged_in', 'true');
            loginAttempts = 0;
            loginError.style.display = 'none';
            checkAuthentication();
            updateUI();
            showToast('Welcome back, System Admin!', 'success');
        } else {
            loginAttempts++;
            if (loginAttempts >= MAX_LOGIN_ATTEMPTS) {
                document.getElementById('loginUser').disabled = true;
                document.getElementById('loginPass').disabled = true;
                loginForm.querySelector('.login-button').disabled = true;
                loginError.innerText = 'Lockout Active: Maximum login attempts exceeded.';
                loginError.style.display = 'block';
                logActivity('Admin Login Lockout Triggered');
            } else {
                loginError.innerText = `Invalid username or password. Attempt ${loginAttempts} of ${MAX_LOGIN_ATTEMPTS}.`;
                loginError.style.display = 'block';
            }
        }
    });
    
    // Logout Button Action
    document.getElementById('logoutBtn').addEventListener('click', () => {
        sessionStorage.removeItem('sms_logged_in');
        logActivity('Admin Logged Out');
        checkAuthentication();
        showToast('Administrator logged out successfully.', 'info');
    });
    
    // 2. Change Admin Password Form
    const changePassForm = document.getElementById('changePasswordForm');
    changePassForm.addEventListener('submit', (e) => {
        e.preventDefault();
        const oldPass = document.getElementById('oldPasswordInput').value;
        const newPass = document.getElementById('newPasswordInput').value;
        
        if (newPass.length < 6) {
            showToast('New password must be at least 6 characters.', 'warning');
            return;
        }
        
        if (changeAdminPassword(oldPass, newPass)) {
            showToast('Password updated successfully.', 'success');
            changePassForm.reset();
            renderLogs();
        } else {
            showToast('Failed to change password. Old password incorrect.', 'danger');
        }
    });
    
    // 3. Student Registration / Edit Form Validation
    const studentForm = document.getElementById('studentForm');
    
    // Setup real-time validators
    const nameInput = document.getElementById('studentName');
    nameInput.addEventListener('input', () => {
        document.getElementById('errStudentName').style.display = isValidName(nameInput.value) ? 'none' : 'block';
    });
    
    const phoneInput = document.getElementById('studentPhone');
    phoneInput.addEventListener('input', () => {
        document.getElementById('errStudentPhone').style.display = isValidPhone(phoneInput.value) ? 'none' : 'block';
    });
    
    const emailInput = document.getElementById('studentEmail');
    emailInput.addEventListener('input', () => {
        document.getElementById('errStudentEmail').style.display = isValidEmail(emailInput.value) ? 'none' : 'block';
    });
    
    const idInput = document.getElementById('studentId');
    idInput.addEventListener('input', () => {
        document.getElementById('errStudentId').style.display = isValidID(idInput.value) ? 'none' : 'block';
    });

    studentForm.addEventListener('submit', (e) => {
        e.preventDefault();
        
        const mode = document.getElementById('formMode').value;
        const id = idInput.value.trim();
        const name = nameInput.value.trim();
        const dept = document.getElementById('studentDept').value.trim();
        const sem = parseInt(document.getElementById('studentSemester').value);
        const email = emailInput.value.trim();
        const phone = phoneInput.value.trim();
        const cgpaVal = parseFloat(document.getElementById('studentCGPA').value);
        
        // Final Validator check
        if (!isValidID(id) || !isValidName(name) || !isValidEmail(email) || !isValidPhone(phone)) {
            showToast('Please correct form validation errors before saving.', 'warning');
            return;
        }
        
        // Subject marks reading
        const mathMarks = parseInt(document.getElementById('subMath').value);
        const physMarks = parseInt(document.getElementById('subPhys').value);
        const chemMarks = parseInt(document.getElementById('subChem').value);
        const csMarks = parseInt(document.getElementById('subCS').value);
        const engMarks = parseInt(document.getElementById('subEng').value);
        
        const mathGrade = getGradeAndPoint(mathMarks);
        const physGrade = getGradeAndPoint(physMarks);
        const chemGrade = getGradeAndPoint(chemMarks);
        const csGrade = getGradeAndPoint(csMarks);
        const engGrade = getGradeAndPoint(engMarks);
        
        const subjects = [
            { name: "Mathematics", marks: mathMarks, grade: mathGrade.grade, gradePoint: mathGrade.gp },
            { name: "Physics", marks: physMarks, grade: physGrade.grade, gradePoint: physGrade.gp },
            { name: "Chemistry", marks: chemMarks, grade: chemGrade.grade, gradePoint: chemGrade.gp },
            { name: "Computer Science", marks: csMarks, grade: csGrade.grade, gradePoint: csGrade.gp },
            { name: "English", marks: engMarks, grade: engGrade.grade, gradePoint: engGrade.gp }
        ];
        
        // GPA calculation
        const totalGP = mathGrade.gp + physGrade.gp + chemGrade.gp + csGrade.gp + engGrade.gp;
        const gpa = Number((totalGP / 5).toFixed(2));
        
        const studentRecord = {
            id, name, department: dept, semester: sem, email, phone, subjects, gpa, cgpa: cgpaVal
        };
        
        if (mode === 'add') {
            if (addStudent(studentRecord)) {
                showToast(`Student record for ${name} added successfully!`, 'success');
                closeDrawer();
                updateUI();
            } else {
                showToast(`Failed to add record. Student ID ${id} already exists.`, 'danger');
                document.getElementById('errStudentId').innerText = 'Student ID already exists.';
                document.getElementById('errStudentId').style.display = 'block';
            }
        } else {
            if (updateStudent(id, studentRecord)) {
                showToast(`Student record for ID ${id} updated successfully.`, 'success');
                closeDrawer();
                updateUI();
            } else {
                showToast(`Record not found.`, 'danger');
            }
        }
    });
    
    // Auto-calculate suggested CGPA when subject marks are altered
    const subMarkInputs = document.querySelectorAll('.sub-mark');
    subMarkInputs.forEach(input => {
        input.addEventListener('input', () => {
            let totalGP = 0;
            subMarkInputs.forEach(i => {
                totalGP += getGradeAndPoint(parseInt(i.value) || 0).gp;
            });
            const calculatedGPA = (totalGP / 5).toFixed(2);
            document.getElementById('studentCGPA').value = calculatedGPA;
        });
    });
}

/**
 * Configure UI Actions and persistent commands
 */
function setupSystemActions() {
    // Export database CSV
    document.getElementById('btnExportCSV').addEventListener('click', () => {
        exportDatabaseCSV();
        showToast('CSV export downloaded successfully.', 'success');
    });
    
    // Backup database JSON
    document.getElementById('btnBackupJSON').addEventListener('click', () => {
        backupDatabaseJSON();
        showToast('Database backup JSON file downloaded.', 'success');
    });
    
    // Restore database JSON upload
    const jsonFileInput = document.getElementById('importJSONFile');
    jsonFileInput.addEventListener('change', (e) => {
        const file = e.target.files[0];
        if (!file) return;
        
        const reader = new FileReader();
        reader.onload = (event) => {
            const contents = event.target.result;
            if (restoreDatabaseJSON(contents)) {
                showToast('Database restored successfully!', 'success');
                updateUI();
                renderLogs();
            } else {
                showToast('Failed to restore. Invalid JSON schema structure.', 'danger');
            }
        };
        reader.readAsText(file);
        // Clear value to allow re-upload of same file
        jsonFileInput.value = '';
    });
    
    // Reset database to samples
    document.getElementById('btnResetDB').addEventListener('click', () => {
        if (confirm('Are you sure you want to overwrite all records and restore default sample database?')) {
            resetToSampleDatabase();
            showToast('Database reset to defaults.', 'info');
            updateUI();
            renderLogs();
        }
    });
    
    // Clear logs terminal
    document.getElementById('btnClearLogs').addEventListener('click', () => {
        clearActivityLogs();
        renderLogs();
        showToast('System logs cleared.', 'info');
    });
    
    // Student Drawer Controls
    const addBtn = document.getElementById('addStudentBtn');
    const closeBtn = document.getElementById('drawerCloseBtn');
    const cancelBtn = document.getElementById('formCancelBtn');
    const overlay = document.getElementById('drawerOverlay');
    
    addBtn.addEventListener('click', () => openDrawer('add'));
    closeBtn.addEventListener('click', closeDrawer);
    cancelBtn.addEventListener('click', closeDrawer);
    
    // Close on overlay click
    overlay.addEventListener('click', (e) => {
        if (e.target === overlay) closeDrawer();
    });
}

/**
 * Drawer functions
 */
function openDrawer(mode = 'add', studentData = null) {
    const overlay = document.getElementById('drawerOverlay');
    const title = document.getElementById('drawerTitle');
    const formMode = document.getElementById('formMode');
    const idInput = document.getElementById('studentId');
    const form = document.getElementById('studentForm');
    
    // Clear validation errors
    document.querySelectorAll('.error-message').forEach(e => e.style.display = 'none');
    form.reset();
    
    overlay.classList.add('active');
    
    if (mode === 'add') {
        title.innerText = "Register New Student";
        formMode.value = "add";
        idInput.disabled = false;
        document.getElementById('studentCGPA').value = '0.00';
    } else {
        title.innerText = `Edit Record: ${studentData.name}`;
        formMode.value = "edit";
        
        // Populate inputs
        idInput.value = studentData.id;
        idInput.disabled = true; // Disable ID modification when editing
        document.getElementById('studentName').value = studentData.name;
        document.getElementById('studentDept').value = studentData.department;
        document.getElementById('studentSemester').value = studentData.semester;
        document.getElementById('studentPhone').value = studentData.phone;
        document.getElementById('studentEmail').value = studentData.email;
        document.getElementById('studentCGPA').value = studentData.cgpa.toFixed(2);
        
        // Populate subject marks
        const math = studentData.subjects.find(s => s.name === 'Mathematics') || { marks: 0 };
        const phys = studentData.subjects.find(s => s.name === 'Physics') || { marks: 0 };
        const chem = studentData.subjects.find(s => s.name === 'Chemistry') || { marks: 0 };
        const cs = studentData.subjects.find(s => s.name === 'Computer Science') || { marks: 0 };
        const eng = studentData.subjects.find(s => s.name === 'English') || { marks: 0 };
        
        document.getElementById('subMath').value = math.marks;
        document.getElementById('subPhys').value = phys.marks;
        document.getElementById('subChem').value = chem.marks;
        document.getElementById('subCS').value = cs.marks;
        document.getElementById('subEng').value = eng.marks;
    }
}

function closeDrawer() {
    document.getElementById('drawerOverlay').classList.remove('active');
}

/**
 * Configure Standalone GPA Calculator events
 */
function setupStandaloneCalculator() {
    const markInputs = document.querySelectorAll('.calc-mark-input');
    markInputs.forEach(input => {
        input.addEventListener('input', () => {
            const index = input.getAttribute('data-index');
            const marks = parseInt(input.value) || 0;
            const evalObj = getGradeAndPoint(marks);
            
            // Update single indicator
            document.getElementById(`calcInd-${index}`).innerText = `Grade: ${evalObj.grade} (${evalObj.gp.toFixed(2)})`;
            
            // Recalculate average
            let totalGP = 0;
            markInputs.forEach(i => {
                totalGP += getGradeAndPoint(parseInt(i.value) || 0).gp;
            });
            const finalGPA = totalGP / 5;
            
            // Update UI elements
            document.getElementById('calcGpaValue').innerText = finalGPA.toFixed(2);
            
            const honorsBadge = document.getElementById('calcHonorsBadge');
            const verdictText = document.getElementById('calcVerdictText');
            
            honorsBadge.className = 'honors-badge'; // Reset class
            if (finalGPA >= 3.75) {
                honorsBadge.innerText = 'First Class / Excellent';
                honorsBadge.classList.add('excellent');
                verdictText.innerText = 'Exceptional performance! You qualify for the highest academic Honors Classifications.';
            } else if (finalGPA >= 3.00) {
                honorsBadge.innerText = 'Second Class Upper / Good';
                honorsBadge.classList.add('good');
                verdictText.innerText = 'Good academic standing. Keep pushing to break into the First Class honors band!';
            } else if (finalGPA >= 2.00) {
                honorsBadge.innerText = 'Second Class Lower / Satisfactory';
                honorsBadge.classList.add('satisfactory');
                verdictText.innerText = 'Satisfactory standing. Focus on raising marks in weaker subjects to secure higher classifications.';
            } else {
                honorsBadge.innerText = 'Fail / Academic Warning';
                honorsBadge.classList.add('fail');
                verdictText.innerText = 'Academic Warning: Your GPA is currently below graduation requirements. Urgent review recommended.';
            }
        });
    });
}

/**
 * Renders activity logs in the mock terminal box
 */
function renderLogs() {
    const box = document.getElementById('logTerminalBox');
    const logs = getActivityLogs();
    
    if (logs.length === 0) {
        box.innerHTML = '<div class="terminal-line"><span class="system-tag">[SYSTEM]</span>No activity records logged.</div>';
        return;
    }
    
    box.innerHTML = logs.map(log => {
        // Extract timestamp
        const stampMatch = log.match(/^\[(.*?)\]/);
        const stamp = stampMatch ? stampMatch[1] : '';
        const msg = log.replace(/^\[.*?\]/, '').trim();
        
        return `<div class="terminal-line"><span class="timestamp">${stamp}</span><span class="system-tag">[SYS]</span>${msg}</div>`;
    }).join('');
    
    // Auto-scroll to bottom of logs
    box.scrollTop = box.scrollHeight;
}

/**
 * Main render function, updates Dashboard values, Charts, and Directories
 */
function updateUI() {
    const students = getStudents();
    
    // 1. Update Dashboard Stats
    const totalCount = students.length;
    document.getElementById('statTotalCount').innerText = totalCount;
    
    if (totalCount > 0) {
        const sumGPA = students.reduce((acc, curr) => acc + curr.gpa, 0);
        const avgGPA = sumGPA / totalCount;
        document.getElementById('statAvgGPA').innerText = avgGPA.toFixed(2);
        
        const highestGPA = Math.max(...students.map(s => s.gpa));
        document.getElementById('statHighestGPA').innerText = highestGPA.toFixed(2);
        
        const lowestGPA = Math.min(...students.map(s => s.gpa));
        document.getElementById('statLowestGPA').innerText = lowestGPA.toFixed(2);
    } else {
        document.getElementById('statAvgGPA').innerText = "0.00";
        document.getElementById('statHighestGPA').innerText = "0.00";
        document.getElementById('statLowestGPA').innerText = "0.00";
    }
    
    // 2. Generate Department Filter List & Enrolment Chart
    renderDepartmentEnrolmentChart(students);
    
    // 3. Generate GPA distribution Chart
    renderGPADistributionChart(students);
    
    // 4. Render Table Directory records
    renderDirectoryTable(students);
}

/**
 * Renders the department enrolment bar chart in SVG
 * @param {object[]} students 
 */
function renderDepartmentEnrolmentChart(students) {
    const container = document.getElementById('deptChartContainer');
    
    // Group departments count
    const deptMap = {};
    students.forEach(s => {
        const d = s.department.toUpperCase().trim();
        deptMap[d] = (deptMap[d] || 0) + 1;
    });
    
    const depts = Object.keys(deptMap);
    
    // Populate Department select filter dropdown
    const filterDept = document.getElementById('filterDept');
    const savedFilter = filterDept.value;
    filterDept.innerHTML = '<option value="">All Departments</option>';
    depts.forEach(d => {
        const option = document.createElement('option');
        option.value = d;
        option.innerText = d;
        if (d === savedFilter) option.selected = true;
        filterDept.appendChild(option);
    });
    
    if (students.length === 0) {
        container.innerHTML = '<span style="color: var(--text-muted);">No data records available</span>';
        return;
    }
    
    // Render Horizontal Bar Chart in SVG
    const rowHeight = 44;
    const padding = 20;
    const chartHeight = Math.max(rowHeight * depts.length + padding * 2, 200);
    const chartWidth = container.clientWidth || 400;
    
    const maxVal = Math.max(...Object.values(deptMap));
    
    let svgHtml = `<svg width="100%" height="${chartHeight}" viewBox="0 0 ${chartWidth} ${chartHeight}">`;
    
    depts.forEach((dept, i) => {
        const count = deptMap[dept];
        const percent = maxVal > 0 ? count / maxVal : 0;
        
        // Math coordinates
        const y = padding + i * rowHeight;
        const maxBarWidth = chartWidth - 160;
        const barWidth = Math.max(maxBarWidth * percent, 10);
        
        svgHtml += `
            <text x="10" y="${y + 24}" fill="var(--text-secondary)" font-family="var(--font-heading)" font-weight="600" font-size="13">${dept}</text>
            <rect x="100" y="${y + 10}" width="${maxBarWidth}" height="18" rx="4" fill="rgba(255,255,255,0.02)" border="1px solid var(--border-light)"></rect>
            <rect x="100" y="${y + 10}" width="${barWidth}" height="18" rx="4" fill="url(#barGradient)">
                <animate attributeName="width" from="0" to="${barWidth}" dur="0.8s" fill="freeze" />
            </rect>
            <text x="${100 + barWidth + 12}" y="${y + 24}" fill="var(--accent-primary)" font-weight="700" font-size="13">${count} ${count === 1 ? 'Student' : 'Students'}</text>
        `;
    });
    
    // Add gradient declaration
    svgHtml += `
        <defs>
            <linearGradient id="barGradient" x1="0%" y1="0%" x2="100%" y2="0%">
                <stop offset="0%" stop-color="var(--accent-primary)" />
                <stop offset="100%" stop-color="var(--accent-info)" />
            </linearGradient>
        </defs>
    </svg>`;
    
    container.innerHTML = svgHtml;
}

/**
 * Renders Academic GPA band distribution in SVG columns
 * @param {object[]} students 
 */
function renderGPADistributionChart(students) {
    const container = document.getElementById('gpaChartContainer');
    
    if (students.length === 0) {
        container.innerHTML = '<span style="color: var(--text-muted);">No data records available</span>';
        return;
    }
    
    // GPA Bands
    const bands = [
        { label: 'Excellent (3.75 - 4.0)', count: 0, color: 'var(--accent-primary)', gradient: 'excGrad' },
        { label: 'Good (3.0 - 3.74)', count: 0, color: 'var(--accent-success)', gradient: 'goodGrad' },
        { label: 'Satisfactory (2.0 - 2.99)', count: 0, color: 'var(--accent-warning)', gradient: 'satGrad' },
        { label: 'Fail (Below 2.0)', count: 0, color: 'var(--accent-danger)', gradient: 'failGrad' }
    ];
    
    students.forEach(s => {
        if (s.gpa >= 3.75) bands[0].count++;
        else if (s.gpa >= 3.00) bands[1].count++;
        else if (s.gpa >= 2.00) bands[2].count++;
        else bands[3].count++;
    });
    
    const maxVal = Math.max(...bands.map(b => b.count));
    
    // Render Horizontal bars for layout scaling
    const rowHeight = 50;
    const padding = 20;
    const chartHeight = rowHeight * bands.length + padding * 2;
    const chartWidth = container.clientWidth || 400;
    
    let svgHtml = `<svg width="100%" height="${chartHeight}" viewBox="0 0 ${chartWidth} ${chartHeight}">`;
    
    bands.forEach((band, i) => {
        const percent = maxVal > 0 ? band.count / maxVal : 0;
        const y = padding + i * rowHeight;
        const maxBarWidth = chartWidth - 180;
        const barWidth = Math.max(maxBarWidth * percent, 8);
        
        svgHtml += `
            <text x="10" y="${y + 24}" fill="var(--text-secondary)" font-size="12" font-weight="500">${band.label}</text>
            <rect x="140" y="${y + 10}" width="${maxBarWidth}" height="18" rx="4" fill="rgba(255,255,255,0.02)"></rect>
            <rect x="140" y="${y + 10}" width="${barWidth}" height="18" rx="4" fill="url(#${band.gradient})">
                <animate attributeName="width" from="0" to="${barWidth}" dur="0.8s" fill="freeze" />
            </rect>
            <text x="${140 + barWidth + 12}" y="${y + 24}" fill="${band.color}" font-weight="700" font-size="13">${band.count}</text>
        `;
    });
    
    svgHtml += `
        <defs>
            <linearGradient id="excGrad" x1="0%" y1="0%" x2="100%" y2="0%">
                <stop offset="0%" stop-color="var(--accent-primary)" /><stop offset="100%" stop-color="var(--accent-info)" />
            </linearGradient>
            <linearGradient id="goodGrad" x1="0%" y1="0%" x2="100%" y2="0%">
                <stop offset="0%" stop-color="var(--accent-success)" /><stop offset="100%" stop-color="var(--accent-primary)" />
            </linearGradient>
            <linearGradient id="satGrad" x1="0%" y1="0%" x2="100%" y2="0%">
                <stop offset="0%" stop-color="var(--accent-warning)" /><stop offset="100%" stop-color="hsl(45, 100%, 60%)" />
            </linearGradient>
            <linearGradient id="failGrad" x1="0%" y1="0%" x2="100%" y2="0%">
                <stop offset="0%" stop-color="var(--accent-danger)" /><stop offset="100%" stop-color="hsl(10, 80%, 60%)" />
            </linearGradient>
        </defs>
    </svg>`;
    
    container.innerHTML = svgHtml;
}

/**
 * Filter search, sort records and render output records in directory table
 * @param {object[]} students 
 */
function renderDirectoryTable(students) {
    const tableBody = document.getElementById('studentTableBody');
    
    const searchVal = document.getElementById('searchBar').value.toLowerCase().trim();
    const deptFilter = document.getElementById('filterDept').value;
    const semFilter = document.getElementById('filterSem').value;
    const sortVal = document.getElementById('sortOption').value;
    
    // Sort array in memory
    let records = [...students];
    
    if (sortVal === 'gpa-desc') {
        records.sort((a, b) => b.gpa - a.gpa);
    } else if (sortVal === 'name-asc') {
        records.sort((a, b) => a.name.localeCompare(b.name));
    }
    
    // Filter rows
    const filteredRecords = records.filter(s => {
        const matchSearch = s.id.toLowerCase().includes(searchVal) || 
                            s.name.toLowerCase().includes(searchVal) ||
                            s.department.toLowerCase().includes(searchVal);
        const matchDept = deptFilter === '' || s.department.toUpperCase() === deptFilter;
        const matchSem = semFilter === '' || s.semester.toString() === semFilter;
        
        return matchSearch && matchDept && matchSem;
    });
    
    if (filteredRecords.length === 0) {
        tableBody.innerHTML = '<tr><td colspan="8" style="text-align: center; padding: 40px; color: var(--text-secondary);">No student records match current criteria.</td></tr>';
        return;
    }
    
    tableBody.innerHTML = filteredRecords.map(s => {
        let gpaClass = 'low';
        if (s.gpa >= 3.75) gpaClass = 'high';
        else if (s.gpa >= 3.00) gpaClass = 'mid';
        
        return `
            <tr>
                <td><span class="student-id-badge">${s.id}</span></td>
                <td><span class="student-name-group">${s.name}</span></td>
                <td>${s.department}</td>
                <td>${s.semester}</td>
                <td><a class="student-email-link" href="mailto:${s.email}">${s.email}</a></td>
                <td><span class="gpa-badge ${gpaClass}">${s.gpa.toFixed(2)}</span></td>
                <td><strong>${s.cgpa.toFixed(2)}</strong></td>
                <td>
                    <div class="action-buttons">
                        <button class="btn-action btn-edit" data-id="${s.id}" title="Edit Student">
                            <svg viewBox="0 0 24 24"><path d="M11 4H4a2 2 0 0 0-2 2v14a2 2 0 0 0 2 2h14a2 2 0 0 0 2-2v-7"/><path d="M18.5 2.5a2.121 2.121 0 1 1 3 3L12 15l-4 1 1-4 9.5-9.5z"/></svg>
                        </button>
                        <button class="btn-action btn-delete" data-id="${s.id}" title="Delete Student">
                            <svg viewBox="0 0 24 24"><polyline points="3 6 5 6 21 6"/><path d="M19 6v14a2 2 0 0 1-2 2H7a2 2 0 0 1-2-2V6m3 0V4a2 2 0 0 1 2-2h4a2 2 0 0 1 2 2v2"/><line x1="10" y1="11" x2="10" y2="17"/><line x1="14" y1="11" x2="14" y2="17"/></svg>
                        </button>
                    </div>
                </td>
            </tr>
        `;
    }).join('');
    
    // Register actions for dynamic table buttons
    tableBody.querySelectorAll('.btn-edit').forEach(btn => {
        btn.addEventListener('click', () => {
            const studentId = btn.getAttribute('data-id');
            const student = students.find(s => s.id === studentId);
            if (student) openDrawer('edit', student);
        });
    });
    
    tableBody.querySelectorAll('.btn-delete').forEach(btn => {
        btn.addEventListener('click', () => {
            const studentId = btn.getAttribute('data-id');
            const student = students.find(s => s.id === studentId);
            if (student && confirm(`Are you sure you want to permanently delete student ${student.name} (ID: ${studentId})?`)) {
                if (deleteStudent(studentId)) {
                    showToast(`Deleted student ${student.name}.`, 'warning');
                    updateUI();
                    renderLogs();
                }
            }
        });
    });
}

// Bind search and filter triggers
document.getElementById('searchBar').addEventListener('input', () => renderDirectoryTable(getStudents()));
document.getElementById('filterDept').addEventListener('change', () => renderDirectoryTable(getStudents()));
document.getElementById('filterSem').addEventListener('change', () => renderDirectoryTable(getStudents()));
document.getElementById('sortOption').addEventListener('change', () => {
    const sortVal = document.getElementById('sortOption').value;
    if (sortVal === 'gpa-desc') {
        sortStudents('gpa', 'desc');
    } else if (sortVal === 'name-asc') {
        sortStudents('name', 'asc');
    }
    updateUI();
    renderLogs();
});

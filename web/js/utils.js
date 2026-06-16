/**
 * Utility functions for input validation and academic calculations.
 */

// Custom grade boundaries and point mappings matching student.c
export const GRADE_SCALE = [
    { min: 90, max: 100, grade: 'A+', gp: 4.00 },
    { min: 85, max: 89,  grade: 'A',  gp: 3.75 },
    { min: 80, max: 84,  grade: 'A-', gp: 3.50 },
    { min: 75, max: 79,  grade: 'B+', gp: 3.25 },
    { min: 70, max: 74,  grade: 'B',  gp: 3.00 },
    { min: 65, max: 69,  grade: 'B-', gp: 2.75 },
    { min: 60, max: 64,  grade: 'C+', gp: 2.50 },
    { min: 55, max: 59,  grade: 'C',  gp: 2.25 },
    { min: 50, max: 54,  grade: 'D',  gp: 2.00 },
    { min: 0,  max: 49,  grade: 'F',  gp: 0.00 }
];

export const DEFAULT_SUBJECTS = [
    "Mathematics",
    "Physics",
    "Chemistry",
    "Computer Science",
    "English"
];

/**
 * Validates Student ID format (Alphanumeric, underscores, and hyphens only).
 * @param {string} id 
 * @returns {boolean}
 */
export function isValidID(id) {
    const idRegex = /^[a-zA-Z0-9\-_]+$/;
    return id.trim().length > 0 && id.trim().length <= 20 && idRegex.test(id);
}

/**
 * Validates Student Name (Alphabetic characters, spaces, hyphens, and dots only).
 * @param {string} name 
 * @returns {boolean}
 */
export function isValidName(name) {
    const nameRegex = /^[a-zA-Z\s\.\-]+$/;
    return name.trim().length > 0 && name.trim().length <= 100 && nameRegex.test(name);
}

/**
 * Validates Email syntax (Must contain '@' and domain extension).
 * @param {string} email 
 * @returns {boolean}
 */
export function isValidEmail(email) {
    const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
    return email.trim().length > 0 && email.trim().length <= 100 && emailRegex.test(email);
}

/**
 * Validates Phone number (Must contain only digits, plus, hyphens, spaces, length 7-15 digits total).
 * @param {string} phone 
 * @returns {boolean}
 */
export function isValidPhone(phone) {
    const cleanPhone = phone.replace(/[\s\-\+]/g, '');
    const phoneRegex = /^\+?[0-9\s\-]+$/;
    return phoneRegex.test(phone) && cleanPhone.length >= 7 && cleanPhone.length <= 15;
}

/**
 * Maps subject score to Letter Grade and GP.
 * @param {number} marks 
 * @returns {{grade: string, gp: number}}
 */
export function getGradeAndPoint(marks) {
    const score = Math.round(Number(marks));
    const gradeMatch = GRADE_SCALE.find(scale => score >= scale.min && score <= scale.max);
    if (gradeMatch) {
        return { grade: gradeMatch.grade, gp: gradeMatch.gp };
    }
    return { grade: 'F', gp: 0.00 };
}

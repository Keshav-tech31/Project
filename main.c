/*
Program : Student Result Management System
Purpose : A console application for managing student records, enabling admins to add, view, update, and delete records.
Student can log in to view and download their results.

Language      : C
Database Used : M.S Excel
Author        : Keshav Sharma
Requirements  : Windows OS, C Compiler, CSV file stores the students's data in MS Excel for portability.
*/



// Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <conio.h>
#include <ctype.h>



// Defining Constants
#define MAX_LINE_LENGTH 1024
#define DATABASE_FILE "students.csv" // CSV file to store student data
#define MAX_NAME_LENGTH 50           // Maximum length for student names
#define MAX_ROLLNO_LENGTH 20         // Maximum length for roll numbers
#define MAX_LENGTH 30                // Maximum length for branch name
#define MAX_STUDENTS 40              // Maximum number of students allowed
#define NUM_SUBJECTS 4               // Number of theory subjects
#define NUM_SUBJECTS_PRAC 7          // Number of practical subjects

// Calculates the total number of subjects in the subjectMarks array 
#define TOTAL_SUBJECTS (sizeof(subjectMarks) / sizeof(SubjectMaxMarks))



//  Defining the subjects names
const char *subjects[NUM_SUBJECTS] = {

    "Software Engineering",
    "Android Application Development",
    "Java Programming",
    "Computer Hardware and Servicing"};



const char *practicalSubjects[NUM_SUBJECTS_PRAC] = {

    "Android Application Development",
    "Java Programming",
    "Computer Hardware and Servicing",
    "Minor Project Work",
    "Industrial Exposure",
    "Industrial Training",
    "General Proficiency"};




/* UserRole - Enum for defining user roles : ADMIN or STUDENT */
typedef enum
{
    ADMIN,  // Admin role with full permissions
    STUDENT // Student role with view-only permissions
} UserRole;



//  Student - Structure to store student details and their marks
typedef struct
{
    char rollNo[MAX_ROLLNO_LENGTH];                // Student roll number
    char name[MAX_NAME_LENGTH];                    // Student name
    char branch[MAX_LENGTH];                       // Student branch
    int semester;                                  // Student semester
    int obtainedMarks[NUM_SUBJECTS];               // Marks in theoretical subjects
    int obtainedPracticalMarks[NUM_SUBJECTS_PRAC]; // Marks in practical subjects

} Student;



// Structure to store maximum marks for each subject
typedef struct
{
    const char *subject;
    int maxTheory;
    int maxPractical;

} SubjectMaxMarks;



// Array to map subjects to their max theory and practical marks
// A structure and an array for mapping subjects to their max marks
SubjectMaxMarks subjectMarks[] = {

    {"Software Engineering", 30, 0},
    {"Android Application Development", 30, 30},
    {"Java Programming", 30, 30},
    {"Computer Hardware and Servicing", 30, 30},
    {"Minor Project Work", 0, 50},
    {"Industrial Exposure", 0, 25},
    {"Industrial Training", 0, 30},
    {"General Proficiency", 0, 25},
};



// Global Variables
Student students[MAX_STUDENTS]; // Array to store student records
int studentCount = 0;           // Count of the students in the system



// Function Prototypes
// Admin functions
void displayAdminMenu();          // Displays admin menu
void addStudent();                // Add a new student record
void viewAllStudents();           // Displays all student records
void searchStudent();             // Searches and displays a student by roll number
void updateStudent();             // Update details of a specific student
void deleteStudent();             // Deletes a student record
void viewSubjectWiseResults();    // Displays all student's marks for a specific subject

// Removes leading and trailing whitespace from the given string
void trim(char *str);




// Student functions
void displayStudentMenu();                        // Displays student menu
void displayStudentWithResults(const Student *s); // Displays result for a single student
void downloadMyResult();                          // Saves a student's result to a file



// Utility functions
void saveToCSV();          // Saves all records to CSV
void loadFromCSV();        // Loads record form CSV
void clearInputBuffer();   // Clears input buffer
void welcome_animation();  // Displays the welcome animation



UserRole login(); // Handles login for admin or student




/* Main Function

* main - Entry point of the program
* Loads student data form a CSV file and then prompts the use for login.
* Based on the login role, displays respective menus and functionalities.
* Admin can manage student records, while student can view/download their own results.

*/
int main()
{
    welcome_animation(); // Display the welcome message
    loadFromCSV();       // Load existing data from CSV file

    int loggedInStudentIndex = -1; // Stores index of logged-in student

    UserRole role = login(&loggedInStudentIndex); // Authenticate and get role

    if (role == ADMIN)
    {

        int choice;

        while (1)
        {
            displayAdminMenu();
            printf("Enter Your Choice : ");
            if (scanf("%d", &choice) != 1)
            {
                printf("Invalid Input! Please Enter a Number.\n");

                clearInputBuffer();
                continue;
            }

            switch (choice)
            {

            case 1:
                addStudent();
                break;

            case 2:
                viewAllStudents();
                break;

            case 3:
                searchStudent();
                break;

            case 4:
                updateStudent();
                break;

            case 5:
                deleteStudent();
                break;

            case 6:
                viewSubjectWiseResults();
                break;

            case 7:
                printf("Exiting The Program. Goodbye!\n");

                exit(0);
                break;

            default:
                printf("Invalid Choice! Please Select a Valid Option.\n");
            }
        }
    }
    else if (role == STUDENT && loggedInStudentIndex != -1)
    {

        int choice;

        while (1)
        {
            displayStudentMenu();

            printf("\n\nEnter Your Choice : ");

            if (scanf("%d", &choice) != 1)
            {
                printf("Invalid Input! Please Enter a Number.\n");

                clearInputBuffer();
                continue;
            }

            switch (choice)
            {

            case 1:
                displayStudentWithResults(&students[loggedInStudentIndex]);
                break;

            case 2:
                downloadMyResult(&students[loggedInStudentIndex]);
                break;

            case 3:
                printf("Exiting The Program. Goodbye!\n");
                exit(0);
                break;

            default:
                printf("Invalid Choice! Please Select a Valid Option.\n");
            }
        }
    }
    else
    {
        printf("Login Failed. Exiting Program.\n");
        exit(1);
    }

    return 0;
}




/*
welcome_animation - Displays a welcome
*/
void welcome_animation()
{
    printf("\n\n\n\n\n\n\n\n\n\n");

    printf("\t\t\t\t\t+==================================================+\n");
    printf("\t\t\t\t\t|                                                  |\n");
    printf("\t\t\t\t\t|        STUDENT RESULT MANAGEMENT SYSTEM          |\n");
    printf("\t\t\t\t\t|                                                  |\n");
    printf("\t\t\t\t\t+==================================================+\n\n\n\n\n\n");
}



/*
saveToCSV - Saves all student records to a CSV file.

This function iterates over the list of students and writes their details including roll number, name, branch, semester, theory marks nad practical marks to the save CSV file specified in DATABASE_FILE.
*/
void saveToCSV()
{

    FILE *fp = fopen(DATABASE_FILE, "w");

    if (fp == NULL)
    {
        printf("Error : Could Not Open Database File For Writing.\n");
        return;
    }

    // Write header for CSV file
    fprintf(fp, "RollNo,Name,Branch,Semester");
    for (int i = 0; i < NUM_SUBJECTS; i++)
    {
        fprintf(fp, ",Theory_%s", subjects[i]);
    }
    for (int i = 0; i < NUM_SUBJECTS_PRAC; i++)
    {
        fprintf(fp, ",Practical_%s", practicalSubjects[i]);
    }
    fprintf(fp, "\n");

    // Write each student's data
    for (int i = 0; i < studentCount; i++)
    {
        fprintf(fp, "%s,%s,%s,%d",
                students[i].rollNo,
                students[i].name,
                students[i].branch,
                students[i].semester);

        // Theory Marks
        for (int j = 0; j < NUM_SUBJECTS; j++)
        {
            fprintf(fp, ",%d", students[i].obtainedMarks[j]);
        }

        // Practical Marks
        for (int j = 0; j < NUM_SUBJECTS_PRAC; j++)
        {
            fprintf(fp, ",%d", students[i].obtainedPracticalMarks[j]);
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
    printf("Database Saved Successfully!\n");
}



/*
loadFromCSV - Loads student records form a CSV file.

This function reads data from the CSV file specified in DATABASE_FILE, and populates the students array with each student's details including theory and practical marks.

If the file does not exist, a message is displayed to inform the user.
*/
void loadFromCSV()
{
    FILE *fp = fopen(DATABASE_FILE, "r");
    if (fp == NULL)
    {
        printf("No Existing Database Found. Starting Fresh.\n");
        return;
    }
    char line[1024];
    int lineCount = 0;
    studentCount = 0;

    fgets(line, 1024, fp);  // Skip the header line

    while (fgets(line, 1024, fp) && studentCount < MAX_STUDENTS)
    {
        Student s;
        char *token = strtok(line, ",");
        if (token == NULL)
            continue;
        // Parse student details
        strncpy(s.rollNo, token, MAX_ROLLNO_LENGTH - 1);

        token = strtok(NULL, ",");
        strncpy(s.name, token, MAX_NAME_LENGTH - 1);

        token = strtok(NULL, ",");
        strncpy(s.branch, token, MAX_LENGTH - 1);

        token = strtok(NULL, ",");
        s.semester = atoi(token);
        //  Parse theory marks
        for (int i = 0; i < NUM_SUBJECTS; i++)
        {
            token = strtok(NULL, ",");
            s.obtainedMarks[i] = atoi(token);
        }
        // Parse practical marks
        for (int i = 0; i < NUM_SUBJECTS_PRAC; i++)
        {
            token = strtok(NULL, ",");
            s.obtainedPracticalMarks[i] = atoi(token);
        }
        students[studentCount++] = s;
    }
    fclose(fp);
    printf("Database Loaded Successfully! Total Students: %d\n", studentCount);
}



/*
displayAdminMenu - Displays the admin menu options.

This function provides a list of actions that an admin can perform, including adding, viewing, updating and deleting student's record, as well as viewing subject-wise results and exiting the program.
*/
void displayAdminMenu()
{
    printf("\n===== Admin Menu =====\n\n");
    printf("1. Add Student\n");
    printf("2. View All Students\n");
    printf("3. Search Student\n");
    printf("4. Update Student\n");
    printf("5. Delete Student\n");
    printf("6. View Subject Wise Result\n");
    printf("7. Exit\n\n");
}



/*
displayStudentMenu - Displays the menu options for a student user.

This function provides the list of actions available to a student such as viewing their results, downloading their results and exiting the program.
*/
void displayStudentMenu()
{
    printf("\n===== Student Menu =====\n\n");
    printf("1. View My Result\n");
    printf("2. Download My Result\n");
    printf("3. Exit\n");
}



/*

addStudent - Adds a new student to the system.

This function prompts the admin to input a student's details,  including their roll number, name, branch and marks for theory and practical subjects.
It checks for duplicate roll numbers to prevent adding the same student twice.

If maximum student capacity is reached, the function will return an error message.

*/
void addStudent()
{
    if (studentCount >= MAX_STUDENTS)
    {
        printf("Cannot add more Students. Maximum Limit Reached.\n");
        return;
    }

    Student s;

    // Prompt for roll number and check if it's unique
    printf("\n===== Add New Student =====\n\n");
    printf("Enter Student Roll No    : ");
    scanf("%s", s.rollNo);

    // Check for duplicate Roll Number
    int i;

    for (i = 0; i < studentCount; i++)
    {

        if (strcmp(students[i].rollNo, s.rollNo) == 0)
        {
            printf("A Student With Roll Number %s Already Exists. Operation Cancelled.\n", s.rollNo);
            return;
        }
    }

    // Get student name, branch and semester
    printf("Enter Student Name       : ");

    // Clear input buffer before reading string with spaces
    clearInputBuffer();

    fgets(s.name, MAX_NAME_LENGTH, stdin);

    // Remove newline character from fgets
    s.name[strcspn(s.name, "\n")] = 0;

    char branchSem[MAX_LENGTH]; // Variable to take input for branch/semester
    // Parse branch and semester input
    printf("Enter Student Branch/Sem : ");
    scanf("%s", branchSem);

    // Split branch and semester
    sscanf(branchSem, "%[^/]/%d", s.branch, &s.semester);

    // Input for Theory marks
    printf("\n===== SESSIONAL MARKS =====\n\n");
    printf("Marks Obtained in Theoretical Examination\n\n");

    // Header for theory marks
    printf("Subject                              Maximum Marks      Marks Obtained\n");
    printf("-----------------------------------------------------------------------\n\n");

    // Collect theory marks and validate each input
    for (i = 0; i < NUM_SUBJECTS; i++)
    {
        int marks;
        int validInput = 0;
        do
        {
            printf("%-40s 30", subjects[i]); // Print subject name and max marks
            printf("%-20s", " ");
            if (scanf("%d", &marks) != 1)
            {
                printf("Invalid Input! Please Enter a Number.\n");
                clearInputBuffer();
            }
            else
            {
                if (marks < 1 || marks > 30)
                {
                    printf("Marks Should be between 0 and 30. Please Re-Enter.\n");
                }
                else
                {
                    validInput = 1;
                }
            }

        } while (!validInput); // Repeat until valid input

        s.obtainedMarks[i] = marks; // Store valid marks
    }
    printf("-----------------------------------------------------------------------\n\n");

    printf("Marks for Theoretical Examination has been added!\n\n"); // Pop up message

    // for practical examination
    printf("Marks Obtained in Practical Examination\n\n");

    // Header for practical marks
    printf("Subject                              Maximum Marks      Marks Obtained\n");
    printf("-----------------------------------------------------------------------\n\n");

    // Practical marks collection and validation
    int marks;
    int validInput = 0;
    do
    {
        printf("Android Application Development%-10s 30", " ");
        printf("%-20s", " ");

        if (scanf("%d", &marks) != 1)
        {
            printf("Invalid Input! Please Enter a Number.\n");
            clearInputBuffer();
        }
        else
        {

            if (marks < 1 || marks > 30)
            {
                printf("Marks should be between 0 and 30. Please Re-Enter.\n");
            }
            else
            {
                validInput = 1;
            }
        }
    } while (!validInput);

    s.obtainedPracticalMarks[0] = marks; // Store valid marks

    validInput = 0;
    do
    {
        printf("Java Programming%-25s 30", " ");
        printf("%-20s", " ");

        if (scanf("%d", &marks) != 1)
        {
            printf("Invalid Input! Please Enter a Number.\n");
            clearInputBuffer();
        }
        else
        {
            if (marks < 1 || marks > 30)
            {
                printf("Marks should be between 0 and 30. Please Re-Enter.\n");
            }
            else
            {
                validInput = 1;
            }
        }

    } while (!validInput); // Repeat until valid input

    s.obtainedPracticalMarks[1] = marks; // Store valid marks

    validInput = 0;
    do
    {
        printf("Computer Hardware and Services%-11s 30", " ");
        printf("%-20s", " ");

        if (scanf("%d", &marks) != 1)
        {
            printf("Invalid Input! Please Enter a Number.\n");
            clearInputBuffer();
        }
        else
        {
            if (marks < 1 || marks > 30)
            {
                printf("Marks should be between 0 and 30. Please Re-Enter.\n");
            }
            else
            {
                validInput = 1;
            }
        }

    } while (!validInput); // Repeat until valid input

    s.obtainedPracticalMarks[2] = marks; // Store valid marks

    validInput = 0;
    do
    {
        printf("Minor Project Work%-23s 50", " ");
        printf("%-20s", " ");

        if (scanf("%d", &marks) != 1)
        {
            printf("Invalid Input! Please Enter a Number.\n");
            clearInputBuffer();
        }
        else
        {
            if (marks < 1 || marks > 50)
            {
                printf("Marks should be between 0 and 50. Please Re-Enter.\n");
            }
            else
            {
                validInput = 1;
            }
        }

    } while (!validInput); // Repeat until valid input

    s.obtainedPracticalMarks[3] = marks; // Store valid marks

    validInput = 0;
    do
    {
        printf("Industrial Exposure%-22s 25", " ");
        printf("%-20s", " ");

        if (scanf("%d", &marks) != 1)
        {
            printf("Invalid Input! Please Enter a Number.\n");
            clearInputBuffer();
        }
        else
        {
            if (marks < 1 || marks > 25)
            {
                printf("Marks should be between 0 and 25. Please Re-Enter.\n");
            }
            else
            {
                validInput = 1;
            }
        }

    } while (!validInput); // Repeat until valid input

    s.obtainedPracticalMarks[4] = marks; // Store valid marks

    validInput = 0;
    do
    {
        printf("Industrial Training%-22s 30", " ");
        printf("%-20s", " ");

        if (scanf("%d", &marks) != 1)
        {
            printf("Invalid Input! Please Enter a Number.\n");
            clearInputBuffer();
        }
        else
        {
            if (marks < 1 || marks > 30)
            {
                printf("Marks should be between 0 and 30. Please Re-Enter.\n");
            }
            else
            {
                validInput = 1;
            }
        }

    } while (!validInput); // Repeat until valid input

    s.obtainedPracticalMarks[5] = marks; // Store valid marks

    validInput = 0;
    do
    {
        printf("General Proficiency%-22s 25", " ");
        printf("%-20s", " ");

        if (scanf("%d", &marks) != 1)
        {
            printf("Invalid Input! Please Enter a Number.\n");
            clearInputBuffer();
        }
        else
        {
            if (marks < 1 || marks > 25)
            {
                printf("Marks should be between 0 and 25. Please Re-Enter.\n");
            }
            else
            {
                validInput = 1;
            }
        }

    } while (!validInput); // Repeat until valid input

    s.obtainedPracticalMarks[6] = marks; // Store valid marks

    printf("-----------------------------------------------------------------------\n\n");

    printf("Marks for Practical Examination has been added!\n\n"); // Pop up message


    // Save the new student to the system and update the CSV file 
    students[studentCount++] = s; // Add student to global array
    printf("Student Added Successfully!\n\n");

    saveToCSV();
}




/*
viewAllStudents - Displays all student records in the system.

This function lists the roll number, name, branch, and semester of all students currently stored in the system. 
It allows the admin to view a specific student's detailed results if desired. The function loops until the admin chooses to exit.
*/

void viewAllStudents()
{

    if (studentCount == 0)
    {
        printf("\n=== View All Students ===\n\n");
        printf("No Student Record available.\n\n");
        return; // Exit the function since there are no students to display
    }

    while (1) // Keep displaying until the admin chooses to exit
    {

        printf("\n=== View All Students ===\n\n");
        printf("Roll No\t\t\t\tName\t\t\t\tBranch/Sem\n");
        printf("------------------------------------------------------------------------------\n");

        // Display all students
        int i;
        for (i = 0; i < studentCount; i++)
        {
            printf("%-32s%-33s%s/%d\n", students[i].rollNo, students[i].name, students[i].branch, students[i].semester);
        }
        printf("------------------------------------------------------------------------------\n\n");

        // Ask if the admin wants to view a student's result
        char choice;
        printf("Do you want to see the result of a specific Student? (y/n): ");
        clearInputBuffer(); // Clear the input buffer
        scanf(" %c", &choice);
        printf("\n\n");

        if (choice == 'y' || choice == 'Y')
        {
            char rollNo[MAX_ROLLNO_LENGTH];
            printf("Enter the Roll No of the Student: ");
            clearInputBuffer(); // Clear the input buffer before reading
            scanf("%s", rollNo);

            // Search for the student
            int found = 0;
            for (i = 0; i < studentCount; i++)
            {
                if (strcmp(students[i].rollNo, rollNo) == 0)
                {
                    displayStudentWithResults(&students[i]); // Display the student's results
                    found = 1;
                    break;
                }
            }

            if (!found)
            {
                printf("No Student found with Roll No %s.\n\n", rollNo);
            }
        }

        // Ask if the admin wants to return to the list or exit
        char returnChoice;
        printf("Do you want to return to the Student List? (y/n): ");
        clearInputBuffer();          // Clear the input buffer
        scanf(" %c", &returnChoice); // Space before %c to consume any newline character

        if (returnChoice == 'n' || returnChoice == 'N')
        {
            break; // Exit the loop if the admin does not want to return
        }
    }

    // Option to return to the admin menu
    printf("Press any key to return to the Admin Menu...\n");
    clearInputBuffer(); // Clear the input buffer
    getchar();          // Wait for the user to press a key
}




/*
displayStudentWithResults - Displays a single student's result.

This function outputs a student's roll number, name, branch, semester and their marks in both theoretical and practical subjects.
It is typically used when a student views their own result or when an admin views the result of a specific student.
*/
void displayStudentWithResults(const Student *s)
{
    printf("\n=== Student Results ===\n\n");
    printf("Roll No    : %-20s\n", s->rollNo);
    printf("Name       : %-20s\n", s->name);
    printf("Branch/Sem : %s/%d\n", s->branch, s->semester);

    // Display theory marks

    printf("\n===Theoretical Marks===\n\n");
    printf("Subject                              Marks Obtained\n");
    printf("----------------------------------------------------\n");

    int i;
    for (i = 0; i < NUM_SUBJECTS; i++)
    {
        printf("%-40s %d\n", subjects[i], s->obtainedMarks[i]);
    }

    printf("----------------------------------------------------\n");

    // Display practical marks 

    printf("\n===Practical Marks===\n\n");
    printf("Subject                              Marks Obtained\n");
    printf("----------------------------------------------------\n");

    for (i = 0; i < NUM_SUBJECTS_PRAC; i++)
    {
        printf("%-40s %d\n", practicalSubjects[i], s->obtainedPracticalMarks[i]);
    }
    printf("----------------------------------------------------\n\n");
}




/*
downloadMyResult - Saves a student's result to a text file.

This function allows a student to download their results to a file named "student_results.txt." 
It includes their roll number, name, branch, semester and marks for both theoretical and practical subjects.
*/
void downloadMyResult(const Student *s)
{

    FILE *fp = fopen("student_results.txt", "w");
    if (fp == NULL)
    {
        printf("Error: Could not open file to save results.\n");
        return;
    }
    
    // Write student result
    fprintf(fp, "=== Your Result ===\n\n");
    fprintf(fp, "Roll No    : %s\n", s->rollNo);
    fprintf(fp, "Name       : %s\n", s->name);
    fprintf(fp, "Branch/Sem : %s/%d\n\n", s->branch, s->semester);

    // Write theoretical marks

    fprintf(fp, "===Theoretical Marks===\n\n");
    fprintf(fp, "Subject                              Marks Obtained\n");
    fprintf(fp, "------------------------------------------------------\n");
    for (int i = 0; i < NUM_SUBJECTS; i++)
    {
        fprintf(fp, "%-40s %d\n", subjects[i], s->obtainedMarks[i]);
    }
    fprintf(fp, "------------------------------------------------------\n\n");

    // Write practical marks

    fprintf(fp, "===Practical Marks===\n\n");
    fprintf(fp, "Subject                              Marks Obtained\n");
    fprintf(fp, "------------------------------------------------------\n");
    for (int i = 0; i < NUM_SUBJECTS_PRAC; i++)
    {
        fprintf(fp, "%-40s %d\n", practicalSubjects[i], s->obtainedPracticalMarks[i]);
    }
    fprintf(fp, "------------------------------------------------------\n\n");

    fclose(fp);
    printf("Student results have been saved to 'student_results.txt'.\n\n");
}





/*
trim - Trims leading and trailing whitespace from a string.

This function modifies a string to remove any spaces at the beginning or end.
It is particularly useful for handling user input that may contain accidental spaces.
*/
void trim(char *str)
{
    int start = 0;
    int end = strlen(str) - 1;

    // Trim leading spaces
    while (isspace((unsigned char)str[start]))
    {
        start++;
    }

    // Trim trailing spaces
    while (end >= start && isspace((unsigned char)str[end]))
    {
        end--;
    }

    // Shift characters to the front
    memmove(str, str + start, end - start + 1);
    str[end - start + 1] = '\0';
}




/*
findSubjectMaxMarks – Finds the maximum theory and practical marks for a given subject.

This function searches the "subjectMarks" array for a subject that matches the provided name (case-insensitive) and returns a pointer to its "SubjectMaxMarks" structure. 

If the subject is not found, it returns NULL.

Parameters:
- subject: The name of the subject to search for.

Returns:
- A pointer to the `SubjectMaxMarks` structure for the subject, or NULL if not found.
*/
SubjectMaxMarks *findSubjectMaxMarks(const char *subject)
{

    for (int i = 0; i < TOTAL_SUBJECTS; i++)
    {

        if (strcasecmp(subjectMarks[i].subject, subject) == 0)
        {

            return &subjectMarks[i];
        }
    }

    return NULL;
}




/*
viewSubjectWiseResults - Displays student results for a specific subject.

This function prompts the user to select a subject and then shows each student’s marks in the chosen subject formatted based on the subject's max theory and practical marks.
It also gives an option to download the displayed results to a text file.
*/

void viewSubjectWiseResults()
{
    int choice;
    const char *subject;

    printf("\n\n===== View Student's Result Subject Wise =====\n\n");

    printf("\nWhich subject's marks do you want to see? Please Enter your Choice:\n\n");

    printf("1. Software Engineering\n");
    printf("2. Android Application Development\n");
    printf("3. Java Programming\n");
    printf("4. Computer Hardware and Servicing\n");
    printf("5. Minor Project Work\n");
    printf("6. Industrial Exposure\n");
    printf("7. Industrial Training\n");
    printf("8. General Proficiency\n\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch (choice)
    {
    case 1:
        subject = "Software Engineering";
        break;
    case 2:
        subject = "Android Application Development";
        break;
    case 3:
        subject = "Java Programming";
        break;
    case 4:
        subject = "Computer Hardware and Servicing";
        break;
    case 5:
        subject = "Minor Project Work";
        break;
    case 6:
        subject = "Industrial Exposure";
        break;
    case 7:
        subject = "Industrial Training";
        break;
    case 8:
        subject = "General Proficiency";
        break;
    default:
        printf("Invalid choice. Returning to Main Menu.\n");
        return;
    }

    // Retrieval of max marks for the chosen subject

    SubjectMaxMarks *subjectMax = findSubjectMaxMarks(subject);

    if (subjectMax == NULL)
    {

        printf("Error : Subject max marks not found.\n");
        return;
    }

    FILE *file = fopen("students.csv", "r");
    if (file == NULL)
    {
        printf("Error: Could not open file.\n");
        return;
    }

    char line[MAX_LINE_LENGTH];
    char headers[MAX_LINE_LENGTH];
    int theoryIndex = -1, practicalIndex = -1;

    // Read and skip the header line
    fgets(headers, MAX_LINE_LENGTH, file);
    trim(headers); // Trim spaces from headers

    // Tokenize headers to find theory and practical column indices for the subject
    char *header = strtok(headers, ",");
    int colIndex = 0;
    while (header != NULL)
    {
        trim(header); // Trim spaces from each header
        // Convert both header and subject to lowercase for case-insensitive comparison
        char lowerHeader[MAX_LINE_LENGTH], lowerSubject[MAX_LINE_LENGTH];
        strcpy(lowerHeader, header);
        strcpy(lowerSubject, subject);

        for (int i = 0; lowerHeader[i]; i++)
            lowerHeader[i] = tolower(lowerHeader[i]);
        for (int i = 0; lowerSubject[i]; i++)
            lowerSubject[i] = tolower(lowerSubject[i]);

        if (strstr(lowerHeader, lowerSubject) != NULL)
        {
            if (strstr(lowerHeader, "theory") != NULL)
            {
                theoryIndex = colIndex;
            }
            else if (strstr(lowerHeader, "practical") != NULL)
            {
                practicalIndex = colIndex;
            }
        }
        header = strtok(NULL, ",");
        colIndex++;
    }

    if (theoryIndex == -1 && practicalIndex == -1)
    {
        printf("Subject not found.\n");
        fclose(file);
        return;
    }

    // Display results header
    printf("\n\n===== Result For %s =====\n\n", subject);
    printf("-------------------------------------------------------------------------------------------------------------\n");
    printf("Roll No         Name                Branch    Semester     Theory Marks (Max %d)     Practical Marks (Max %d)\n", subjectMax->maxTheory, subjectMax->maxPractical);
    printf("-------------------------------------------------------------------------------------------------------------\n");

    // Now read the actual student data
    while (fgets(line, MAX_LINE_LENGTH, file))
    {
        char *token;
        int currentCol = 0;
        char rollNo[20], name[50], branch[50], semester[10];
        char theoryMarks[10] = "N/A", practicalMarks[10] = "N/A";

        // Tokenize line
        token = strtok(line, ",");

        // Read roll number, name, branch, and semester
        strcpy(rollNo, token);
        token = strtok(NULL, ",");
        strcpy(name, token);
        token = strtok(NULL, ",");
        strcpy(branch, token);
        token = strtok(NULL, ",");
        strcpy(semester, token);
        token = strtok(NULL, ",");

        // Retrieve theory and practical marks for the subject
        for (currentCol = 4; currentCol < colIndex; currentCol++)
        {

            if (currentCol == theoryIndex)
            {
                strcpy(theoryMarks, token);
            }
            if (currentCol == practicalIndex)
            {
                strcpy(practicalMarks, token);
            }
            token = strtok(NULL, ",");
        }

        // Print row data
        printf("%-15s %-20s %-12s %-18s %-23s %s\n", rollNo, name, branch, semester, theoryMarks, practicalMarks);
    }

    printf("-------------------------------------------------------------------------------------------------------------\n");

    fclose(file);

    char printChoice;

    printf("\n\nDo you want to print this result? (y / n) : ");
    clearInputBuffer();
    scanf("%c", &printChoice);

    if (printChoice == 'y' || printChoice == 'Y') {

        FILE *downloadFile = fopen("subject_wise_result.txt", "w");

        if (downloadFile == NULL) {

            printf("Error : Could not open file to save results.\n");
            return;
        }


    fprintf(downloadFile,"\n\n===== Result For %s =====\n\n", subject);

    fprintf(downloadFile,"-------------------------------------------------------------------------------------------------------------\n");
    fprintf(downloadFile,"Roll No         Name                Branch    Semester     Theory Marks (Max %d)     Practical Marks (Max %d)\n", subjectMax->maxTheory, subjectMax->maxPractical);
    fprintf(downloadFile,"-------------------------------------------------------------------------------------------------------------\n");

    file = fopen("students.csv", "r");

    fgets(line, MAX_LINE_LENGTH, file);

    while (fgets(line, MAX_LINE_LENGTH, file)) {

        char *token;
        int currentCol = 0;

        char rollNo[20], name[50], branch[50], semester[10];
        char theoryMarks[10] = "N/A", practicalMarks[10] = "N/A";

        // Tokenize line
        token = strtok(line, ",");

        // Read roll number, name, branch, and semester
        strcpy(rollNo, token);
        token = strtok(NULL, ",");
        strcpy(name, token);
        token = strtok(NULL, ",");
        strcpy(branch, token);
        token = strtok(NULL, ",");
        strcpy(semester, token);
        token = strtok(NULL, ",");

        // Retrieve theory and practical marks for the subject
        for (currentCol = 4; token != NULL; currentCol++)
        {

            if (currentCol == theoryIndex)
            {
                strcpy(theoryMarks, token);
            }
            if (currentCol == practicalIndex)
            {
                strcpy(practicalMarks, token);
            }
            token = strtok(NULL, ",");
        }
        
        // Print row data
    fprintf(downloadFile,"%-15s %-20s %-12s %-18s %-23s %s\n", rollNo, name, branch, semester, theoryMarks, practicalMarks);

    }
    
        fprintf(downloadFile,"-------------------------------------------------------------------------------------------------------------\n");
        
        fclose(downloadFile);
        fclose(file);

        printf("Subject-Wise Student Results have been saved to 'subject_wise_results.txt'.\n");
    }
}




/*
searchStudent - Searches for a student by roll number and displays their details.

This function allows the admin to search for a student by their roll number.
If found, it displays the student's details, including their marks.
If not found, it displays an error message.
 */
void searchStudent()
{
    char rollNo[MAX_ROLLNO_LENGTH];
    int found = 0;

    printf("\n--- Search Student ---\n\n");
    printf("Enter Roll Number to Search : ");
    scanf("%s", rollNo);

    // Search for the students 
    int i;
    for (i = 0; i < studentCount; i++)
    {
        if (strcmp(students[i].rollNo, rollNo) == 0)
        {
            printf("\nStudent Found\n\n");
            displayStudentWithResults(&students[i]); // Display found student's details
            found = 1;
            break;
        }
    }

    // If student not found, show a message

    if (!found)
    {
        printf("No student found with Roll Number: %s\n", rollNo);
    }
}





/*
updateStudent - Updates a specific student's information.

This function allows the admin to update a student's name, theoretical marks or practical marks based on their roll number. Changes can be saved or discarded.
*/
void updateStudent()
{
    char updateRollNo[MAX_ROLLNO_LENGTH];

    printf("\n--- Update Student ---\n\n");
    printf("Enter Student Roll Number to Update : ");
    scanf("%s", updateRollNo);
    clearInputBuffer();

    // Find the student by roll number
    int found = -1;
    for (int i = 0; i < studentCount; i++)
    {
        if (strcmp(students[i].rollNo, updateRollNo) == 0)
        {
            found = i;
            break;
        }
    }

    // If student not found, show a message and exit

    if (found == -1)
    {
        printf("Student with Roll Number %s not found.\n\n", updateRollNo);
        return;
    }

    // Creating a temporary copy for updates
    Student tempStudent;
    memcpy(&tempStudent, &students[found], sizeof(Student)); // Using memcpy for safe copy

    int choice;

    do
    {
        // Display current student details and  prompt for update choice
        printf("\n=== Current Student Details ===\n");
        printf("Name     : %s\n", tempStudent.name);
        printf("Roll No. : %s\n", tempStudent.rollNo);
        printf("Branch   : %s\n", tempStudent.branch);
        printf("Semester : %d\n", tempStudent.semester);

        printf("\nWhat would you like to update?\n");
        printf("1. Name\n");
        printf("2. Theoretical Marks\n");
        printf("3. Practical Marks\n");
        printf("4. Save and Exit\n");
        printf("5. Exit without Saving\n");
        printf("\n\nEnter Your Choice : ");

        if (scanf("%d", &choice) != 1)
        {
            clearInputBuffer();
            choice = 0; // Invalid input
            continue;
        }
        clearInputBuffer();

        switch (choice)
        {
        case 1:
            printf("\nCurrent Name: %s\n", tempStudent.name);
            printf("Enter New Name (or press Enter to keep current): ");
            char newName[MAX_NAME_LENGTH];
            fgets(newName, MAX_NAME_LENGTH, stdin);
            newName[strcspn(newName, "\n")] = 0;
            if (strlen(newName) > 0)
            {
                strncpy(tempStudent.name, newName, MAX_NAME_LENGTH - 1);
                tempStudent.name[MAX_NAME_LENGTH - 1] = '\0';
                printf("Name Updated Successfully!\n");
            }
            else
            {
                printf("Name Unchanged.\n");
            }
            break;

        case 2:
            printf("\nUpdating Theoretical Marks:\n");
            for (int i = 0; i < NUM_SUBJECTS; i++)
            {
                printf("Current Marks for %s: %d\n", subjects[i], tempStudent.obtainedMarks[i]);
                printf("Enter New Marks: ");
                if (scanf("%d", &tempStudent.obtainedMarks[i]) != 1)
                {
                    printf("Invalid input! Keeping previous marks.\n");
                    clearInputBuffer();
                }
            }
            printf("Theoretical Marks Updated Successfully!\n");
            break;

        case 3:
            printf("\nUpdating Practical Marks:\n");
            for (int i = 0; i < NUM_SUBJECTS_PRAC; i++)
            {
                printf("Current Practical Marks for %s: %d\n", practicalSubjects[i], tempStudent.obtainedPracticalMarks[i]);
                printf("Enter New Practical Marks: ");
                if (scanf("%d", &tempStudent.obtainedPracticalMarks[i]) != 1)
                {
                    printf("Invalid input! Keeping previous marks.\n");
                    clearInputBuffer();
                }
            }
            printf("Practical Marks Updated Successfully!\n");
            break;

        case 4:
            // Save changes
            memcpy(&students[found], &tempStudent, sizeof(Student));
            saveToCSV();
            printf("\nChanges Saved Successfully.\n");
            return;

        case 5:
            printf("\nExiting Without Saving Changes.\n");
            return;

        default:
            printf("\nInvalid Choice! Please select a valid option.\n");
        }

        printf("\nPress Enter to continue...");
        getchar();

    } while (1); // Loop will exit through cases 4 or 5 only
}




/*
deleteStudent - Deletes a student record by roll number.

This function allows the admin to delete a student record by entering the roll number.
It first displays a list of all student and asks the admin to confirm befor deletion.
After deletion it updates the CSV file to reflect the changes.
*/
void deleteStudent()
{

    printf("\n=== View All Students ===\n\n");
    printf("Roll No\t\t\t\tName\t\t\t\tBranch/Sem\n");
    printf("------------------------------------------------------------------------------\n");

    // Display all students
    int i;
    for (i = 0; i < studentCount; i++)
    {
        printf("%-32s%-33s%s/%d\n", students[i].rollNo, students[i].name, students[i].branch, students[i].semester);
    }
    printf("------------------------------------------------------------------------------\n\n");

    char deleteRollNo[MAX_ROLLNO_LENGTH];
    printf("\n--- Delete Student ---\n\n");
    printf("Enter Student Roll Number to Delete : ");
    scanf("%s", deleteRollNo);

    // Search for the student
    int found = -1;
    for (i = 0; i < studentCount; i++)
    {
        if (strcmp(students[i].rollNo, deleteRollNo) == 0)
        {
            found = i;
            break;
        }
    }

    if (found == -1)
    {
        printf("Student with Roll Number %s not found.\n\n", deleteRollNo);
        return;
    }

    // Display student details
    printf("\n--- Student Details ---\n\n");
    printf("Roll No    : %s\n", students[found].rollNo);
    printf("Name       : %s\n", students[found].name);
    printf("Branch/Sem : %s/%d\n", students[found].branch, students[found].semester);

    printf("\n=== Theoretical Marks ===\n\n");
    printf("--------------------------------------------\n");

    for (i = 0; i < NUM_SUBJECTS; i++)
    {
        printf("%-35s :   %d\n", subjects[i], students[found].obtainedMarks[i]);
    }

    printf("\n=== Practical Marks ===\n\n");
    printf("--------------------------------------------\n");

    for (i = 0; i < NUM_SUBJECTS_PRAC; i++)
    {
        printf("%-35s :   %d\n", practicalSubjects[i], students[found].obtainedPracticalMarks[i]);
    }
    printf("--------------------------------------------\n");

    // Confirmation prompt
    char confirm;
    printf("\nAre you sure you want to delete this Student? (y/n): ");
    scanf(" %c", &confirm); // Note the space before %c to consume any leftover newline

    if (confirm == 'y' || confirm == 'Y')
    {
        // Shift all students after the found student to the left by one
        for (i = found; i < studentCount - 1; i++)
        {
            students[i] = students[i + 1];
        }

        // Decrement the student count
        studentCount--;

        printf("Student with Roll Number %s has been deleted successfully.\n\n", deleteRollNo);
    }
    else
    {
        printf("Deletion cancelled. No changes made.\n\n");
    }

    saveToCSV();
}





/*
clearInputBuffer - Clears the input buffer to avoid unwanted characters.

This function removes any leftover characters in the input buffer, especially useful when reading characters and integers after a string input.
*/
void clearInputBuffer()
{

    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}





/*
login - Authenticates the user and assigns a role.

This function prompts the user to select a role (Admin or Student).
For Admin, it checks username and password.
For Student, it verifies the roll number.

If successful, it returns the user's role otherwise, it allows retrying.
*/

UserRole login(int *studentIndex)
{
    int roleChoice;

    printf("=== Login ===\n\n");
    printf("Select Your Role\n\n");
    printf("1. Admin\n");
    printf("2. Student\n\n");
    printf("Enter Your Choice : ");
    if (scanf("%d", &roleChoice) != 1)
    {
        printf("Invalid Input! Defaulting to Student.\n");
        clearInputBuffer();
        return STUDENT;
    }
    printf("\n\n");
    clearInputBuffer();

    if (roleChoice == 1)
    {
        // Simple admin authentication
        char username[20], password[20];
        int i = 0;
        char ch;

        printf("Enter Admin Username : ");
        while ((ch = getch()) != '\r')
        { // Capture username input

            if (ch == '\b' && i > 0)
            { // Handle backspace

                printf("\b \b");
                i--;
            }
            else if (ch != '\b')
            {

                username[i++] = ch;
                printf("*"); // Print asterisk
            }
        }
        username[i] = '\0';

        printf("\nEnter Admin Password : ");
        i = 0;
        while ((ch = getch()) != '\r')
        { // Capture password input

            if (ch == '\b' && i > 0)
            {

                printf("\b \b");
                i--;
            }
            else if (ch != '\b')
            {

                password[i++] = ch;
                printf("*"); // Print asterisk
            }
        }
        password[i] = '\0';

        printf("\n\n");

        if (strcmp(username, "a") == 0 && strcmp(password, "1") == 0)
        {
            printf("Admin Login Successful!\n");
            return ADMIN;
        }
        else
        {
            printf("Invalid Admin Credentials! Please Re-Enter to Login.\n");
            return login(studentIndex); // Recursive call to  allow retry
        }
    }
    else if (roleChoice == 2)
    {
        // Student login by entering their Roll Number
        char studentRollNo[MAX_ROLLNO_LENGTH];
        printf("Enter Your Roll No : ");
        scanf("%19s", studentRollNo);
        clearInputBuffer();

        // Check if student exists
        int i;
        for (i = 0; i < studentCount; i++)
        {
            if (strcmp(students[i].rollNo, studentRollNo) == 0)
            {
                printf("Student Login Successful!\n");
                *studentIndex = i;
                return STUDENT;
            }
        }

        printf("Roll Number not found! Please Re-Enter to Login.\n");
        return login(studentIndex);
    }
    else
    {
        printf("Invalid Choice! Please Choose the Valid Option.\n");
        return login(studentIndex);
    }
}

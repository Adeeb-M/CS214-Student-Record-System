#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <limits>
using namespace std;

const int NUM_COURSES = 8;
const string COURSE_CODES[NUM_COURSES] = {
    "CS181", "CS214", "CS222", "IT221", "IT352", "COE121", "IC103", "ENG102"
};

// Grade range constants
const int MIN_GRADE = 0;
const int MAX_GRADE = 100;
const int MAX_STUDENTS = 100;
const int MAX_QUEUE_SIZE = 10;
const int MAX_STACK_SIZE = 10;

// File name for saving data
const string STUDENT_FILE = "students.dat";

struct Student {
    string name;
    int grades[NUM_COURSES];

    Student() {
        for (int i = 0; i < NUM_COURSES; i++) {
            grades[i] = 0;
        }
    }

    double getAverage() const {
        int sum = 0;
        for (int i = 0; i < NUM_COURSES; i++) {
            sum += grades[i];
        }
        return static_cast<double>(sum) / NUM_COURSES;
    }

    string getLetter(double grade) const {
        if (grade >= 95) return "A+";
        else if (grade >= 90) return "A";
        else if (grade >= 85) return "B+";
        else if (grade >= 80) return "B";
        else if (grade >= 75) return "C+";
        else if (grade >= 70) return "C";
        else if (grade >= 65) return "D+";
        else if (grade >= 60) return "D";
        else return "F";
    }

    string getGpaLetter() const {
        return getLetter(getAverage());
    }
};

// Custom implementation of Stack for deleted students
class StudentStack {
private:
    Student data[MAX_STACK_SIZE];
    int topIndex;

public:
    StudentStack() : topIndex(-1) {}

    bool isEmpty() const {
        return topIndex == -1;
    }

    bool isFull() const {
        return topIndex == MAX_STACK_SIZE - 1;
    }

    void push(const Student& student) {
        if (isFull()) {
            for (int i = 0; i < MAX_STACK_SIZE - 1; i++) {
                data[i] = data[i + 1];
            }
            data[topIndex] = student;
        } else {
            data[++topIndex] = student;
        }
    }

    Student pop() {
        if (isEmpty()) {
            cout << "Stack is empty!" << endl;
            return Student();
        }
        return data[topIndex--];
    }

    Student& top() {
        if (isEmpty()) {
            cout << "Stack is empty!" << endl;
            static Student empty;
            return empty;
        }
        return data[topIndex];
    }

    void displayStack() const {
        cout << "\n=== Deletion History ===\n";

        if (isEmpty()) {
            cout << "No deletion history available.\n";
            return;
        }

        cout << "Last " << (topIndex + 1) << " deleted students:\n";
        for (int i = topIndex; i >= 0; i--) {
            cout << "\n" << (topIndex - i + 1) << ". " << data[i].name << " (Average: "
                 << fixed << setprecision(2) << data[i].getAverage() << ")\n";
        }
    }

    int size() const {
        return topIndex + 1;
    }

    void getAllItems(Student* items, int& size) const {
        size = topIndex + 1;
        for (int i = 0; i <= topIndex; i++) {
            items[i] = data[i];
        }
    }

    void clear() {
        topIndex = -1;
    }

    void loadFromArray(const Student* items, int size) {
        clear();
        for (int i = 0; i < size && i < MAX_STACK_SIZE; i++) {
            push(items[i]);
        }
    }
};

// Custom implementation of Queue for search history
class StringQueue {
private:
    string data[MAX_QUEUE_SIZE];
    int front;
    int rear;
    int count;

public:
    StringQueue() : front(0), rear(-1), count(0) {}

    bool isEmpty() const {
        return count == 0;
    }

    bool isFull() const {
        return count == MAX_QUEUE_SIZE;
    }

    void enqueue(const string& value) {
        if (isFull()) {
            front = (front + 1) % MAX_QUEUE_SIZE;
            count--;
        }

        rear = (rear + 1) % MAX_QUEUE_SIZE;
        data[rear] = value;
        count++;
    }

    string dequeue() {
        if (isEmpty()) {
            cout << "Queue is empty!" << endl;
            return "";
        }

        string value = data[front];
        front = (front + 1) % MAX_QUEUE_SIZE;
        count--;
        return value;
    }

    string& peek() {
        if (isEmpty()) {
            cout << "Queue is empty!" << endl;
            static string empty;
            return empty;
        }
        return data[front];
    }

    int size() const {
        return count;
    }

    void getAllItems(string* items, int& size) const {
        size = count;
        for (int i = 0; i < count; i++) {
            int index = (front + i) % MAX_QUEUE_SIZE;
            items[i] = data[index];
        }
    }

    void clear() {
        front = 0;
        rear = -1;
        count = 0;
    }

    void loadFromArray(const string* items, int size) {
        clear();
        for (int i = 0; i < size && i < MAX_QUEUE_SIZE; i++) {
            enqueue(items[i]);
        }
    }
};

// Custom implementation of Min Heap for students
class StudentHeap {
private:
    Student data[MAX_STUDENTS];
    int heapSize;

    int parent(int i) {
        return (i - 1) / 2;
    }

    int leftChild(int i) {
        return 2 * i + 1;
    }

    int rightChild(int i) {
        return 2 * i + 2;
    }

    void heapify(int i) {
        int smallest = i;
        int left = leftChild(i);
        int right = rightChild(i);

        if (left < heapSize && data[left].getAverage() < data[smallest].getAverage()) {
            smallest = left;
        }

        if (right < heapSize && data[right].getAverage() < data[smallest].getAverage()) {
            smallest = right;
        }

        if (smallest != i) {
            swap(data[i], data[smallest]);
            heapify(smallest);
        }
    }

public:
    StudentHeap() : heapSize(0) {}

    void insert(const Student& student) {
        if (heapSize >= MAX_STUDENTS) {
            cout << "Heap is full!" << endl;
            return;
        }

        data[heapSize] = student;

        int i = heapSize++;
        while (i > 0 && data[parent(i)].getAverage() > data[i].getAverage()) {
            swap(data[i], data[parent(i)]);
            i = parent(i);
        }
    }

    Student extractMin() {
        if (heapSize <= 0) {
            cout << "Heap is empty!" << endl;
            return Student();
        }

        Student root = data[0];
        data[0] = data[--heapSize];

        if (heapSize > 0) {
            heapify(0);
        }

        return root;
    }

    bool empty() const {
        return heapSize == 0;
    }

    int size() const {
        return heapSize;
    }

    Student* findByName(const string& name) {
        for (int i = 0; i < heapSize; i++) {
            if (data[i].name == name) {
                return &data[i];
            }
        }
return NULL;
    }

    void removeByName(const string& name) {
        for (int i = 0; i < heapSize; i++) {
            if (data[i].name == name) {
                data[i] = data[--heapSize];

                if (heapSize > 0) {
                    heapify(0);
                }
                return;
            }
        }
    }

    void getAllStudents(Student* students, int& size) const {
        size = heapSize;
        for (int i = 0; i < heapSize; i++) {
            students[i] = data[i];
        }
    }

    void clear() {
        heapSize = 0;
    }

    void loadFromArray(const Student* students, int size) {
        clear();
        for (int i = 0; i < size && i < MAX_STUDENTS; i++) {
            insert(students[i]);
        }
    }
};

// Function prototypes
void addStudent(StudentHeap& studentHeap);
void editStudentGrade(StudentHeap& studentHeap, StringQueue& searchHistory);
void deleteStudent(StudentHeap& studentHeap, StudentStack& deletedStudents);
void searchStudent(StudentHeap& studentHeap, StringQueue& searchHistory);
void displayAllStudents(StudentHeap& studentHeap);
void showClassAverage(StudentHeap& studentHeap);
void showDeletionHistory(StudentStack& deletedStudents);
void displayStudent(const Student& student);
void saveDataToFile(StudentHeap& studentHeap, StudentStack& deletedStudents, StringQueue& searchHistory);
void loadDataFromFile(StudentHeap& studentHeap, StudentStack& deletedStudents, StringQueue& searchHistory);

int main() {
    StudentHeap studentHeap;
    StudentStack deletedStudents;
    StringQueue searchHistory;

    int choice;

    loadDataFromFile(studentHeap, deletedStudents, searchHistory);

    do {
        cout << "\n=== Student Record System ===\n";
        cout << "1. Add Student\n";
        cout << "2. Edit Student Grade\n";
        cout << "3. Delete Student\n";
        cout << "4. Search Student\n";
        cout << "5. Display All Students\n";
        cout << "6. Show Class Average\n";
        cout << "7. Show Deletion History\n";
        cout << "8. Save Data to File\n";
        cout << "9. Load Data from File\n";
        cout << "10. Exit\n";
        cout << "Select an option: ";
        cin >> choice;

        switch (choice) {
            case 1:
                addStudent(studentHeap);
                break;
            case 2:
                editStudentGrade(studentHeap, searchHistory);
                break;
            case 3:
                deleteStudent(studentHeap, deletedStudents);
                break;
            case 4:
                searchStudent(studentHeap, searchHistory);
                break;
            case 5:
                displayAllStudents(studentHeap);
                break;
            case 6:
                showClassAverage(studentHeap);
                break;
            case 7:
                showDeletionHistory(deletedStudents);
                break;
            case 8:
                saveDataToFile(studentHeap, deletedStudents, searchHistory);
                break;
            case 9:
                loadDataFromFile(studentHeap, deletedStudents, searchHistory);
                break;
            case 10: {
                cout << "Do you want to save data before exiting? (y/n): ";
                char saveChoice;
                cin >> saveChoice;

                if (saveChoice == 'y' || saveChoice == 'Y') {
                    saveDataToFile(studentHeap, deletedStudents, searchHistory);
                }

                cout << "Exiting...\n";
                break;
            }
            default:
                cout << "Invalid choice!\n";
        }

    } while (choice != 10);

    return 0;
}

void addStudent(StudentHeap& studentHeap) {
    Student s;
    cout << "Enter name of student: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, s.name);

    for (int j = 0; j < NUM_COURSES; ++j) {
        int grade;
        bool validInput = false;

        do {
            cout << "Enter grade for " << COURSE_CODES[j] << " (0-100): ";

            if (cin >> grade) {
                if (grade >= MIN_GRADE && grade <= MAX_GRADE) {
                    validInput = true;
                } else {
                    cout << "Invalid grade. Please enter a value between 0 and 100.\n";
                }
            } else {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Please enter a numeric value.\n";
            }
        } while (!validInput);

        s.grades[j] = grade;
    }

    studentHeap.insert(s);
    cout << "Student added successfully!\n";
}

void displayStudent(const Student& student) {
    cout << "\n-----------------------------\n";
    cout << "Name: " << student.name << "\n";

    for (int i = 0; i < NUM_COURSES; i++) {
        int grade = student.grades[i];
        cout << COURSE_CODES[i] << ": " << grade
             << " (" << student.getLetter(grade) << ")\n";
    }

    cout << "Average: " << fixed << setprecision(2) << student.getAverage()
         << " (" << student.getGpaLetter() << ")\n";
}

void editStudentGrade(StudentHeap& studentHeap, StringQueue& searchHistory) {
    string name;
    cout << "Enter student name to edit: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, name);

    searchHistory.enqueue(name);

    Student* student = studentHeap.findByName(name);
    if (!student) {
        cout << "Student not found.\n";
        return;
    }

    cout << "Enter course index to edit (1-" << NUM_COURSES << "): \n";
    for (int i = 0; i < NUM_COURSES; i++) {
        cout << (i + 1) << ". " << COURSE_CODES[i] << "\n";
    }

    int courseIndex;
    bool validCourse = false;

    do {
        cout << "Choose course (1-" << NUM_COURSES << "): ";

        if (cin >> courseIndex) {
            if (courseIndex >= 1 && courseIndex <= NUM_COURSES) {
                validCourse = true;
            } else {
                cout << "Invalid course index. Please enter a value between 1 and "
                     << NUM_COURSES << ".\n";
            }
        } else {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a numeric value.\n";
        }
    } while (!validCourse);

    courseIndex--;

    int newGrade;
    bool validGrade = false;

    do {
        cout << "Enter new grade (0-100): ";

        if (cin >> newGrade) {
            if (newGrade >= MIN_GRADE && newGrade <= MAX_GRADE) {
                validGrade = true;
            } else {
                cout << "Invalid grade. Please enter a value between 0 and 100.\n";
            }
        } else {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a numeric value.\n";
        }
    } while (!validGrade);

    student->grades[courseIndex] = newGrade;
    cout << "Grade updated successfully!\n";
}

void deleteStudent(StudentHeap& studentHeap, StudentStack& deletedStudents) {
    string name;
    cout << "Enter student name to delete: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, name);

    Student* student = studentHeap.findByName(name);
    if (!student) {
        cout << "Student not found.\n";
        return;
    }

    deletedStudents.push(*student);
    studentHeap.removeByName(name);

    cout << "Student deleted successfully!\n";
    cout << "Deleted students can be viewed in deletion history.\n";
}

void searchStudent(StudentHeap& studentHeap, StringQueue& searchHistory) {
    string name;
    cout << "Enter student name to search: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, name);

    searchHistory.enqueue(name);

    Student* student = studentHeap.findByName(name);
    if (!student) {
        cout << "Student not found.\n";
        return;
    }

    displayStudent(*student);

    cout << "\nRecent searches (" << searchHistory.size() << "):\n";

    string searchItems[MAX_QUEUE_SIZE];
    int searchSize = 0;
    searchHistory.getAllItems(searchItems, searchSize);

    for (int i = 0; i < searchSize; i++) {
        cout << "- " << searchItems[i] << "\n";
    }
}

void displayAllStudents(StudentHeap& studentHeap) {
    if (studentHeap.empty()) {
        cout << "No students in the system.\n";
        return;
    }

    cout << "\n=== All Students ===\n";

    Student allStudents[MAX_STUDENTS];
    int numStudents = 0;
    studentHeap.getAllStudents(allStudents, numStudents);

    for (int i = 0; i < numStudents - 1; i++) {
        for (int j = 0; j < numStudents - i - 1; j++) {
            if (allStudents[j].name > allStudents[j + 1].name) {
                swap(allStudents[j], allStudents[j + 1]);
            }
        }
    }

    for (int i = 0; i < numStudents; i++) {
        displayStudent(allStudents[i]);
    }

    cout << "\nTotal students: " << numStudents << "\n";
}

void showClassAverage(StudentHeap& studentHeap) {
    if (studentHeap.empty()) {
        cout << "No students in the system.\n";
        return;
    }

    Student allStudents[MAX_STUDENTS];
    int numStudents = 0;
    studentHeap.getAllStudents(allStudents, numStudents);

    double totalAverage = 0.0;

    for (int i = 0; i < numStudents; i++) {
        totalAverage += allStudents[i].getAverage();
    }

    totalAverage /= numStudents;

    Student tempStudent;
    string letterGrade = tempStudent.getLetter(totalAverage);

    cout << "\n=== Class Average ===\n";
    cout << "Number of students: " << numStudents << "\n";
    cout << "Average grade: " << fixed << setprecision(2) << totalAverage
         << " (" << letterGrade << ")\n";

    cout << "\nAverage by Course:\n";
    for (int c = 0; c < NUM_COURSES; c++) {
        double courseSum = 0.0;

        for (int i = 0; i < numStudents; i++) {
            courseSum += allStudents[i].grades[c];
        }

        double courseAvg = courseSum / numStudents;
        string courseLetter = tempStudent.getLetter(courseAvg);

        cout << COURSE_CODES[c] << ": " << fixed << setprecision(2) << courseAvg
             << " (" << courseLetter << ")\n";
    }
}

void showDeletionHistory(StudentStack& deletedStudents) {
    if (deletedStudents.isEmpty()) {
        cout << "No deletion history available.\n";
        return;
    }

    deletedStudents.displayStack();
}

void saveDataToFile(StudentHeap& studentHeap, StudentStack& deletedStudents, StringQueue& searchHistory) {
   ofstream outFile(STUDENT_FILE.c_str(), ios::binary);

    if (!outFile) {
        cout << "Error: Could not open file for writing.\n";
        return;
    }

    Student students[MAX_STUDENTS];
    int studentCount = 0;
    studentHeap.getAllStudents(students, studentCount);

    Student deletedStudentArray[MAX_STACK_SIZE];
    int deletedCount = 0;
    deletedStudents.getAllItems(deletedStudentArray, deletedCount);

    string searches[MAX_QUEUE_SIZE];
    int searchCount = 0;
    searchHistory.getAllItems(searches, searchCount);

    outFile.write(reinterpret_cast<char*>(&studentCount), sizeof(studentCount));
    outFile.write(reinterpret_cast<char*>(&deletedCount), sizeof(deletedCount));
    outFile.write(reinterpret_cast<char*>(&searchCount), sizeof(searchCount));

    for (int i = 0; i < studentCount; i++) {
        int nameLength = students[i].name.length();
        outFile.write(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
        outFile.write(students[i].name.c_str(), nameLength);

        for (int j = 0; j < NUM_COURSES; j++) {
            outFile.write(reinterpret_cast<char*>(&students[i].grades[j]), sizeof(int));
        }
    }

    for (int i = 0; i < deletedCount; i++) {
        int nameLength = deletedStudentArray[i].name.length();
        outFile.write(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
        outFile.write(deletedStudentArray[i].name.c_str(), nameLength);

        for (int j = 0; j < NUM_COURSES; j++) {
            outFile.write(reinterpret_cast<char*>(&deletedStudentArray[i].grades[j]), sizeof(int));
        }
    }

    for (int i = 0; i < searchCount; i++) {
        int strLength = searches[i].length();
        outFile.write(reinterpret_cast<char*>(&strLength), sizeof(strLength));
        outFile.write(searches[i].c_str(), strLength);
    }

    outFile.close();
    cout << "Data saved successfully to " << STUDENT_FILE << "!\n";
}

void loadDataFromFile(StudentHeap& studentHeap, StudentStack& deletedStudents, StringQueue& searchHistory) {
    ifstream inFile(STUDENT_FILE.c_str(), ios::binary);

    if (!inFile) {
        cout << "Note: No saved data file found. Starting with empty database.\n";
        return;
    }

    try {
        int studentCount = 0;
        int deletedCount = 0;
        int searchCount = 0;

        inFile.read(reinterpret_cast<char*>(&studentCount), sizeof(studentCount));
        inFile.read(reinterpret_cast<char*>(&deletedCount), sizeof(deletedCount));
        inFile.read(reinterpret_cast<char*>(&searchCount), sizeof(searchCount));

        if (studentCount < 0 || studentCount > MAX_STUDENTS ||
            deletedCount < 0 || deletedCount > MAX_STACK_SIZE ||
            searchCount < 0 || searchCount > MAX_QUEUE_SIZE) {
            throw runtime_error("Invalid data in file");
        }

        Student students[MAX_STUDENTS];

        for (int i = 0; i < studentCount; i++) {
            int nameLength = 0;
            inFile.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));

            if (nameLength <= 0 || nameLength > 100) {
                throw runtime_error("Invalid name length in file");
            }

            char* nameBuf = new char[nameLength + 1];
            inFile.read(nameBuf, nameLength);
            nameBuf[nameLength] = '\0';
            students[i].name = string(nameBuf);
            delete[] nameBuf;

            for (int j = 0; j < NUM_COURSES; j++) {
                inFile.read(reinterpret_cast<char*>(&students[i].grades[j]), sizeof(int));

                if (students[i].grades[j] < MIN_GRADE || students[i].grades[j] > MAX_GRADE) {
                    throw runtime_error("Invalid grade in file");
                }
            }
        }

        Student deletedStudentArray[MAX_STACK_SIZE];

        for (int i = 0; i < deletedCount; i++) {
            int nameLength = 0;
            inFile.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));

            if (nameLength <= 0 || nameLength > 100) {
                throw runtime_error("Invalid name length in deleted students");
            }

            char* nameBuf = new char[nameLength + 1];
            inFile.read(nameBuf, nameLength);
            nameBuf[nameLength] = '\0';
            deletedStudentArray[i].name = string(nameBuf);
            delete[] nameBuf;

            for (int j = 0; j < NUM_COURSES; j++) {
                inFile.read(reinterpret_cast<char*>(&deletedStudentArray[i].grades[j]), sizeof(int));

                if (deletedStudentArray[i].grades[j] < MIN_GRADE ||
                    deletedStudentArray[i].grades[j] > MAX_GRADE) {
                    throw runtime_error("Invalid grade in deleted students");
                }
            }
        }

        string searches[MAX_QUEUE_SIZE];

        for (int i = 0; i < searchCount; i++) {
            int strLength = 0;
            inFile.read(reinterpret_cast<char*>(&strLength), sizeof(strLength));

            if (strLength <= 0 || strLength > 100) {
                throw runtime_error("Invalid string length in search history");
            }

            char* strBuf = new char[strLength + 1];
            inFile.read(strBuf, strLength);
            strBuf[strLength] = '\0';
            searches[i] = string(strBuf);
            delete[] strBuf;
        }

        inFile.close();

        studentHeap.clear();
        deletedStudents.clear();
        searchHistory.clear();

        studentHeap.loadFromArray(students, studentCount);
        deletedStudents.loadFromArray(deletedStudentArray, deletedCount);
        searchHistory.loadFromArray(searches, searchCount);

        cout << "Data loaded successfully from " << STUDENT_FILE << "!\n";
        cout << "Loaded " << studentCount << " students, " << deletedCount
             << " deletion records, and " << searchCount << " search history items.\n";
    }
    catch (const exception& e) {
        inFile.close();

        cout << "Error reading file: " << e.what() << "\n";
        cout << "File may be corrupted. Starting with empty database.\n";

        studentHeap.clear();
        deletedStudents.clear();
        searchHistory.clear();
    }
}

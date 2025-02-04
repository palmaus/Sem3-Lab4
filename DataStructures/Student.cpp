#include "Student.h"
#include <cstring>
#include <limits>
#include <iomanip>
#include <iostream>

Person::Person() : ID(0) {
    dateOfBirth[0] = dateOfBirth[1] = dateOfBirth[2] = 0;
}

Person::Person(const std::string& firstName, const std::string& lastName, int ID, const int dob[3]) :
    firstName(firstName), lastName(lastName), ID(ID) {
    std::copy(dob, dob + 3, this->dateOfBirth);
}

Person::~Person() {}

Person::Person(const Person& other) : firstName(other.firstName), lastName(other.lastName), ID(other.ID) {
    std::copy(other.dateOfBirth, other.dateOfBirth + 3, dateOfBirth);
}

Person& Person::operator=(const Person& other) {
    if (this != &other) {
        firstName = other.firstName;
        lastName = other.lastName;
        ID = other.ID;
        std::copy(other.dateOfBirth, other.dateOfBirth + 3, dateOfBirth);
    }
    return *this;
}

std::istream& operator>>(std::istream& is, Person& person) {
    std::string buffer;

    std::cout << "Enter first name: ";
    while (true) {
        is >> std::setw(256) >> buffer;
        bool valid = true;
        for (char c : buffer) {
            if (!std::isalpha(c)) {
                std::cout << "Invalid input. Please enter a valid name: ";
                is.clear(); // Очищаем флаги ошибок
                is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                valid = false;
                break;
            }
        }
        if (valid) break;
    }
    person.firstName = buffer;

    std::cout << "Enter last name: ";
    while (true) {
        is >> std::setw(256) >> buffer;
        bool valid = true;
        for (char c : buffer) {
            if (!std::isalpha(c)) {
                std::cout << "Invalid input. Please enter a valid name: ";
                is.clear();
                is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                valid = false;
                break;
            }
        }
        if (valid) break;
    }
    person.lastName = buffer;

    std::cout << "Enter ID: ";
    while (!(is >> person.ID)) {
        std::cout << "Invalid input. Please enter a number for ID: ";
        is.clear();
        is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');


    std::cout << "Enter date of birth (day month year): ";
    for (int& dob : person.dateOfBirth) {
        while (!(is >> dob)) {
            std::cout << "Invalid input. Please enter a number for date of birth: ";
            is.clear();
            is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
    is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    return is;
}

std::ostream& operator<<(std::ostream& os, const Person& person) {
    os << person.getFirstName() << " " << person.getLastName() << " " << person.getID() << " ";
    for (int i = 0; i < 3; ++i) {
        os << person.getDateOfBirth()[i] << " ";
    }
    return os;
}

Student::Student() : Person(), yearOfStudy(0) {}

Student::Student(const std::string& firstName, const std::string& lastName, int ID, const int dob[], int year) : Person(firstName, lastName, ID, dob), yearOfStudy(year) {}


Student::Student(const Student& other) : Person(other), yearOfStudy(other.yearOfStudy) {}

Student& Student::operator=(const Student& other) {
    if (this != &other) {
        Person::operator=(other);
        yearOfStudy = other.yearOfStudy;
    }
    return *this;
}

std::istream& operator>>(std::istream& is, Student& student) {
    is >> static_cast<Person&>(student);
    std::cout << "Enter year of study: ";
    while (!(is >> student.yearOfStudy)) {
        is.clear();
        is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Please enter a number for year of study: ";
    }
    is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return is;
}

std::ostream& operator<<(std::ostream& os, const Student& student) {
    os << static_cast<const Person&>(student) << student.getYearOfStudy();
    return os;
}

std::istream& inputFromFile(std::istream& is, Person& person) {
    std::string firstName;
    std::string lastName;
    int id;
    int dob[3];

    if (is >> firstName >> lastName >> id >> dob[0] >> dob[1] >> dob[2]) {
        person = Person(firstName, lastName, id, dob);
    }
    else {
        is.clear();
        is.setstate(std::ios::failbit);
    }
    return is;
}

std::istream& inputFromFile(std::istream& is, Student& student) {
    std::string firstName;
    std::string lastName;
    int id;
    int dob[3];
    int yearOfStudy;

    if (is >> firstName >> lastName >> id >> dob[0] >> dob[1] >> dob[2] >> yearOfStudy) {
        student = Student(firstName, lastName, id, dob, yearOfStudy);
    }
    else {
        is.clear();
        is.setstate(std::ios::failbit);
    }

    return is;
}
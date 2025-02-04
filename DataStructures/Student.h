#ifndef STUDENT_H
#define STUDENT_H

#include <string>
#include <iostream>
#include <algorithm>

class Person {
private:
    std::string firstName;
    std::string lastName;
    int ID;
    int dateOfBirth[3];
public:
    Person();
    Person(const std::string& firstName, const std::string& lastName, int ID, const int dob[3]);

    virtual ~Person();

    Person(const Person& other);
    Person& operator=(const Person& other);

    const std::string getFirstName() const { return firstName; }
    const std::string getLastName() const { return lastName; }
    int getID() const { return ID; }
    const int* getDateOfBirth() const { return dateOfBirth; }

    friend std::istream& operator>>(std::istream& is, Person& person);
    friend std::ostream& operator<<(std::ostream& os, const Person& person);
    friend std::istream& inputFromFile(std::istream& is, Person& person);
};

class Student : public Person {
private:
    int yearOfStudy;
public:
    Student();
    Student(const std::string& firstName, const std::string& lastName, int ID, const int dob[], int year);
    Student(const Student& other);
    Student& operator=(const Student& other);
    int getYearOfStudy() const {return yearOfStudy; }

    friend std::istream& operator>>(std::istream& is, Student& student);
    friend std::ostream& operator<<(std::ostream& os, const Student& student);
    friend std::istream& inputFromFile(std::istream& is, Student& student);
};

#endif //STUDENT_H
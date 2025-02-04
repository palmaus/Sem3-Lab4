#ifndef COMPARATORS_H
#define COMPARATORS_H

#include "Student.h"

class StudentComparator {
public:
    virtual ~StudentComparator() = default;

    virtual bool operator()(const Student& s1, const Student& s2) const = 0;
};

class CompareStudentsByID : public StudentComparator {
public:

    bool operator()(const Student& s1, const Student& s2) const override {
        return s1.getID() < s2.getID();
    }
};

class CompareStudentsByYear : public StudentComparator {
public:

    bool operator()(const Student& s1, const Student& s2) const override {
        return s1.getYearOfStudy() < s2.getYearOfStudy();
    }
};

class CompareStudentsByLastName : public StudentComparator {
public:

    bool operator()(const Student& s1, const Student& s2) const override {
        std::string lastName1 = s1.getLastName();
        std::string lastName2 = s2.getLastName();
        return lastName1 < lastName2;
    }
};

class CompareStudentsByFirstName : public StudentComparator {
public:

    bool operator()(const Student& s1, const Student& s2) const override {
        std::string firstName1 = s1.getFirstName();
        std::string firstName2 = s2.getFirstName();

        std::transform(firstName1.begin(), firstName1.end(), firstName1.begin(), ::tolower);
        std::transform(firstName2.begin(), firstName2.end(), firstName2.begin(), ::tolower);

        return firstName1 < firstName2;
    }
};

#endif //COMPARATORS_H
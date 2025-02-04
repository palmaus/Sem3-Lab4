#pragma once

#include <functional>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>

class TestRunner {
public:
    void runTest(const std::string& testName, const std::function<void()>& testFunction);
    void runTestGroup(const std::string& groupName, const std::vector<std::function<void()>>& testFunctions);

    template<typename ExceptionType>
    void expectException(const std::string& testName, std::function<void()> testFunction);
    void expectNoException(const std::string& testName, const std::function<void()> &testFunction);
    void expectOutput(const std::string& testName, std::function<void()> testFunction, const std::string& expectedOutput);

    void printResults() const;

private:
    int passedTestCount = 0;
    int failedTestCount = 0;

    void printSuccess(const std::string& message) const;
    void printError(const std::string& message) const;
    void printInfo(const std::string& message) const;
};

template<typename ExceptionType>
void TestRunner::expectException(const std::string& testName, std::function<void()> testFunction) {
    try {
        testFunction();
        printError("[FAILED] " + testName + ": Expected exception " + std::string(typeid(ExceptionType).name()) +
                   " not thrown.");
    } catch (const ExceptionType&) {
        ++passedTestCount;
        printSuccess(
                "[PASSED] " + testName + ": Exception " + std::string(typeid(ExceptionType).name()) +
                " thrown as expected.");
    } catch (const std::exception& e) {
        ++failedTestCount;
        printError("[FAILED] " + testName + + ": Unexpected exception thrown: " + std::string(e.what()));
    } catch (...) {
        ++failedTestCount;
        printError("[FAILED] " + testName + + ": Unexpected exception thrown.");
    }
}
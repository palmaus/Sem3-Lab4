#include "TestRunner.h"
#include <iostream>
#include <exception>

void TestRunner::runTest(const std::string& testName, const std::function<void()>& testFunction) {
    try {
        testFunction();
        ++passedTestCount;
        printSuccess("[PASSED] " + testName);
    } catch (const std::exception& ex) {
        ++failedTestCount;
        printError("[FAILED] " + testName + ": " + ex.what());
    } catch (...) {
        ++failedTestCount;
        printError("[FAILED] " + testName + ": Unknown exception.");
    }
}

void TestRunner::runTestGroup(const std::string& groupName, const std::vector<std::function<void()>>& testFunctions) {
    printInfo("Running test group: " + groupName);
    for (const auto& testFunction : testFunctions) {
        runTest(groupName + "::" +  __func__ , testFunction);
    }
    printInfo("Test group completed: " + groupName);
}

void TestRunner::expectNoException(const std::string& testName, const std::function<void()> &testFunction) {
    try {
        testFunction();
        ++passedTestCount;
        printSuccess("[PASSED] " + testName + ": No exception thrown as expected.");
    } catch (const std::exception& e) {
        ++failedTestCount;
        printError("[FAILED] " + testName +  + ": Unexpected exception thrown: " + std::string(e.what()));
    } catch (...) {
        ++failedTestCount;
        printError("[FAILED] " + testName +  + ": Unexpected exception thrown.");
    }
}

void TestRunner::expectOutput(const std::string& testName, std::function<void()> testFunction, const std::string& expectedOutput) {
    std::stringstream ss;
    // Сохраняем старые буферы вывода и ошибок
    std::streambuf *oldCout = std::cout.rdbuf();
    std::streambuf *oldCerr = std::cerr.rdbuf();
    // Перенаправляем cout и cerr в stringstream
    std::cout.rdbuf(ss.rdbuf());
    std::cerr.rdbuf(ss.rdbuf());
    // Выполняем тестовую функцию
    testFunction();
    // Восстанавливаем старые буферы
    std::cout.rdbuf(oldCout);
    std::cerr.rdbuf(oldCerr);
    std::string output = ss.str();
    output.erase(std::remove(output.begin(), output.end(), '\r'), output.end()); // Удаляем \r
    if (output != expectedOutput) {
        ++failedTestCount;
        printError("[FAILED] " + testName + ": Unexpected CLI output: '" + ss.str() + "' Expected: '" + expectedOutput + "'");
    } else {
        ++passedTestCount;
        printSuccess("[PASSED] " + testName + ": CLI output matches expected.");
    }
}

void TestRunner::printResults() const {
    std::cout << "\nTest results: " << passedTestCount << " passed, "
              << failedTestCount << " failed." << std::endl;
}

void TestRunner::printSuccess(const std::string& message) const {
    std::cout << "\033[32m" << message << "\033[0m" << std::endl;
}

void TestRunner::printError(const std::string& message) const {
    std::cerr << "\033[31m" << message << "\033[0m" << std::endl;
}

void TestRunner::printInfo(const std::string& message) const {
    std::cout << "\033[36m" << message << "\033[0m" << std::endl;
}
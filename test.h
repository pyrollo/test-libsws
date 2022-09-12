/*
Test Lib SWS - Unit tests for libsws
Copyright (C) 2022 Pierre-Yves Rollo <dev@pyrollo.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef TEST_H
#define TEST_H

#include <string>

using namespace std;

#define CATCHEXCEPTIONS(TITLE)                                                 \
    catch (const std::exception & e) {                                         \
        cout << "Failed: Unexpected exception" << endl;                        \
        cout << "Message: " << e.what() << endl;                               \
        cout << "TypeId: " << typeid(e).name() << endl;                        \
        failtest(TITLE);                                                       \
    } catch (...) {                                                            \
        std::exception_ptr p = std::current_exception();                       \
        cout << "Failed: Unexpected exception" << endl;                        \
        if (p)                                                                 \
            cout << "Type: " << p.__cxa_exception_type()->name() << std::endl; \
        failtest(TITLE);                                                       \
    }

#define TESTNOEXCEPTION(TITLE, ...)                                            \
    try {                                                                      \
        TEST_TOTAL++;                                                          \
        __VA_ARGS__;                                                           \
        TEST_PASSED++;                                                         \
        passtest(TITLE);                                                       \
    }                                                                          \
    CATCHEXCEPTIONS(TITLE);

#define TESTEXCEPTION(TITLE, EXCEPTION, ...)                                   \
    try {                                                                      \
        TEST_TOTAL++;                                                          \
        __VA_ARGS__;                                                           \
        failtest(TITLE);                                                       \
    } catch (EXCEPTION) {                                                      \
        TEST_PASSED++;                                                         \
        passtest(TITLE);                                                       \
    }                                                                          \
    CATCHEXCEPTIONS(TITLE);

#define TESTTRUE(TITLE, ...)                                                   \
    try {                                                                      \
        TEST_TOTAL++;                                                          \
        auto fct = [&]() {                                                     \
            __VA_ARGS__;                                                       \
        };                                                                     \
        if (!fct()) {                                                          \
            failtest(TITLE);                                                   \
        } else {                                                               \
            TEST_PASSED++;                                                     \
            passtest(TITLE);                                                   \
        }                                                                      \
    }                                                                          \
    CATCHEXCEPTIONS(TITLE);

#define TEST(TITLE, ...) \
    { \
        cout << "Test: " << TITLE << endl; \
        unsigned int TEST_TOTAL = 0; \
        unsigned int TEST_PASSED = 0; \
        __VA_ARGS__; \
        if (TEST_PASSED == TEST_TOTAL) \
            cout << "[\033[32mSUCCESS\033[0m] All " << TEST_TOTAL << " test passed." << endl; \
        else \
            cout << "[\033[31mFAILED \033[0m] Failed " << TEST_TOTAL - TEST_PASSED << " of " << TEST_TOTAL << " tests." << endl; \
        cout << endl; \
    }

void failtest(std::string title);
void passtest(std::string title);

#endif // TEST_H

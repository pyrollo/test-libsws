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

using namespace std;

template <class EXCEPTION>
bool testException(void fct())
{
    try {
        fct();
        cout << "Failed: No exception" << endl;
        return false;
    } catch (EXCEPTION) {
        return true;
    } catch (std::exception e) {
        cout << "Failed: Unexpected exception" << endl;
        cout << "Message: " << e.what() << endl;
        cout << "Type: " << typeid(e).name() << endl;
        return false;
    } catch (...) {
        std::exception_ptr p = std::current_exception();
        cout << "Failed: Unexpected exception" << endl;
        if (p)
            cout << "Type: " << p.__cxa_exception_type()->name() << std::endl;
        return false;
    };
}

bool testNoException(bool fct());
bool testNoException(void fct());
void test(std::string title, bool result);

#endif // TEST_H

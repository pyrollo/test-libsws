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

#include <iostream>
#include "sws.h"
#include "test.h"

void testModuleHierarchy()
{
    std::cout << "test module hierarchy" << std::endl;

    test("Create basic module at root level",
        testNoException([] {
            sws::newModule("myadd", "add");
        })
    );

    test("Cannot create module at same level with same name",
        testException<sws::duplicate_name>([] {
            sws::newModule("myadd", "add");
        })
    );

    test("Cannot create module as basic module child",
        testException<sws::illegal_operation>([] {
            sws::newModule("myadd/myadd2", "add");
        })
    );

    test("Create container module at root level",
        testNoException([] {
            sws::newModule("container", "container");
        })
    );

    test("Create basic module in container",
        testNoException([] {
            sws::newModule("container/myadd", "add");
        })
    );

    test("Cannont create basic module in non existing container",
        testException<sws::unknown_module>([] {
            sws::newModule("containerZ/myadd", "add");
        })
    );

    test("Create container in container",
        testNoException([] {
            sws::newModule("container/container", "container");
        })
    );

    test("Create basic module in container in container",
        testNoException([] {
            sws::newModule("container/container/myadd", "add");
        })
    );
}

int main()
{
    testModuleHierarchy();

    return 0;
}

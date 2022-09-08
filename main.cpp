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
#include "swsengine.h"
#include "test.h"

void testModuleHierarchy()
{
    TEST("Module hierarchy",

        swsEngine sws;

        TESTNOEXCEPTION("Create basic module at root level",
            sws.newModule("myadd", "add");
        );

        TESTEXCEPTION("Cannot create module at same level with same name",
            sws::duplicate_name,
            sws.newModule("myadd", "add");
        );

        TESTEXCEPTION("Cannot create module as basic module child",
            sws::unknown_schema,
            sws.newModule("myadd/myadd2", "add");
        );

        TESTNOEXCEPTION("Create container module at root level",
            sws.newModule("container", "container");
        );

        TESTNOEXCEPTION("Create basic module in container",
            sws.newModule("container/myadd", "add");
        );

        TESTEXCEPTION("Cannont create basic module in non existing container",
            sws::unknown_schema,
            sws.newModule("containerZ/myadd", "add");
        );

        TESTNOEXCEPTION("Create container in container",
            sws.newModule("container/container", "container");
        );

        TESTNOEXCEPTION("Create basic module in container in container",
            sws.newModule("container/container/myadd", "add");
        );
    );
}

void testModuleDeletion()
{
    TEST("Module deletion",

        swsEngine sws;

        TESTNOEXCEPTION("Create fixtures",
            sws.newModule("myadd", "add");
            sws.newModule("myemptycontainer", "container");
            sws.newModule("myonelevelcontainer", "container");
            sws.newModule("myonelevelcontainer/myadd", "add");
            sws.newModule("myonelevelcontainer/myadd2", "add");
            sws.newModule("myonelevelcontainer/myadd3", "add");
            sws.newModule("mycontainer", "container");
            sws.newModule("mycontainer/mycontainer2", "container");
            sws.newModule("mycontainer/mycontainer2/myadd", "add");
            sws.newModule("mycontainer/mycontainer2/myadd2", "add");
            sws.newModule("mycontainer/mycontainer2/myadd3", "add");
        );

        TESTNOEXCEPTION("Delete an existing base module at root",
            sws.deleteModule("myadd");
        );

        TESTNOEXCEPTION("Delete an existing base module in container",
            sws.deleteModule("mycontainer/mycontainer2/myadd");
        );

        TESTEXCEPTION("Cannot delete non existing module at root",
            sws::unknown_module,
            sws.deleteModule("nonexistant");
        );

        TESTEXCEPTION("Cannot delete non existing module in container",
            sws::unknown_module,
            sws.deleteModule("mycontainer/mycontainer2/nonexistant");
        );

        TESTNOEXCEPTION("Delete an empty container",
            sws.deleteModule("myemptycontainer");
        );

        TESTNOEXCEPTION("Delete an one level container",
            sws.deleteModule("myonelevelcontainer");
        );

        TESTNOEXCEPTION("Delete an several level container",
            sws.deleteModule("mycontainer");
        );
    );
}

int main()
{
    testModuleHierarchy();
    testModuleDeletion();

    return 0;
}

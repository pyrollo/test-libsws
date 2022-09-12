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
void testConnecting()
{
    TEST("Connecting",
        swsEngine sws;

        TESTNOEXCEPTION("Create fixtures",
            sws.newModule("add1", "add");
            sws.newModule("add2", "add");
            sws.newModule("add3", "add");
            sws.newModule("container", "container");
            sws.newModule("container/add", "add");
        );

        TESTEXCEPTION("Cannot connect input to input",
            sws::illegal_connection,
            sws.connect("add1#op1", "add2#op1");
        );

        TESTEXCEPTION("Cannot connect output to output",
            sws::illegal_connection,
            sws.connect("add1#result", "add2#result");
        );

        TESTEXCEPTION("Cannot connect between different schemas",
            sws::illegal_connection,
            sws.connect("add1#result", "container/add#op1");
        );

        TESTEXCEPTION("Cannot connect interconnected module to itself",
            sws::illegal_connection,
            sws.connect("add1#result", "add1#op1");
        );

        TESTEXCEPTION("Cannot connect interconnected module to itself (reverse)",
            sws::illegal_connection,
            sws.connect("add1#op1", "add1#result");
        );

        TESTNOEXCEPTION("Can connect input to output",
            sws.connect("add1#op1", "add2#result");
        );

        TESTNOEXCEPTION("Can connect output to input",
            sws.connect("add3#result", "add2#op1");
        );

        TESTEXCEPTION("Cannot create circular connection",
            sws::illegal_connection,
            sws.connect("add3#op1", "add1#result");
        );

        TESTEXCEPTION("Cannot create circular connection (reverse)",
            sws::illegal_connection,
            sws.connect("add1#result", "add3#op1");
        );

        TESTEXCEPTION("Cannot connect same input twice",
            sws::already_connected,
            sws.connect("add1#op1", "add3#result");
        );

        TESTNOEXCEPTION("Can connect same output twice",
            sws.connect("add1#op2", "add2#result");
        );

    );
}

void testBasicSchema()
{
    TEST("Basic schema",
        swsEngine sws;

        TESTNOEXCEPTION("Create fixtures",
            sws.newModule("val1", "value");
            sws.newModule("val2", "value");
            sws.newModule("val3", "value");
            sws.newModule("add1", "add");
            sws.newModule("add2", "add");
            sws.connect("val3#value",  "add2#op1");
            sws.connect("add1#result", "add2#op2");
            sws.connect("val1#value",  "add1#op1");
            sws.connect("val2#value",  "add1#op2");
        );

        TESTTRUE("Step 1",
            sws.set("val1#value", 1);
            sws.set("val2#value", 2);
            sws.set("val3#value", 3);
            sws.step();
            return sws.get("add2#result") == 6;
        );

        TESTTRUE("Step 2",
            sws.set("val1#value", 5);
            sws.step();
            return sws.get("add2#result") == 10;
        );
    );
}

int main()
{
    testModuleHierarchy();
    testModuleDeletion();
    testConnecting();
    testBasicSchema();

    return 0;
}

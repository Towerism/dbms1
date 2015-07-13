CC=g++ -std=c++11
IFLAGS=-I/opt/csw/include/boost/ -IC:/boost/
OFLAGS=
OBJECTS=sql_query.o sql_command_simple.o sql_command_create.o sql_command_update.o sql_command_delete.o sql_command_insert.o sql_command_complex.o sql_command.o sql_parser.o main.o
all : sql_parser
sql_parser: $(OBJECTS)
	$(CC) $(OBJECTS) -o sql_parser
sql_query.o: subgrammar/sql_query.cpp subgrammar/sql_query.h subgrammar/sql_basic.h subgrammar/sql_semantics.h
	$(CC) $(IFLAGS) $(OFLAGS) -c subgrammar/sql_query.cpp
sql_command.o: subgrammar/sql_command.cpp subgrammar/sql_command.h subgrammar/sql_basic.h subgrammar/sql_query.h subgrammar/sql_semantics.h
	$(CC) $(IFLAGS) $(OFLAGS) -c subgrammar/sql_command.cpp
sql_command_simple.o: subgrammar/sql_command_simple.cpp subgrammar/sql_command.h subgrammar/sql_basic.h subgrammar/sql_query.h subgrammar/sql_semantics.h
	$(CC) $(IFLAGS) $(OFLAGS) -c subgrammar/sql_command_simple.cpp
sql_command_create.o: subgrammar/sql_command_create.cpp subgrammar/sql_command.h subgrammar/sql_basic.h subgrammar/sql_query.h subgrammar/sql_semantics.h
	$(CC) $(IFLAGS) $(OFLAGS) -c subgrammar/sql_command_create.cpp
sql_command_update.o: subgrammar/sql_command_update.cpp subgrammar/sql_command.h subgrammar/sql_basic.h subgrammar/sql_query.h subgrammar/sql_semantics.h
	$(CC) $(IFLAGS) $(OFLAGS) -c subgrammar/sql_command_update.cpp
sql_command_delete.o: subgrammar/sql_command_update.cpp subgrammar/sql_command.h subgrammar/sql_basic.h subgrammar/sql_query.h subgrammar/sql_semantics.h
	$(CC) $(IFLAGS) $(OFLAGS) -c subgrammar/sql_command_delete.cpp
sql_command_insert.o: subgrammar/sql_command_update.cpp subgrammar/sql_command.h subgrammar/sql_basic.h subgrammar/sql_query.h subgrammar/sql_semantics.h
	$(CC) $(IFLAGS) $(OFLAGS) -c subgrammar/sql_command_insert.cpp
sql_command_complex.o: subgrammar/sql_command_complex.cpp subgrammar/sql_command.h subgrammar/sql_basic.h subgrammar/sql_query.h subgrammar/sql_semantics.h
	$(CC) $(IFLAGS) $(OFLAGS) -c subgrammar/sql_command_complex.cpp
sql_parser.o: sql_parser.cpp sql_parser.h subgrammar/sql_basic.h subgrammar/sql_query.h subgrammar/sql_command.h subgrammar/sql_semantics.h
	$(CC) $(IFLAGS) $(OFLAGS) -c sql_parser.cpp
main.o: main.cpp sql_parser.h subgrammar/sql_basic.h subgrammar/sql_query.h subgrammar/sql_command.h subgrammar/sql_semantics.h
	$(CC) $(IFLAGS) $(OFLAGS) -c main.cpp

.PHONY: clean
clean:
	rm -rf *.o sql_parser sql_parser.exe

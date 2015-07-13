CC=g++ -std=c++11
FPIC1=
FPIC2=
IFLAGS=-I/opt/csw/include/boost/ -IC:/boost/
OFLAGS=
OBJ=attribute.o attribute_list.o database_manager.o relation.o \
    sql_query.o sql_command_simple.o sql_command_create.o \
    sql_command_update.o sql_command_delete.o \
    sql_command_insert.o sql_command_complex.o sql_command.o \
    sql_parser.o main.o
all : dbms_shell
so : set_fpic1 set_fpic2 dbms_so
dbms_so: $(OBJ)
	$(CC) $(OBJ) -shared -o libdbms.so -lc
dbms_shell: $(OBJ)
	$(CC) $(OBJ) -o dbms
attribute.o: attribute.cpp attribute.h util.h ../sqlparser/subgrammar/sql_semantics.h
	$(CC) $(IFLAGS) $(OFLAGS) $(FPIC1) -c attribute.cpp
attribute_list.o: attribute_list.cpp attribute_list.h attribute.h ../sqlparser/subgrammar/sql_semantics.h
	$(CC) $(IFLAGS) $(OFLAGS) $(FPIC1) -c attribute_list.cpp
database_manager.o: database_manager.cpp database_manager.h relation.h attribute_list.h ../sqlparser/sql_parser.h
	$(CC) $(IFLAGS) $(OFLAGS) $(FPIC1) -c database_manager.cpp
relation.o: relation.cpp relation.h attribute_list.h ../sqlparser/subgrammar/sql_semantics.h
	$(CC) $(IFLAGS) $(OFLAGS) $(FPIC1) -c relation.cpp
sql_query.o: ../sqlparser/subgrammar/sql_query.cpp ../sqlparser/subgrammar/sql_query.h ../sqlparser/subgrammar/sql_basic.h ../sqlparser/subgrammar/sql_semantics.h
	$(CC) $(IFLAGS) $(OFLAGS) $(FPIC1) -c ../sqlparser/subgrammar/sql_query.cpp
sql_command.o: ../sqlparser/subgrammar/sql_command.cpp ../sqlparser/subgrammar/sql_command.h ../sqlparser/subgrammar/sql_basic.h ../sqlparser/subgrammar/sql_query.h ../sqlparser/subgrammar/sql_semantics.h
	$(CC) $(IFLAGS) $(OFLAGS) $(FPIC1) -c ../sqlparser/subgrammar/sql_command.cpp
sql_command_simple.o: ../sqlparser/subgrammar/sql_command_simple.cpp ../sqlparser/subgrammar/sql_command.h ../sqlparser/subgrammar/sql_basic.h ../sqlparser/subgrammar/sql_query.h ../sqlparser/subgrammar/sql_semantics.h
	$(CC) $(IFLAGS) $(OFLAGS) $(FPIC1) -c ../sqlparser/subgrammar/sql_command_simple.cpp
sql_command_create.o: ../sqlparser/subgrammar/sql_command_create.cpp ../sqlparser/subgrammar/sql_command.h ../sqlparser/subgrammar/sql_basic.h ../sqlparser/subgrammar/sql_query.h ../sqlparser/subgrammar/sql_semantics.h
	$(CC) $(IFLAGS) $(OFLAGS) $(FPIC1) -c ../sqlparser/subgrammar/sql_command_create.cpp
sql_command_update.o: ../sqlparser/subgrammar/sql_command_update.cpp ../sqlparser/subgrammar/sql_command.h ../sqlparser/subgrammar/sql_basic.h ../sqlparser/subgrammar/sql_query.h ../sqlparser/subgrammar/sql_semantics.h
	$(CC) $(IFLAGS) $(OFLAGS) $(FPIC1) -c ../sqlparser/subgrammar/sql_command_update.cpp
sql_command_delete.o: ../sqlparser/subgrammar/sql_command_delete.cpp ../sqlparser/subgrammar/sql_command.h ../sqlparser/subgrammar/sql_basic.h ../sqlparser/subgrammar/sql_query.h ../sqlparser/subgrammar/sql_semantics.h
	$(CC) $(IFLAGS) $(OFLAGS) $(FPIC2) -c ../sqlparser/subgrammar/sql_command_delete.cpp
sql_command_insert.o: ../sqlparser/subgrammar/sql_command_insert.cpp ../sqlparser/subgrammar/sql_command.h ../sqlparser/subgrammar/sql_basic.h ../sqlparser/subgrammar/sql_query.h ../sqlparser/subgrammar/sql_semantics.h
	$(CC) $(IFLAGS) $(OFLAGS) $(FPIC2) -c ../sqlparser/subgrammar/sql_command_insert.cpp
sql_command_complex.o: ../sqlparser/subgrammar/sql_command_complex.cpp ../sqlparser/subgrammar/sql_command.h ../sqlparser/subgrammar/sql_basic.h ../sqlparser/subgrammar/sql_query.h ../sqlparser/subgrammar/sql_semantics.h
	$(CC) $(IFLAGS) $(OFLAGS) $(FPIC1) -c ../sqlparser/subgrammar/sql_command_complex.cpp
sql_parser.o: ../sqlparser/sql_parser.cpp ../sqlparser/sql_parser.h ../sqlparser/subgrammar/sql_basic.h ../sqlparser/subgrammar/sql_query.h ../sqlparser/subgrammar/sql_command.h ../sqlparser/subgrammar/sql_semantics.h
	$(CC) $(IFLAGS) $(OFLAGS) $(FPIC1) -c ../sqlparser/sql_parser.cpp
main.o: main.cpp database_manager.h
	$(CC) $(IFLAGS) $(OFLAGS) $(FPIC1) -c main.cpp

.PHONY: clean set_fpic1 set_fpic2
clean:
	rm -rf *.o *.so *.so.* dbms dbms.exe
set_fpic1:
	$(eval FPIC1 := -fPIC)
set_fpic2:
	$(eval FPIC2 := -fpic)

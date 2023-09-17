#ifndef INSTRUCTIONS_H_INCLUDED
#define INSTRUCTIONS_H_INCLUDED

#include <map>
#include <cstdlib>
#include <vector>
#include <iostream>

void error_out(int line_num, std::string msg);
void reg_error(int line_num);
void invalid_instruction_error(int line_num);
void offset_val_erroir(int line_num);
void value_error(int line_num);
void value_range_error(int line_num);
void invalid_vector_error(int line_num);
int get_imm_value(std::string val);
bool validate_instruction(std::string symbol);

int execute_instruction(std::vector<std::string> row, int k, std::map<std::string, int> symbol_table, int mem_start);
#endif

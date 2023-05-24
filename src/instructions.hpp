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

std::string get_reg_code(std::string reg);
int calculate_offet(int mem_start, int dest_address, int k);
bool key_exists(std::map<std::string, int> map, std::string key);
int get_imm_value(std::string val);
std::vector<int> get_dest_val(std::map<std::string, int> symbol_table, std::string token, int line_num);

std::string add_and_not_func (std::vector<std::string> row, int line_num, int k, std::map<std::string, int> symbol_table, int start, std::string mach_code, int mem_start);
std::string br_func (std::vector<std::string> row, int line_num, int k, std::map<std::string, int> symbol_table, int start, std::string mach_code, int mem_start);
std::string jmp_func (std::vector<std::string> row, int line_num, int k, std::map<std::string, int> symbol_table, int start, std::string mach_code, int mem_start);
std::string jsr_jsrr_func (std::vector<std::string> row, int line_num, int k, std::map<std::string, int> symbol_table, int start, std::string mach_code, int mem_start);
std::string ld_ldi_lea_st_sti_func (std::vector<std::string> row, int line_num, int k, std::map<std::string, int> symbol_table, int start, std::string mach_code, int mem_start);
std::string ret_rti_func (std::vector<std::string> row, int line_num, int k, std::map<std::string, int> symbol_table, int start, std::string mach_code, int mem_start);
std::string ldr_str_func (std::vector<std::string> row, int line_num, int k, std::map<std::string, int> symbol_table, int start, std::string mach_code, int mem_start);
std::string trap_func (std::vector<std::string> row, int line_num, int k, std::map<std::string, int> symbol_table, int start, std::string mach_code, int mem_start);
std::string fill_func (std::vector<std::string> row, int line_num, int k, std::map<std::string, int> symbol_table, int start, std::string mach_code, int mem_start);
std::string halt_func (std::vector<std::string> row, int line_num, int k, std::map<std::string, int> symbol_table, int start, std::string mach_code, int mem_start);

bool validate_instruction(std::string symbol);

std::string execute_instruction(std::vector<std::string> row, int k, std::map<std::string, int> symbol_table, int mem_start);
#endif

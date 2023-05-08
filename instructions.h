#include <cstdlib>
#include <map>
#include <functional>
#include <iostream>
#include <regex>




void error_out (int line_num, std::string msg) {
  throw std::runtime_error("Line " + std::to_string(line_num+1) + " => " + msg);
}

void reg_error (int line_num) {
  error_out(line_num, "Invalid register code");
}

void invalid_instruction_error(int line_num) {
  error_out(line_num, "Invalid instruction");
}

void offset_val_error(int line_num) {
  error_out(line_num, "Offset value out of range (9 bit twos complement)");
}


std::string get_reg_code (std::string reg) { //returns binary string of register code or "" if not valid code
  std::regex regex("^R[0-7]$");
  if (std::regex_match(reg, regex)) {
    std::string x = reg.substr(1);
    std::bitset<3> binary(std::stoi(x)-1);
    return binary.to_string();
  } else {
    return "";
  }

}


std::string add_and_func (std::vector<std::string> row, int k, int skipped, std::map<std::string, int> symbol_table, int start, std::string mach_code, int mem_start) {

  std::string reg_code;
  
  if (row[start]== "ADD") {
    mach_code += "0001"; 
  } 
  else {
    mach_code += "0101";
  }
  reg_code = get_reg_code(row[start+1]);
  if (reg_code == "") {
    reg_error(k+skipped);
  }
  mach_code += reg_code;
  reg_code = get_reg_code(row[start+2]);
  if(reg_code == "") {
    reg_error(k+skipped);
  }
  mach_code += reg_code;
  int imm = 0;
  if (row[start+3][0] == '#') {
    reg_code = row[start+3];
    reg_code = reg_code.substr(1);
    try {
      imm = std::stoi(reg_code);
    } catch (const std::exception& e) {
      error_out(k+skipped, "Invalid immediate value");
    }
    if (imm > 0b11111) {
      error_out(k+skipped, "Immediate value out of bounds; Needs to be less than 2^5");
    }
    std::bitset<5> b(imm);
    std::string binary_str = b.to_string();
    mach_code += "1";
    mach_code += binary_str;
    return mach_code;
  }
  reg_code = get_reg_code(row[start+2]);

  if (reg_code == "") {
    reg_error(k+skipped);
  }
  mach_code += "000";
  mach_code += reg_code;
  return mach_code;
  
}

std::string br_func (std::vector<std::string> row, int k, int skipped, std::map<std::string, int> symbol_table, int start, std::string mach_code, int mem_start) {
  
  if (!symbol_table.count(row[start+1])) {
    invalid_instruction_error(k+skipped);
  }
  //std::cout << "here"; 
  mach_code += "0000";
  std::bitset<3> con_codes ("000");
  
  for (int i = 2; i < row[start].length(); i++) {
    
    if (row[start][i] == 'n') {
      con_codes[0] = "1";
    }
    if (row[start][i] == 'z') {
      con_codes[1] = "1";
    }
    if (row[start][i] == 'p') {
      con_codes[2] = "1";
    }
    std::cout << con_codes << std::endl;

  }
  mach_code += con_codes.to_string();
  int jump_val = symbol_table[row[start+1]];
  int offset_val = jump_val - (mem_start + k);
  //std::cout << offset_val << " " << jump_val <<  " " << mem_start << " " << line << std::endl;
  if (offset_val > 255 || offset_val < -256) {
    offset_val_error(k+skipped);
  }
  std::bitset<9> bits(offset_val);
  mach_code += bits.to_string();
  std::cout << bits.to_string() << std::endl;
  return mach_code;
  
} 



bool validate_instruction(std::string symbol) {
  std::regex regex("^(\\.(ORIG|END|FILL|BLKW|STRINGZ)|ADD|AND|BR(n?z?p?|n?p|z?p)|JMP(R|RR)?|JSR(R)?|LD(R)?|LDI(R)?|LEA|NOT|RET|RTI|ST(R)?|STI(R)?|STR|TRAP(x?0?|x21|x22|x23|x25|x31|x33))$");
  return std::regex_match(symbol, regex);
}



        std::cout << cur_address << std::endl;
std::string execute_instruction(std::vector<std::string> row, int k, int skipped, std::map<std::string, int> symbol_table, int mem_start) {
  std::map<std::string, std::function<std::string(std::vector<std::string>, int, int, std::map<std::string, int>, int, std::string, int)>> function_map = {
    {"ADD", &add_and_func},
    {"AND", &add_and_func},
    {"BR", &br_func}
  };
  int line = skipped+k;
  int start = 0;
  if (!validate_instruction(row[0])) {
    start = 1;
  }
  std::string token = row[start];
  if (!validate_instruction(token)) {
    std::cout << token << std::endl;
    invalid_instruction_error(line);
  }
  if (token.substr(0, 2) == "BR") {
    token = "BR";
  }
  auto func = function_map[token];
  std::string result = func(row, k, skipped, symbol_table, start, "", mem_start);
  return result;
}

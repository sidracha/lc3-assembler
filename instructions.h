#include <cstdlib>
#include <map>
#include <functional>
#include <iostream>
#include <regex>
#include <cmath>




void error_out (int line_num, std::string msg) {
  throw std::runtime_error("Line " + std::to_string(line_num) + " => " + msg);
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

void value_error(int line_num) {
  error_out(line_num, "Invalid value");
}

void value_range_error(int line_num) {
  error_out(line_num, "Value out of range");
}

void invalid_vector_error(int line_num) {
  error_out(line_num, "Invalid vector");
}


std::string get_reg_code (std::string reg) { //returns binary string of register code or "" if not valid code
  std::regex regex("^R[0-7]$");
  if (std::regex_match(reg, regex)) {
    std::string x = reg.substr(1);
    std::bitset<3> binary(std::stoi(x));
    return binary.to_string();
  } else {
    return "";
  }

}

int calculate_offset(int mem_start, int dest_address, int k) {
  return dest_address - (mem_start + k) - 1; //imcremented PC + offset
}

std::string int_to_bin9_str (int num) {
  std::bitset<9> b(num);
  return b.to_string();
}

bool key_exists (std::map<std::string, int> map, std::string key) {
  if (!map.count(key)) {
    return false;
  }
  return true;
}

int get_imm_value (std::string val) {
  int x;
  if (val.substr(0, 2) == "0x") {
    x = std::stoi(val, 0, 16);
  }
  if (val[0] == 'x') {
    x = std::stoi(val.substr(1) , 0, 16);
  }
  if (val[0] == '#') {
    x = std::stoi(val.substr(1));
  }
  return x;
}


std::vector<int> get_dest_val (std::map<std::string, int> symbol_table, std::string token, int line_num) {
  std::vector<int> return_vect; //if immediate, 0, if symbol, 1
  if (!(token[0] == '#' || token[0] == 'x' || token.substr(0, 2) == "0x")) {
    if (!(key_exists(symbol_table, token))) {
      value_error(line_num);
    }
    return_vect = {1, symbol_table[token]};
  }
  else {
    return_vect = {0, get_imm_value(token)};

  }
  return return_vect;
}

std::string add_and_not_func (std::vector<std::string> row, int line_num, int k, std::map<std::string, int> symbol_table, int start, std::string mach_code, int mem_start) {
  //std::cout << "here" << std::endl;
  std::string reg_code;
  
  if (row[start] == "ADD") {
    mach_code += "0001"; 
  } 
  if (row[start] == "AND"){
    mach_code += "0101";
  }
  if (row[start] == "NOT") {
    mach_code += "1001";
  }
  reg_code = get_reg_code(row[start+1]);
  if (reg_code == "") {
    reg_error(line_num);
  }
  mach_code += reg_code;
  reg_code = get_reg_code(row[start+2]);
  if(reg_code == "") {
    reg_error(line_num);
  }
  mach_code += reg_code;
  if (row[start] == "NOT") {
    if (row.size() > 5) {
      invalid_instruction_error(line_num);
    }
    mach_code += "111111";
    return mach_code;
  }
  int imm = 0;
  if (row[start+3][0] == '#' || row[start+3][0] == 'x' || row[start+3].substr(0, 2) == "0x") {
    //std::cout << "here" << std::endl;
    int imm = get_imm_value(row[start+3]);
    if (imm > 15 || imm < -16) {
      //error_out(k+skipped, "Immediate value out of bounds; Needs to be less than 2^5");
      value_range_error(line_num);
    }
    std::bitset<5> b(imm);
    std::string binary_str = b.to_string();
    mach_code += "1";
    mach_code += binary_str;
    return mach_code;
  }
  reg_code = get_reg_code(row[start+3]);

  if (reg_code == "") {
    reg_error(line_num);
  }
  mach_code += "000";
  mach_code += reg_code;
  return mach_code;
  
}

std::string br_func (std::vector<std::string> row, int line_num, int k, std::map<std::string, int> symbol_table, int start, std::string mach_code, int mem_start) {
  if (row[start].length() == 2) {
    invalid_instruction_error(line_num);
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
  
  std::vector<int> dest_val_vect = get_dest_val(symbol_table, row[start+1], line_num);
  
  int offset_val;
  if (dest_val_vect[0] == 0) {
    offset_val = dest_val_vect[1];
  } else {
    offset_val = calculate_offset(mem_start, dest_val_vect[1], k);
  }

  //std::cout << offset_val << " " << jump_val <<  " " << mem_start << " " << line << std::endl;
  if (offset_val > 255 || offset_val < -256) {
    offset_val_error(line_num);
  }
  mach_code += int_to_bin9_str(offset_val);
  //std::cout << bits.to_string() << std::endl;
  return mach_code;
  
}

std::string jmp_func (std::vector<std::string> row, int line_num, int k, std::map<std::string, int> symbol_table, int start, std::string mach_code, int mem_start) {
  if (row.size() > 4) {
    invalid_instruction_error(line_num);
  }

  std::string reg_code = get_reg_code(row[start+1]);
  if (reg_code == "") {
    invalid_instruction_error(line_num);
  }
  mach_code += "1100";
  mach_code += "000";
  mach_code += reg_code;
  mach_code += "000000";
  return mach_code;
}

std::string jsr_jsrr_func (std::vector<std::string> row, int line_num, int k, std::map<std::string, int> symbol_table, int start, std::string mach_code, int mem_start) {
  

  mach_code += "0100";
  if (row[start] == "JSR") {//PC offset
    std::vector<int> dest_val_vect = get_dest_val(symbol_table, row[start+1], line_num);
    int dest_val;
    if (dest_val_vect[0] == 0) {
      //std::cout << "here" << std::endl;
      dest_val = dest_val_vect[1];
    } else {
      std::cout << "here" << std::endl;
      dest_val = calculate_offset(mem_start, dest_val_vect[1], k);
      std::cout << dest_val << std::endl;
    }

    mach_code += "1";
    std::bitset<11> b(dest_val);
    mach_code += b.to_string();
    return mach_code;

  }
  //JSRR
  mach_code += "000";
  std::string reg_code = get_reg_code(row[start+1]);
  if (reg_code == "") {
    reg_error(line_num); 
  }
  mach_code += (reg_code + "000000");
  return mach_code;

}

std::string ld_ldi_lea_st_sti_func (std::vector<std::string> row, int line_num, int k,  std::map<std::string, int> symbol_table, int start, std::string mach_code, int mem_start) {
  std::map<std::string, std::string> code_map = {
    {"LD", "0010"},
    {"LDI", "1010"},
    {"LEA", "1110"},
    {"ST", "0011"},
    {"STI", "1011"}
  };

  mach_code += code_map[row[start]];
  std::string reg_code = get_reg_code(row[start+1]);
  if (reg_code == "") {
    reg_error(line_num);
  }
  mach_code += reg_code;
  

  std::vector<int> dest_val_vect = get_dest_val(symbol_table, row[start+2], line_num);
  int offset_val;
  if (dest_val_vect[0] == 0) {
    offset_val = dest_val_vect[1];
  } else {
    offset_val = calculate_offset(mem_start, dest_val_vect[1], k);
  }

  std::string offset = int_to_bin9_str(offset_val);
  mach_code += offset;
  return mach_code;
}

std::string ret_rti_func (std::vector<std::string> row, int line_num, int k, std::map<std::string, int> symbol_table, int start, std::string mach_code, int mem_start) {
  
  if (row[start] == "RET") {
    mach_code += "1100";
    mach_code += "000111000000";
    return mach_code;
  }
  mach_code += "1000";
  mach_code += "000000000000";
  return mach_code;
}

std::string ldr_str_func (std::vector<std::string> row, int line_num, int k, std::map<std::string, int> symbol_table, int start, std::string mach_code, int mem_start) {
  

  int offset;
  std::vector<int> dest_val_vect = get_dest_val(symbol_table, row[start+3], line_num);
  if (dest_val_vect[0] == 0) {
    offset = dest_val_vect[1];
  } else {
    offset = calculate_offset(mem_start, dest_val_vect[1], k);
  }


  if (offset > 31 || offset < -32) {
    value_range_error(line_num); 
  }
  

  if (row[start] == "LDR") {
    mach_code += "0110";  
  } 
  else { //str
    mach_code += "0111";
  }

  std::string reg_code = get_reg_code(row[start+1]);
  if (reg_code == "") {
    reg_error(line_num);
  }
  mach_code += reg_code;
  reg_code = get_reg_code(row[start+2]);
  if (reg_code == "") {
    reg_error(line_num);
  }
  mach_code += reg_code;
  std::bitset<6> b(offset);
  mach_code += b.to_string();
  return mach_code;


}

std::string trap_func (std::vector<std::string> row, int line_num, int k, std::map<std::string, int> symbol_table, int start, std::string mach_code, int mem_start) {
  
  int vector_int;
  std::vector vect = get_dest_val(symbol_table, row[start+1], line_num);

  if (vect[0] == 0) {
    vector_int = vect[1];
  } else {
    vector_int = calculate_offset(mem_start, vect[1], k);
  }

  if (vector_int > 255 || vector_int < 0) {
    invalid_vector_error(line_num);
  }
  
  std::bitset<8> b(vector_int);
  std::string vector_code = b.to_string();
  mach_code += "11110000";
  mach_code += vector_code;
  return mach_code;

}


std::string fill_func(std::vector<std::string> row, int line_num, int k, std::map<std::string, int> symbol_table, int start, std::string mach_code, int mem_start) {
  
  int num = std::stoi(row[2]);
  if (num > 0xFFFF) {
    value_range_error(line_num); 
  }
  std::bitset<16> b(num);
  return b.to_string();

}

std::string halt_func(std::vector<std::string> row, int line_num, int k, std::map<std::string, int> symbol_table, int start, std::string mach_code, int mem_start) {
  std::string trap_vect = "0x25";
  row.push_back(trap_vect);
  mach_code = trap_func(row, line_num, k, symbol_table, start, mach_code, mem_start);

  return mach_code;
}

bool validate_instruction(std::string symbol) {
  std::regex regex("^(\\.(ORIG|END|FILL|BLKW|STRINGZ)|ADD|AND|BR(n?z?p?|n?p|z?p)|JMP(R|RR)?|JSR(R)?|LD(R)?|LDI(R)?|LEA|NOT|RET|RTI|ST(R)?|STI|STR|HALT|TRAP)$");
  return std::regex_match(symbol, regex);
}



std::string execute_instruction(std::vector<std::string> row, int k, std::map<std::string, int> symbol_table, int mem_start) {
  std::map<std::string, std::function<std::string(std::vector<std::string>, int, int, std::map<std::string, int>, int, std::string, int)>> function_map = {
    {"ADD", &add_and_not_func},
    {"AND", &add_and_not_func},
    {"BR", &br_func},
    {"JMP", &jmp_func}, 
    {"JSR", &jsr_jsrr_func},
    {"JSRR", &jsr_jsrr_func},
    {"LD", &ld_ldi_lea_st_sti_func},
    {"LDI", &ld_ldi_lea_st_sti_func},
    {"LEA", &ld_ldi_lea_st_sti_func},
    {"ST", &ld_ldi_lea_st_sti_func},
    {"STI", &ld_ldi_lea_st_sti_func},
    {"LDR", &ldr_str_func},
    {"STR", &ldr_str_func},
    {"NOT", &add_and_not_func},
    {"RET", &ret_rti_func},
    {"RTI", &ret_rti_func},
    {"TRAP", &trap_func},
    {"FILL", &fill_func},
    {"HALT", &halt_func}
  };
  int line_num = std::stoi(row[0]);
  int start = 1;

  if (!validate_instruction(row[1])) {
    start = 2;
  }
  std::string token = row[start];

  if (row[1] == "") { //ascii character
    token = "FILL";
  }
  //std::cout << token << std::endl;
  //std::cout << validate_instruction(token) << std::endl;
  if (!validate_instruction(token)) {
    //std::cout << "here" << std::endl;
    invalid_instruction_error(line_num);
  }
  if (token.substr(0, 2) == "BR") {
    token = "BR";
  }


  auto func = function_map[token];
  std::string result = func(row, line_num, k, symbol_table, start, "", mem_start);
  return result;
}

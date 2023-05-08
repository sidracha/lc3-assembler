#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <regex>
#include <cstdlib>
#include <ctype.h>

#include "instructions.h"



std::vector<std::string> line_to_tokens (std::string line) { //breaks up line into tokens

  int j = 0;
  while (j < line.length() && (line[j] == ' ' || line[j] == '\t')) { //goes to first real character
    j++;
  }

  std::vector<std::string> tokens;

  if (j == line.length() || line.length() == 0) {
    return tokens;
  }
  std::string token = "";
  bool in_quotes = false;
  for (int i = j; i < line.length(); i++) {

    if (line[i] == '"') {
      if (in_quotes) {
        in_quotes = false;
      } else { //not in quotes
        in_quotes = true;
      }
    }
    if (line[i] == ';' && !in_quotes) {
      return tokens;
    }
    if (line[i] == ' ' || line[i] == '\t' || line[i] == ',') {
      if (token == "") {
        continue;
      }
      if (!in_quotes) {
        tokens.push_back(token);
        token = "";
        continue;
      }
    }
    token += line[i];
    //std::cout << token << std::endl;
  }

  if (token != "") {
    tokens.push_back(token);
  }

  return tokens;


}

bool validate_hex_val (std::string val) {

  std::regex pattern("^x[0-9]{4}$");
  return std::regex_match(val, pattern);

}

int print_map (std::map<std::string, int> m) {

  for (const auto& [key, value] : m) {
    std::cout << key << ", " <<  value << std::endl;
  }
  return 0;
}

bool validate_label (std::string label) {
  std::regex regex("^[a-zA-Z0-9]{1,20}$");
  return std::regex_match(label, regex);
}




int assemble_and_output () {
  
  //std::cout << validate_instruction("AND") << std::endl;
  //return 0;

  std::ifstream file("text.txt");
  if (file.is_open()) {

    std::map<std::string, int> symbol_table;

    std::vector<std::vector<std::string> > instruction_vect;

    std::string line;
    size_t i = 0;
    bool orig_found = false;
    int skipped = 0;
    int mem_start = 0;
    int cur_address = 0;

    //first pass to generate symbol table and break instructions into tokens
    while(std::getline(file, line)) {


      //std::cout << "here" <<std::endl;
      std::vector<std::string> tokens = line_to_tokens(line);

      if (tokens.size() == 0) { //if empty line or commented
        skipped += 1;
        continue;
      }

      if (tokens[0] == ".END") { //if encounters end stop parsing and end program;
        break;
      }

      if (tokens[0] == ".STRINGZ" || tokens[0] == ".BLKW" || tokens[0] == ".FILL") { //these pseudo-ops need a label
        error_out(skipped+i, "pseudo-op with no label");
      }

      if (tokens[0] != ".ORIG") {

        if (!orig_found) {
          skipped += 1;
          continue;
        }

      } else {
        if (orig_found) {
          error_out(skipped+i, "Duplicate origin");
        }


        //std::cout << tokens.size() << std::endl;
        //std::cout << tokens[0] << std::endl;
        if (tokens.size() < 2) {
          error_out(i + skipped, "No start value");
        }

        if (!validate_hex_val(tokens[1])) {
          error_out(i + skipped, "Start value invalid: need to be in form x0000");
        }

        std::string subs = tokens[1].substr(1, tokens[1].length()-1);
        mem_start = std::stoi(subs, 0, 16);
        std::cout << mem_start << std::endl;
        cur_address = mem_start;
        orig_found = true;
        skipped += 1;
        continue;

      }

      //this is kinda chalked but if vector[0] = "" then vector[1] holds an ascii if vector[1] = "" then vector[0] holds an interger in string form

      if (!validate_instruction(tokens[0])) {
        std::cout << cur_address << std::endl;
        if (tokens[0][tokens[0].length()-1] == ':' ) {
          tokens[0] = tokens[0].substr(0, tokens[0].length()-1);
        }
        if (!validate_label(tokens[0])) {
          std::cout << tokens[0] << std::endl;
          error_out(i+skipped, "Invalid label");
        }

        std::vector<std::string> str_vect;

        if (tokens.size() < 3) {
          error_out(i+skipped, "");
        }
        //check if label
        if (tokens[0][tokens[0].length() - 1]  == ':') {
          tokens[0] = tokens[0].substr(0, tokens[0].length()-1);
        }


        //we know that it is a label here; we need to allocate memory for any pseudo-ops
        if (tokens[1] == ".STRINGZ") {

          if (tokens[2][0] != '"' || tokens[2][tokens[2].length()-1] != '"') { //checking if not incapsulated in quotes
            error_out(skipped+i, "Invalid string");
          }

          symbol_table[tokens[0]] = cur_address;
          for (int x = 1; x < tokens[2].length()-1; x++) {
            if (!isascii(tokens[2][x])) {
              error_out(i+skipped, "Invalid character");
            }
            int ascii_code = static_cast<int>(tokens[2][x]);
            str_vect = {"", std::to_string(ascii_code)};
            instruction_vect.push_back(str_vect); //.STRINGZ pesudo-op initializes a string in memory
          }
          str_vect = {"", "0"};
          instruction_vect.push_back(str_vect);
          cur_address += tokens[2].length()-1;
          i++;
          continue;
        }

        if (tokens[1] == ".FILL") {
          int fill_val;
          if (!validate_hex_val(tokens[2])) {
            try {
              fill_val = std::stoi(tokens[2]);
            } catch (const std::exception& e) {
              error_out(skipped+i, "Invalid value");
            }
          } else {
            std::string subs = tokens[2].substr(1);
            fill_val = std::stoi(subs, 0, 16);
          }
          std::string fill_val_str;
          fill_val_str = std::to_string(fill_val);
          symbol_table[tokens[0]] = cur_address;
          str_vect = {fill_val_str, ""};
          instruction_vect.push_back(str_vect);
          i++;
          cur_address++;
          continue;

        }
        //std::cout << tokens[2] << std::endl;
        if (tokens[1] == ".BLKW") {
          int block_count;
          if(tokens[2][0] == '#') { //allows for first index to be #
            tokens[2] = tokens[2].substr(1);
            try {
              //std::cout << tokens[2] << std::endl;
              block_count = std::stoi(tokens[2]);
            } catch (const std::exception& e) {
              error_out(skipped+i, "Invalid valueegegege");

            }
          }
          else if (tokens[2][0] == 'x') {
            try {
              tokens[2] = tokens[2].substr(2);
              block_count = std::stoi(tokens[2], 0, 16);
            } catch (const std::exception& e) {
              error_out(skipped+i, "Invalid value");
            }
          }
          else {
            try {
              block_count = std::stoi(tokens[2]);
            } catch (const std::exception& e) {
              error_out(skipped+i, "Invalid value");
            }
          }
          symbol_table[tokens[0]] = cur_address;
          for (int x = 0; x < block_count; x++) {
            //std::cout << "here\n";
            str_vect = {"0", ""};
            instruction_vect.push_back(str_vect);
            cur_address++;
          }
          i++;
          continue;

        }
        //if ()

        //std::cout << "here" <<std::endl;
        symbol_table[tokens[0]] = cur_address;
        //std::cout << tokens[0] << " " << symbol_table[tokens[0]] << std::endl;
      }

      instruction_vect.push_back(tokens);
      cur_address++;
      i++;
      if (cur_address > 0xFFFF) { //check if exceeding memory address space
        throw std::runtime_error("Address space exceeded");
      }
    }
    if (instruction_vect.size() == 0) {
      error_out(0, "No .ORIG found");
    }
    print_map(symbol_table);
    std::string mach_code = execute_instruction(instruction_vect[0], 0, skipped, symbol_table, mem_start);
    std::cout << mach_code << std::endl;
    return 0;
    //second pass starts to assmeble instructions
    //first index of the vector is the .ORIG row so we start from index
    //print_map(symbol_table);


    //for (int x = 0; x < instruction_vect.size(); x++) {
      //std::string line = "";
      //for (int k = 0; k < instruction_vect[x].size(); k++) {
        //line += (instruction_vect[x][k] + " ");
      //}
      //std::cout << line << std::endl;
    //}
    std::vector<std::string> row;
    int start;
    std::string opcode;

    std::string d_reg;
    std::string s_reg;


    for (int k = 0; k < instruction_vect.size(); k++) {

      row = instruction_vect[k];
      if (validate_instruction(row[0])) {
        start = 1;
      } else {
        start = 0;
      }
      std::string token = row[start]; //index of first instruction




    }
  }

  return 0;

}


int main () {

  assemble_and_output();

  return 0;
}

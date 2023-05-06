#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <regex>



bool is_valid_instruction (std::string symbol) {

  std::regex regex("^(\\.(ORIG|END|FILL|BLKW|STRINGZ)|ADD|AND|BR(n?z?p?|np|nz|zp)|JMP(R|RR)?|JSR(R)?|LD(R)?|LDI(R)?|LEA|NOT|RET|RTI|ST(R)?|STI(R)?|STR|TRAP(x?0?|x21|x22|x23|x25|x31|x33))$");
  return std::regex_match(symbol, regex);

}

void error_out (int line_num) {
  throw std::runtime_error("Line " + std::to_string(line_num));
}

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

  for (int i = j; i < line.length(); i++) {
  
    if (line[i] == ';') {
      return tokens;
    }
    if (line[i] == ' ' || line[i] == '\t' || line[i] == ',') {
      if (token == "") {
        continue;
      }
      tokens.push_back(token);
      token = "";
      continue;
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
 
  


int assemble_and_output () {

  std::ifstream file("example.txt");
  if (file.is_open()) {
    
    std::map<std::string, int> symbol_table;

    std::vector<std::vector<std::string> > instruction_vect;

    std::string line;
    size_t i = 0;
    bool orig_found = false;
    int skipped = 0;
    int mem_start = 0;
    

    //first pass to generate symbol table and break instructions into tokens
    while(std::getline(file, line)) {

      std::vector<std::string> tokens = line_to_tokens(line);

      if (tokens.size() == 0) { //if empty line or commented
        skipped += 1;
        continue;
      }

      if (tokens[0] == ".END") { //if encounters end stop parsing and end program;
        break;
      }

      if (tokens[0] == ".STRINGZ" || tokens[0] == ".BLKW" || tokens[0] == ".FILL") { //these pseudo-ops need a label
        error_out(skipped+i);
      }

      if (tokens[0] != ".ORIG") {
        
        if (!orig_found) {
          skipped += 1;
          continue;
        }

      } else {
        if (orig_found) {
          throw std::runtime_error("Duplicate origin");
        }
        
        //std::cout << tokens.size() << std::endl;
        //std::cout << tokens[0] << std::endl;
        if (tokens.size() < 2) {
          error_out(i + skipped);
        }
        
        if (!validate_hex_val(tokens[1])) {
          error_out(i + skipped);
        }
        
        std::string subs = tokens[1].substr(1, tokens[1].length()-1);
        mem_start = std::stoi(subs, 0, 16);

        orig_found = true;
        skipped += 1;
        continue;

      }

      if (!is_valid_instruction(tokens[0])) { //check if label
        if (tokens[0][tokens[0].length() - 1]  == ':') {
          tokens[0] = tokens[0].substr(0, tokens[0].length()-1);
        }
        symbol_table[tokens[0]] = mem_start + i;
        //std::cout << tokens[0] << " " << symbol_table[tokens[0]] << std::endl;
      }
      //
      instruction_vect.push_back(tokens);    
      i++;
    }
    
    //second pass starts to assmeble instructions
    //first index of the vector is the .ORIG row so we start from index 1
    
    std::vector<std::string> row;
    int start;
    std::string opcode;
      
    std::string d_reg;
    std::string s_reg;

    for (int k = 1; k < instruction_vect.size(); k++) {
      
      row = instruction_vect[k];
      if (!is_valid_instruction(row[0])) {
        start = 1;  
      } else {
        start = 0;
      }
      
  }
    }
  return 0;

}


int main () {

  assemble_and_output();

  return 0;
}

#include <fstream>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <set>
#include <stdexcept>

using basic_block = std::vector<nlohmann::json>;
const std::set<std::string> TERMINATORS = {"jmp", "br", "ret"};
std::map<std::string, basic_block> block_map;

std::string getBlockName(basic_block& block) {
  // Get the name of the basic block
  // If the fist instruction is a label, then 'label' name
  // is the name of the block
  // else
  // create a name a number as prefix like b{0}, b{1} etc

  static int blockcount = 0;
  auto firstInstr = block[0];
  std::string name;
  if (firstInstr.find("label") != firstInstr.end()) {
    name = firstInstr["label"];
  } else if (firstInstr.find("labels") != firstInstr.end()) {
    name = firstInstr["labels"];
  } else {
    name = "b{ " + std::to_string(blockcount++) + " }";
  }
  return name;
}

void formBlocks(const nlohmann::json& body) {
  basic_block curBlock;

  for (const auto& instr : body) {
    if (instr.find("op") != instr.end()) {  // an actual instruction
      curBlock.push_back(instr);

      // check for terminators
      if (TERMINATORS.find(instr["op"]) != TERMINATORS.end()) {
        // // Print the current block
        // for (const auto& blockInstr : curBlock) {
        //   std::cout << blockInstr << std::endl;
        // }

        // Add blocks to tne map of blocks
        block_map[getBlockName(curBlock)] = curBlock;

        curBlock.clear();
      }
    } else {  // A label
      // Print the current block
      // for (const auto& blockInstr : curBlock) {
      //   std::cout << blockInstr << std::endl;
      // }

      // Add blocks to tne map of blocks
      block_map[getBlockName(curBlock)] = curBlock;

      curBlock.clear();
      curBlock.push_back(instr);
    }
  }

  // Print any remaining instructions in the last block
  // for (const auto& blockInstr : curBlock) {
  //   std::cout << blockInstr << std::endl;
  // }
  // Add blocks to tne map of blocks
  block_map[getBlockName(curBlock)] = curBlock;
  curBlock.clear();
}

void printBlocks(){
// Print all the basic blocks in a program
  for(const auto& pair : block_map){
    std::cout << pair.first << std::endl;
    for(const auto& instr : pair.second){
      std::cout << instr << std::endl;
    }
  }
}

int main() {
  // Load JSON data from stdin
  nlohmann::json prog;
  try {
        std::cin >> prog;
    } catch (const std::ios_base::failure& e) {
        // Catch and ignore the BrokenPipeError
        if (e.code() != std::io_errc::stream) {
            throw; // Re-throw if it's not a BrokenPipeError
        }
    }
  // std::ifstream f("prog.json");
  // prog = nlohmann::json::parse(f);

  for (const auto& func : prog["functions"]) {
    formBlocks(func["instrs"]);
  }

  printBlocks();

  return 0;
}

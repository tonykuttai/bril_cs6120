#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <nlohmann/json.hpp>
#include <set>
#include <stdexcept>
#include "dualoutputstream.hpp"

using basic_block = std::vector<nlohmann::json>;
using block_map = std::map<std::string, basic_block>;
using cfg = std::map<std::string, std::vector<std::string>>;

const std::set<std::string> TERMINATORS = {"jmp", "br", "ret"};
// std::map<std::string, basic_block> block_map;
// std::map<std::string, std::vector<std::string>> cfg;

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
    name = "b" + std::to_string(blockcount++);
  }
  return name;
}

block_map formBlocks(const nlohmann::json& body) {
  basic_block curBlock;
  block_map blockmap;
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
        if (curBlock.size() > 0) {
          blockmap[getBlockName(curBlock)] = curBlock;
        }

        curBlock.clear();
      }
    } else {  // A label
      // Print the current block
      // for (const auto& blockInstr : curBlock) {
      //   std::cout << blockInstr << std::endl;
      // }

      // Add blocks to the map of blocks
      if (curBlock.size() > 0) {
        blockmap[getBlockName(curBlock)] = curBlock;
      }

      curBlock.clear();
      curBlock.push_back(instr);
    }
  }

  // Print any remaining instructions in the last block
  // for (const auto& blockInstr : curBlock) {
  //   std::cout << blockInstr << std::endl;
  // }
  // Add blocks to tne map of blocks
  if (curBlock.size() > 0) {
    blockmap[getBlockName(curBlock)] = curBlock;
  }

  curBlock.clear();
  return blockmap;
}

void printBlocks(block_map& blockmap) {
  // Print all the basic blocks in a program
  for (const auto& pair : blockmap) {
    std::cout << pair.first << std::endl;
    for (const auto& instr : pair.second) {
      std::cout << instr << std::endl;
    }
  }
}

void printCfg(cfg& blockcfg) {
  // Print the cfg
  for (const auto& b : blockcfg) {
    std::cout << b.first << " : "
              << " [ ";
    for (const auto& label : b.second) {
      std::cout << label << " ";
    }
    std::cout << " ]" << std::endl;
  }
  std::cout << std::endl;
}

cfg formCfg(block_map& blockmap) {
  // cfg is a map which have mapping from a Label to its successors
  cfg blockcfg;
  for (auto itr = blockmap.begin(); itr != blockmap.end(); ++itr) {
    auto pair = *itr;
    auto lastInstr = pair.second.back();
    if ((lastInstr.find("op") != lastInstr.end())) {
      if (lastInstr["op"] == "jmp" || lastInstr["op"] == "br") {
        // Last instruction is "jmp" or "br"
        // Add edge from the block to the last.dest
        std::vector<std::string> succ;
        for (auto l : lastInstr["labels"]) {
          succ.emplace_back(l);
        }
        blockcfg[pair.first] = succ;
      } else if (lastInstr["op"] == "ret") {
        // Last instruction is a return statement
        std::vector<std::string> succ;
        blockcfg[pair.first] = succ;
      } else {
        // point the current block to the next block if available
        auto next = std::next(itr);
        std::vector<std::string> succ;
        if (next != blockmap.end()) {
          auto p = *next;
          auto l = p.first;
          succ.push_back(l);
        }
        blockcfg[pair.first] = succ;
      }
    }
  }
  return blockcfg;
}

void generateDigraph(const nlohmann::json& func, block_map& blockmap,
                     cfg& blockcfg) {
  // Create a DualOutputStream that writes to "output.dot" and std::cout
  DualOutputStream dualStream("output.dot");
  dualStream << "digraph " << func["name"] << " {" << std::endl;
  for (const auto& block : blockmap) {
    dualStream << "  " << block.first << ";" << std::endl;
  }
  for (const auto& b : blockcfg) {
    for (const auto& label : b.second) {
      dualStream << "  " << b.first << " -> " << label << std::endl;
    }
  }
  dualStream << "}" << std::endl;
}

int main(int argc, char* argv[]) {
  //  // Check if the user provided the filename as a command-line argument
  //  if(argc != 2){
  //   std::cerr << "Usage: " << argv[0] << " <input_file_json>" << std::endl;
  //   return 1;
  //  }
  // std::string inputJson = argv[1];
  // std::ifstream f(inputJson);
  // if (!f.is_open()) {
  //   std::cerr << "Error: Unable to open the input file." << std::endl;
  //   return 1;  // Exit with an error code
  // }

  // Load JSON data from stdin
  nlohmann::json prog; 
  try {
    std::cin >> prog;
  } catch (const std::ios_base::failure& e) {
    // Catch and ignore the BrokenPipeError
    if (e.code() != std::io_errc::stream) {
      throw;  // Re-throw if it's not a BrokenPipeError
    }
  }
  // std::ifstream f("anotherjmpret.json");
  // prog = nlohmann::json::parse(f);

  for (const auto& func : prog["functions"]) {
    auto blockmap = formBlocks(func["instrs"]);
    // printBlocks(blockmap);
    auto blockcfg = formCfg(blockmap);
    // printCfg(blockcfg);
    generateDigraph(func, blockmap, blockcfg);
  }

  return 0;
}

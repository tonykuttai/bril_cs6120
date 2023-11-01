import json
import sys

TERMINATORS = {'jmp', 'br', 'ret'}
def form_blocks(body):
    cur_block = []
    for instr in body:
        if 'op' in instr: # an actual instruction
            cur_block.append(instr)
            # check for terminators
            if instr['op'] in TERMINATORS:
                yield cur_block
                cur_block = []
        else: # A label
                yield cur_block

                cur_block = [instr]
    yield cur_block

def block_map(blocks):
     out = {}
     for block in blocks:
          if 'label' in block[0]:
               name = block[0]['label']
               block = block[1:]
          else:
               name = 'b{}'.format(len(out))

          out[name] = block
     return out

def get_cfg(name2block):
     # given a name-to-block map, produce a mapping from block names to successor block names
     out = {}
     for block in name2block.items():
           
        
def mycfg():
    prog = json.load(sys.stdin)
    for func in prog['functions']:
        name2block = block_map(form_blocks(func['instrs']))
        print(name2block)

        for block in name2block:
        # for block in form_blocks(func['instrs']):
        #      print(block)

if __name__ == '__main__':
    mycfg()

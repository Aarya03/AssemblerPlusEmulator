# AssemblerPlusEmulator
<p>
Two pass assembler and Emulator for an extended SIMPLE instruction set</br>

<b>Instruction Set</b></br>
<table style="width:100%">
  <tr>
    <th>Mnemonic</th>
    <th>Opcode</th>
    <th>Operand</th>
    <th>Formal Specifications</th>
    <th>Description</th>
  </tr>
  <tr>
    <td>data</td>
    <td></td>
    <td>value</td>
    <td></td>
    <td>Reserve a memory location, initialized to the value specified</td>
  </tr>
  <tr>
    <td>ldc</td>
    <td>0</td>
    <td>value</td>
    <td>B := A; A := value; </td>
    <td>Load accumulator with the value specified </td>
  </tr>
  <tr>
    <td>adc</td>
    <td>1</td>
    <td>value</td>
    <td>A := A + value;</td>
    <td>Add the value specified to the accumulator </td>
  </tr>
  <tr>
    <td>ldl</td>
    <td>2</td>
    <td>offset</td>
    <td>B := A; A := memory[SP + offset];</td>
    <td>Load local</td>
  </tr>
  <tr>
    <td>stl</td>
    <td>3</td>
    <td>offset</td>
    <td>memory[SP + offset] := A; A := B; </td>
    <td>Store local</td>
  </tr>
  <tr>
    <td>ldnl</td>
    <td>4</td>
    <td>offset</td>
    <td>A := memory[A + offset];</td>
    <td>Load non-local</td>
  </tr>
  <tr>
    <td>stnl</td>
    <td>5</td>
    <td>offset</td>
    <td>memory[A + offset] := B; </td>
    <td>Store non-local</td>
  </tr>
  <tr>
    <td>add</td>
    <td>6</td>
    <td></td>
    <td>A := B + A;</td>
    <td>Addition</td>
  </tr>
  <tr>
    <td>sub</td>
    <td>7</td>
    <td></td>
    <td>A := B - A; </td>
    <td>Subtraction</td>
  </tr>
  <tr>
    <td>shl</td>
    <td>8</td>
    <td></td>
    <td>A := B << A; </td>
    <td>Shift left </td>
  </tr>
  <tr>
    <td>shr</td>
    <td>9</td>
    <td></td>
    <td>A := B >> A; </td>
    <td>Shift right</td>
  </tr>
  <tr>
    <td>adj</td>
    <td>10</td>
    <td>value</td>
    <td>SP := SP + value; </td>
    <td>Adjust SP </td>
  </tr>
  <tr>
    <td>a2sp</td>
    <td>11</td>
    <td></td>
    <td>SP := A; A := B; </td>
    <td>Transfer A to SP</td>
  </tr>
  <tr>
    <td>sp2a</td>
    <td>12</td>
    <td></td>
    <td>B := A; A := SP; </td>
    <td>Transfer SP to A </td>
  </tr>
  <tr>
    <td>call</td>
    <td>13</td>
    <td>offset</td>
    <td>B := A; A := PC; PC := PC + offset; </td>
    <td>Call procedure </td>
  </tr>
  <tr>
    <td>return</td>
    <td>14</td>
    <td></td>
    <td>PC := A; A := B; </td>
    <td>Return from procedure </td>
  </tr>
  <tr>
    <td>brz</td>
    <td>15</td>
    <td>offset</td>
    <td>if A == 0 then PC := PC + offset; </td>
    <td>If accumulator is zero, branch to specified offset </td>
  </tr>
  <tr>
    <td>brlz</td>
    <td>16</td>
    <td>offset</td>
    <td>if A < 0 then PC := PC + offset; </td>
    <td>If accumulator is less than zero, branch to specified offset </td>
  </tr>
  <tr>
    <td>br</td>
    <td>17</td>
    <td>offset</td>
    <td>PC := PC + offset; </td>
    <tdBranch to specified offset ></td>
  </tr>
  <tr>
    <td>HALT</td>
    <td>18</td>
    <td></td>
    <td></td>
    <td>Stop the emulator. This is not a `real' instruction, but needed to tell your emulator when to finish. </td>
  </tr>
  <tr>
    <td>SET</td>
    <td>19</td>
    <td>value</td>
    <td></td>
    <td>Set the label on this line to the specified value (rather than the PC). This is an optional extension, for which additional marks are available. </td>
  </tr>
</table>
<b>ASSUMPTIONS:</b></br>
● If a value is in Hexadecimal it would always start with ‘0x’.</br>
● If a value is in Octal it would always start with ‘0’.</br>
● If any operand is missing or invalid the error is ‘improper operand’.</br>
● The Pass1 of the assembler outputs no code and does not fail on undefined labels.</br>
● The size of DATA and SET instructions is assumed to be 32 bits.</br>
● The labels can have ‘_’ anywhere in their names.</br>
</br>
<b>EXPLANATION OF C++ Code:</b></br>
● The data structures like maps, vectors and arrays are declared to store values such as instructions, operands, opcodes, etc.</br>
● Declared functions like removeUneccesarySpaces, removeComment, opcode_to_hex_string, etc. for carrying out various functions.</br>
● In main first the init() function is called to initialize instruct_table and error_table.
Then the extension of the file is checked. If the extension is not ‘.asm’ then error is printed on the screen. Then Pass1() and Pass2() are called.</br>
● The SET instruction is implemented.</br>
● In Pass1() the program is read line by line and errors like ‘extra on end of line’, ‘improper operand’, etc are taken into account. The instructions without labels and
offsets are taken care of. The program counter is also dealt with in this pass.</br>
● In Pass2() the errors regarding labels are taken into account. The ‘.lst’ , ‘.log’ and ‘.o’ files are created.</br>
● Finally in Pass2() writeObjFile function is called if there are no errors. We are opening a .o file as “wb” which stands for writebinary. The hex machine code
obtained is converted to int and written into the ‘.o’ file.</br></br>
<b>Compiling and Using the assembler:</b></br>
Use the following commands:</br>
● g++ asm.cpp -o asm</br>
-> After this an asm executable file will be created.</br>
● ./asm filename.asm</br>
-> After this command a '<b>.o</b>' will be created. This is a binary file and its contents can be observed using '<b>hexdump filename.o</b>' commmand.</br></br>
<b>Compiling and Using the emulator:</b></br>
Use the following commands:</br>
● g++ emu.cpp -o emu</br>
-> After this an emu executable file will be created.</br>
● ./emu filename.o</br>
-> This will list the functions available in the emulator and a log file will also be generated</br>
-trace  show instruction trace</br>
-read   show memory reads</br>
-write  show memory writes</br>
-before show memory dump before execution</br>
-after  show memory dump after execution</br>
-wipe   wipe written flags before execution</br>
-isa    display ISA</br>
-> Now use one of these commands e.g '<b>./emu -trace filename.o</b>' and observe the results.</br>

<b> Both the assembler and emulator were tested in Ubuntu OS.</b>
</p>

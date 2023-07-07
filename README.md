# Assembler-Emulator
### Introduction
This project contains:
* 2 pass assembler for an extended SIMPLE instruction set
* Emulator for the SIMPLE machine

More on the functionality can be found in claims.txt

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
    <td>data</td>
    <td>19</td>
    <td>value</td>
    <td></td>
    <td>Reserve a memory location, initialized to the value specified</td>
  </tr>
  <tr>
    <td>SET</td>
    <td>20</td>
    <td>value</td>
    <td></td>
    <td>Set the label on this line to the specified value (rather than the PC).</td>
  </tr>
</table>

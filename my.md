# my
## 内存块初始化为递减序列的作用

### 1. **内存测试 (Memory Testing)**
```
FF FE FD FC FB ... 03 02 01 00
```
- **March测试算法**：使用递增/递减模式检测内存位是否粘连或损坏
- 写入特定模式后读取，验证数据是否保持正确
- 相比全0或全F，递减序列能更好检测地址线故障

### 2. **缓冲区初始化**
```c
uint8_t buffer[256];
memset(buffer, 0, 256);  // 传统方式：全0
```
- 确保缓冲区有确定初始值
- 避免"幽灵数据"导致的未定义行为
- 递减序列便于调试时识别"未初始化"区域

### 3. **硬件寄存器初始化**
- 某些硬件需要特定初始模式才能正常工作
- 递减序列提供唯一可识别模式

### 4. **调试与可视化**
- 内存中一眼就能看出：
  - 哪些被写入（变成递减序列）
  - 哪些未被访问（全FF/00）
  - 数据是否正确覆盖

### 5. **汇编学习目的**
- 理解CPU寄存器（AX, DS, BX, CX）
- 理解内存寻址方式（`[bx]`）
- 理解循环结构（`loop`指令）

---

**简单说**：递减序列是最简单直观的内存填充方式，便于验证程序是否正确写入内存，是底层编程的基础练习。

choco install dosbox -y

MASM 仅运行在win
mkdir -p ~/masm && cd ~/masm && wget -q https://github.com/Alexpux/MASM/archive/refs/heads/master.zip -O masm.zip && unzip -q masm.zip && ls -la MASM-master/

mkdir -p /mnt/d/develops/git/github/as/AssemblyLab/masm && cd /mnt/d/develops/git/github/as/AssemblyLab/masm && curl -L -o masm32.zip "https://sourceforge.net/projects/masm32/files/masm32v11r.zip/download"

网盘链接：
https://pan.baidu.com/s/1XBiaXwQwUXzADuSSNHDuWw
提取密码： c2u2

## cmd

```
 & "C:\Program Files (x86)\DOSBox-0.74-3\DOSBox.exe" "D:\develops\git\github\as\AssemblyLab\run_link.bat"
 ```
```
 & "C:\Program Files (x86)\DOSBox-0.74-3\DOSBox.exe" "D:\develops\git\github\as\AssemblyLab\Lab10\LAB10_1.EXE"
 ```
Assembling: lab10_1.asm
D:NLAB10>..\MASMNLINK.EXE LAB10_1.OBJ
Copyright (C) Microsoft Corp 1984-1992. All rights reserved
Sicrosoft (R) Segmented Executable Linker Version 5.31.009 Jul 13 1992
Run File [LAB10_1.exel:
List File [nul.mapl:
Libraries [.libl:
Definitions File [nul.defl:
LINK: warning L4021: no stack segment
D:\LAB10>..NMASMNML.EXE /c lab10_2.asmMicrosoft (R) Macro Assembler Version 6.11Copyright (C) Microsoft Corp 1981-1993. All rights reserved
Assembling: lab10_2.asm
D:\LAB10>..\MASMNLINK.EXE LAB10 2.OBJ
Microsoft (R) Segmented Executable Linker Version 5.31.009 Jul 13 1992Copyright (C) Microsoft Corp 1984-1992. All rights reserved
Bun File [LAB10 2.exel:AA



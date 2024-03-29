
/*

The project is developed as part of Computer Architecture class
Project Name: Functional Simulator for subset of RISCV Processor

Developer's Name:SUSHIL KUMAR
Developer's Entry number:2021CSB1136
Developer's Name:GYANENDRA MANI
Developer's Entry number:2021csb1090
Developer's Name:RAKESH
Developer's Entry number:2021CSB1126
Date:13-3-2023

*/

/* myRISCVSim.cpp
   Purpose of this file: implementation file for myRISCVSim
*/
#include <bits/stdc++.h>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

// current instruction 
int PC=0,loop=1;

//result after execution
int resultALU,resultMEM;

// Register file
vector<int> X(32, 0);

// memory
vector<int> MEM(40000, 0);

// current instruction
bitset<32> Inst,imm; // int i = stoul(p, nullptr, 16);

bitset<7> opcode, funct7; // opcode

bitset<5> rd, rs1, rs2;

bitset<3> funct3;

// it is used to set the reset values
// reset all registers and memory content to 0


// reads from the instruction memory and updates the instruction register
void fetch(){
    string x,s;
   stringstream ss;
  ss << hex << PC;
  s = "0x" + ss.str();
    fstream FileName;                   
    FileName.open("input.mc", ios::in);         
    if(!FileName){                        
        cout<<"File doesn’t exist.";          
    }else{
        while (1) {         
            FileName>>x;              
           if (FileName.eof()){
             loop=0;
             break;
             }
            if(x==s){
              FileName>>x;
              x.erase(x.begin(),x.begin()+2);
              unsigned int num =  stoul(x, nullptr, 16); 
              Inst=num; 
              break;    
            }              
        }
    }
    FileName.close();
}

// reads the instruction register, reads operand1, operand2 fromo register file, decides the operation to be performed in execute stage
void decode()
{
  for (int i = 0; i < 7; i++) // opcode
  {
    opcode[i] = Inst[i];
  }
  for (int i = 0; i < 5; i++) // rd
  {
    rd[i] = Inst[i + 7];
  }
  for (int i = 0; i < 3; i++) // funct3
  {
    funct3[i] = Inst[i + 12];
  }
  for (int i = 0; i < 5; i++) // rs1
  {
    rs1[i] = Inst[i + 15];
  }
  for (int i = 0; i < 5; i++) // rs2
  {
    rs2[i] = Inst[i + 20];
  }
  for (int i = 0; i < 7; i++) // funct7
  {
    funct7[i] = Inst[i + 25];
  }
  
  if(opcode.to_ulong()==19 || opcode.to_ulong()==3 ||opcode.to_ulong()==103)
  {
    for (int i = 0; i < 12; i++) // immidaite of type I
    {
      imm[i] = Inst[i + 20];
    }    
    if(imm[11]==1)
    {
      for (int i = 12; i < 32; i++)
      {
        imm[i]=1;
      }
    }
    else
    {
      for (int i = 12; i < 32; i++)
      {
        imm[i]=0;
      }
    }  //immidiate of type I
  }
  else if(opcode.to_ulong()==35)
  {
    for (int i = 0; i < 5; i++) // immidiate of type S
    {
      imm[i] = Inst[i + 7];
    }
    for (int i = 0; i < 7; i++)
    {
      imm[i + 5] = Inst[i + 25];
    }  
    if(imm[11]==1)
    {
      for (int i = 12; i < 32; i++)
      {
        imm[i]=1;
      }
    }
    else
    {
      for (int i = 12; i < 32; i++)
      {
        imm[i]=0;
      }
    }
  }                         // immidiate of type S
  else if(opcode.to_ulong()==99)
  {
    imm[11] = Inst[7]; // immidiate of B type
     imm[0]=0;
    for (int i = 0; i < 4; i++)
    {
      imm[i + 1] = Inst[i + 8];
    }
    for (int i = 0; i < 6; i++)
    {
      imm[i + 5] = Inst[i + 25];
    }
    imm[12] = Inst[31];  
    if(imm[12]==1)
    {
      for (int i = 13; i < 32; i++)
      {
        imm[i]=1;
      }
    }
    else
    {
      for (int i = 13; i < 32; i++)
      {
        imm[i]=0;
      }
    }
    //immidiate of B type
  }
  else if(opcode.to_ulong()==55 || opcode.to_ulong()==23)
  {
    imm=Inst;
    for (int i = 0; i < 12; i++) // immidiate of U type
    {
      imm[i] =0;
    } // immidiate of U type
  }
  else if(opcode.to_ulong()==111)
  {
    for (int i = 0; i < 8; i++) // immidiate of J type
    {
      imm[i + 12] = Inst[i + 12];
    }
    imm[11] = Inst[20];
    for (int i = 0; i < 10; i++)
    {
      imm[i + 1] = Inst[i + 21];
    }
    imm[20] = Inst[31];  
    if(imm[20]==1)
    {
      for (int i = 21; i < 32; i++)
      {
        imm[i]=1;
      }
    }
    else
    {
      for (int i = 21; i < 32; i++)
      {
        imm[i]=0;
      }
    }
    //immidiate of J type
  }
}

// executes the ALU operation based on ALUop
void execute()
{
    switch (opcode.to_ulong()) // R type operations
    {
    case 51:
        switch (funct7.to_ulong())
        {
        case 32:
            switch (funct3.to_ulong())
            {
            case 0: // sub
                resultALU = X[rs1.to_ulong()] - X[rs2.to_ulong()];
                break;
            case 5: // sra
                if (X[rs1.to_ulong()] >> 31 == 0)
                resultALU = X[rs1.to_ulong()] >> X[rs2.to_ulong()];
             else
              {
              resultALU = X[rs1.to_ulong()];
              for (int i = 0; i < X[rs2.to_ulong()]; i++)
             {
              resultALU >>= 1;
              resultALU += pow(2, 31);
             }
            }
        break;
            }
            break;
        case 0:
            switch (funct3.to_ulong())
            {
            case 0: // add
                resultALU = X[rs1.to_ulong()] + X[rs2.to_ulong()];
                break;
            case 4: // xor
                resultALU = X[rs1.to_ulong()] ^ X[rs2.to_ulong()];
                break;
            case 6: // or
                resultALU = X[rs1.to_ulong()] | X[rs2.to_ulong()];
                break;
            case 7: // and
                resultALU = X[rs1.to_ulong()] & X[rs2.to_ulong()];
                break;
            case 1: // sll
                resultALU = X[rs1.to_ulong()] << X[rs2.to_ulong()];
                break;
            case 5: // srl
                resultALU = X[rs1.to_ulong()] >> X[rs2.to_ulong()];
                break;
            case 2: // slt
                (X[rs1.to_ulong()] < X[rs2.to_ulong()]) ? resultALU = 1 : resultALU = 0;
                break;
            }
            break;
        }
        break; // R type end
    case 19:   // I type operation
        switch (funct3.to_ulong())
        {
        case 0: // addi
            resultALU = X[rs1.to_ulong()] + (int32_t)imm.to_ulong();
            break;
         case 1: // slli
         { 
            bitset<5> b(imm.to_string(), 27, 31);
           resultALU = X[rs1.to_ulong()] << b.to_ulong();
             break;
         }
        case 7: // andi
            resultALU = X[rs1.to_ulong()] & (int32_t)imm.to_ulong();
            break;
        case 6: // ori
            resultALU = X[rs1.to_ulong()] | (int32_t)imm.to_ulong();
            break;
        }
        break; // I type operations
    case 3:    // load type operations
        switch (funct3.to_ulong())
        {
        case 0: // lb
            resultALU = (X[rs1.to_ulong()] + (int32_t)imm.to_ulong())/4;
            break;

        case 1: // lh
            resultALU = (X[rs1.to_ulong()] + (int32_t)imm.to_ulong())/4;
            break;

        case 2: // lw
            resultALU = (X[rs1.to_ulong()] + (int32_t)imm.to_ulong())/4;
            break;
        }
        break; // load type operations
    case 103:  // jalr
        switch (funct3.to_ulong())
        {
        case 0: // jalr
            resultALU = X[rs1.to_ulong()] + (int32_t)imm.to_ulong();
            break;
        }
        break;
    case 35: // S type operation
        switch (funct3.to_ulong())
        {
        case 0: // sb
            resultALU = (X[rs1.to_ulong()] + (int32_t)imm.to_ulong())/4;
            break;

        case 1: // sh
            resultALU = (X[rs1.to_ulong()] + (int32_t)imm.to_ulong())/4;
            break;

        case 2: // sw
            resultALU = (X[rs1.to_ulong()] + (int32_t)imm.to_ulong())/4;
            break;
        }
        break;

    case 99: // B type operations
        switch (funct3.to_ulong())
        {
        case 0: // beq
            resultALU = PC + (int32_t)imm.to_ulong();
            break;
        case 1: // bne
            resultALU = PC + (int32_t)imm.to_ulong();
            break;

        case 4: // blt
            resultALU = PC + (int32_t)imm.to_ulong();
            break;

        case 5: // bge
            resultALU = PC + (int32_t)imm.to_ulong();
            break;
        }
        break;
    case 55: // U type (lui)
        resultALU = (int32_t)imm.to_ulong();
        break;
    case 23: // U type (auipc)
        resultALU = PC + (int32_t)imm.to_ulong();
        break;
    case 111: // J type (jal)
        resultALU = PC + (int32_t)imm.to_ulong();
        break;
    default:
        break;
    }
}
// perform the memory operation
void mem(){
   if(opcode.to_ulong()==3){
      resultMEM=MEM[resultALU];
   }else if(opcode.to_ulong()==35){
       MEM[resultALU]=X[rs2.to_ulong()];
   }
}
// writes the results back to register file
void write_back()
{
  PC = PC + 4;
  switch (opcode.to_ulong())
  {
  case 51://R type
    X[rd.to_ulong()] = resultALU;
    break;
  case 19:// I type
    X[rd.to_ulong()] = resultALU;
    break;
  case 3:// Load type    
   X[rd.to_ulong()] = resultMEM;
    break;
  case 99://B type
    switch (funct3.to_ulong())
    {
    case 0://beq
      if (X[rs1.to_ulong()]==X[rs2.to_ulong()])
      {
        PC = resultALU;
      }
      break;
    case 1://bne
      if (X[rs1.to_ulong()]!= X[rs2.to_ulong()])
      {
        PC = resultALU;
      }
      break;
    case 4://blt
      if (X[rs1.to_ulong()]<X[rs2.to_ulong()])
      {
        PC = resultALU;
      }
      break;
    case 5://bge
      if (X[rs1.to_ulong()]>=X[rs2.to_ulong()])
      {
        PC = resultALU;
      }
      break;
    default:
      break;
    }
    break;
  case 111://jal
    X[rd.to_ulong()] = PC;
    PC = resultALU;
    break;
  case 103://jalr
    X[rd.to_ulong()] = PC;
    PC = resultALU;
    break;
  case 55://lui
    X[rd.to_ulong()] = resultALU;
    break;
  case 23://auipc
    X[rd.to_ulong()] = resultALU;
    break;

  default:
    break;
  }
}

int main()
{
  
  while (1)
  {
    fetch();
     if(!loop) break;
    decode();
    execute();
    mem();
    write_back();
  }
 ofstream outputFile("output.txt");
 outputFile<<"\n"<<"Values of register's"<<"\n\n";
  for (int i = 0; i < X.size(); i++)
  {
    outputFile << "x" << i << " -> " << X[i] << endl;
  }
  outputFile<<"\n"<<"Values at the memory addresses"<<"\n\n";
  for (int i = 0; i < MEM.size(); i++)
  {
    outputFile << i << " -> "<< MEM[i] << endl;
  }
}

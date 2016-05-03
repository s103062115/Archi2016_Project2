#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define v0 2
#define v1 3
#define a0 4
#define a1 5
#define a2 6
#define a3 7
#define t0 8
#define t1 9
#define t2 10
#define t3 11
#define t4 12
#define t5 13
#define t6 14
#define t7 15
#define s0 16
#define s1 17
#define s2 18
#define s3 19
#define s4 20
#define s5 21
#define s6 22
#define s7 23
#define t8 24
#define t9 25
#define gp 28
#define sp 29
#define fp 30
#define ra 31
int IF = 0;
int fwdr = 0;
char* ID = "NOP";
char* EX = "NOP";
char* DM = "NOP";
char* WB = "NOP";
unsigned int Pow(int a,int p){
    int i;
    unsigned int tmp = 1;
    for(i = 0;i < p;i++){
        tmp = tmp*a;
    }
    return tmp;
}
unsigned int Dm[256] = {0};
int Dnum;
unsigned int Im[256] = {0};
int Inum;
unsigned int registers[32] = {0};
int error_w0 = 0;
int error_no = 0;
int error_ao = 0;
int error_dm = 0;
int error(){
    return error_ao || error_dm;
}
#define add 0x20
#define addu 0x21
#define sub 0x22
#define and 0x24
#define or 0x25
#define xor 0x26
#define nor 0x27
#define nand 0x28
#define slt 0x2A
#define sll 0x00
#define srl 0x02
#define sra 0x03
#define jr 0x08
int end = 0;
int wbadd = -1;
int wbrsd = -1;
int stall = 0;
int flush = 0;
int for_B = 0;
int for_Bie = 0;
char forad = 0;
int op,rs,rt,rd,shamt,func,imm,ads;
int wbad = -1;
int wbrs = -1;
int for_id = 0;
int for_ex = 0;
int go = 0;
int asad = 0;
char idtype, extype, dmtype, wbtype;
unsigned int inPC,PC;
unsigned int S(int C){
    unsigned int A1;
    if(C>=0x8000){
        A1 = C+0xffff0000;
    }else{
        A1 = C;
    }
    return A1;
}
int sign(unsigned int a){
    return a/(int)(Pow(2,31)+0.01);
}
int Add(int rs,int rt,int rd){
    unsigned int RS = registers[rs];
    unsigned int RT = registers[rt];
    if(for_ex == 1){
        if(forad == 's'){
            RS = wbrsd;
        }else{
            RT = wbrsd;
        }
    }else if(for_id == 1){
        if(forad == 's'){
            RS = wbrs;
        }else{
            RT = wbrs;
        }
    }
    wbrs = RS + RT;
    wbad = rd;
    if((int)RS > 0 && (int)wbrs < (int)RT){
        return -1;
    }else if((int)RT > 0 && (int)wbrs < (int)RS){
        return -1;
    }else if((int)RS < 0 && (int)wbrs > (int)RT){
        return -1;
    }else if((int)RT < 0 && (int)wbrs > (int)RS){
        return -1;
    }
    return 1;
}
int Sub(int rs,int rt,int rd){
    int RS = registers[rs];
    int RT = registers[rt];
    if(for_ex == 1){
        if(forad == 's'){
            RS = wbrsd;
        }else{
            RT = wbrsd;
        }
    }else if(for_id == 1){
        if(forad == 's'){
            RS = wbrs;
        }else{
            RT = wbrs;
        }
    }
    wbad = rd;
    wbrs = RS - RT;
    if(((int)wbrs > 0 && (int)RS < (int)RT) || ((int)wbrs < 0 && (int)RS > (int)RT)){
        return -1;
    }
    return 1;
}

void And(int rs,int rt,int rd){
    int RS = registers[rs];
    int RT = registers[rt];
    if(for_ex == 1){
        if(forad == 's'){
            RS = wbrsd;
        }else{
            RT = wbrsd;
        }
    }else if(for_id == 1){
        if(forad == 's'){
            RS = wbrs;
        }else{
            RT = wbrs;
        }
    }
    unsigned int A1 = RT;
    unsigned int A2 = RS;
    int i;
    wbad = rd;
    wbrs = 0;
    for(i = 0;i < 32;i++){
        wbrs = wbrs + ((A1%2)&&(A2%2))*(int)(Pow(2,i)+0.01);
        A1 = A1/2;
        A2 = A2/2;
    }
}
void Or(int rs,int rt,int rd){
    int RS = registers[rs];
    int RT = registers[rt];
    if(for_ex == 1){
        if(forad == 's'){
            RS = wbrsd;
        }else{
            RT = wbrsd;
        }
    }else if(for_id == 1){
        if(forad == 's'){
            RS = wbrs;
        }else{
            RT = wbrs;
        }
    }
    unsigned int A1 = RT;
    unsigned int A2 = RS;
    int i;
    wbrs = 0;
    wbad = rd;
    for(i = 0;i < 32;i++){
        wbrs = wbrs + ((A1%2)||(A2%2))*(int)(Pow(2,i)+0.01);
        A1 = A1/2;
        A2 = A2/2;
    }
}
void Xor(int rs,int rt,int rd){
    int RS = registers[rs];
    int RT = registers[rt];
    if(for_ex == 1){
        if(forad == 's'){
            RS = wbrsd;
        }else{
            RT = wbrsd;
        }
    }else if(for_id == 1){
        if(forad == 's'){
            RS = wbrs;
        }else{
            RT = wbrs;
        }
    }
    unsigned int A1 = RT;
    unsigned int A2 = RS;
    int i;
    wbad = rd;
    wbrs = 0;
    for(i = 0;i < 32;i++){
        wbrs = wbrs + ((A1+A2)%2)*(int)(Pow(2,i)+0.01);
        A1 = A1/2;
        A2 = A2/2;
    }
}
void Nor(int rs,int rt,int rd){
    int RS = registers[rs];
    int RT = registers[rt];
    if(for_ex == 1){
        if(forad == 's'){
            RT = registers[rt];
        }else{
            RT = wbrsd;
        }
    }else if(for_id == 1){
        if(forad == 's'){
            RS = wbrs;
        }else{
            RT = wbrs;
        }
    }
    unsigned int A1 = RT;
    unsigned int A2 = RS;
    int i;
    wbad = rd;
    wbrs = 0;
    for(i = 0;i < 32;i++){
        wbrs = wbrs + (!((A1%2)||(A2%2)))*(int)(Pow(2,i)+0.01);
        A1 = A1/2;
        A2 = A2/2;
    }
}
void Nand(int rs,int rt,int rd){
    int RS = registers[rs];
    int RT = registers[rt];
    if(for_ex == 1){
        if(forad == 's'){
            RS = wbrsd;
        }else{
            RT = wbrsd;
        }
    }else if(for_id == 1){
        if(forad == 's'){
            RS = wbrs;
        }else{
            RT = wbrs;
        }
    }
    unsigned int A1 = RT;
    unsigned int A2 = RS;
    int i;
    wbad = rd;
    wbrs = 0;
    for(i = 0;i < 32;i++){
        wbrs = wbrs + (!((A1%2)&&(A2%2)))*(int)(Pow(2,i)+0.01);
        A1 = A1/2;
        A2 = A2/2;
    }
}
void Slt(int rs,int rt,int rd){
    int RS = registers[rs];
    int RT = registers[rt];
    if(for_ex == 1){
        if(forad == 's'){
            RS = wbrsd;
        }else{
            RT = wbrsd;
        }
    }else if(for_id == 1){
        if(forad == 's'){
            RS = wbrs;
        }else{
            RT = wbrs;
        }
    }
    int A1 = RT;
    int A2 = RS;
    wbad = rd;
    wbrs = A1>A2;
}
void Sll(int rt,int rd,int C){
    int A1 = registers[rt];
    if(for_ex == 1)A1 = wbrsd;
    else if(for_id == 1)A1 = wbrs;
    wbad = rd;
    wbrs = A1 << C;
}
void Srl(int rt,int rd,int C){
    int A1 = registers[rt];
    if(for_ex == 1)A1 = wbrsd;
    wbad = rd;
    wbrs = A1 >> C;
}
void Sra(int rt,int rd,int C){
    int A1 = registers[rt];
    if(for_ex == 1)A1 = wbrsd;
    else if(for_id == 1)A1 = wbrs;
    wbad = rd;
    wbrs = A1 >> C;
}
int Addi(int rs,int rt,int C){
    int RS;
    if(for_ex == 1)RS = wbrsd;
    else if(for_id == 1)RS = wbrs;
    else RS = registers[rs];
    unsigned int A1 = S(C);
    wbad = rt;
    wbrs = RS + A1;
    if((int)RS > 0 && (int)wbrs < (int)A1){
        return -1;
    }else if((int)A1 > 0 && (int)wbrs < (int)RS){
        return -1;
    }else if((int)RS < 0 && (int)wbrs > (int)A1){
        return -1;
    }else if((int)A1 < 0 && (int)wbrs > (int)RS){
        return -1;
    }
    return 1;
}
void Addiu(int rs,int rt,int C){
    unsigned int RS;
    if(for_ex == 1)RS = wbrsd;
    else if(for_id == 1)RS = wbrs;
    else RS = registers[rs];
    unsigned int A1 = C;
    wbad = rt;
    wbrs = RS + A1;
}
void Lui(int rt,int C){
    int RS;
    if(for_ex == 1)RS = wbrsd;
    else if(for_id == 1)RS = wbrs;
    else RS = registers[rs];
    wbad = rt;
    wbrs = C << 16;
}
void Andi(int rs,int rt,int C){
    int RS;
    if(for_ex == 1)RS = wbrsd;
    else if(for_id == 1)RS = wbrs;
    else RS = registers[rs];
    unsigned int A1 = C;
    unsigned int A2 = RS;
    int i;
    wbad = rt;
    wbrs = 0;
    for(i = 0;i < 32;i++){
        wbrs = wbrs + ((A1%2)&&(A2%2))*(int)(Pow(2,i)+0.01);
        A1 = A1/2;
        A2 = A2/2;
    }
}
void Ori(int rs,int rt,int C){
    unsigned int RS;
    if(for_ex == 1)RS = wbrsd;
    else if(for_id == 1)RS = wbrs;
    else RS = registers[rs];
    unsigned int A1 = C;
    unsigned int A2 = RS;
    int i;
    wbad = rt;
    wbrs = 0;
    for(i = 0;i < 32;i++){
        wbrs = wbrs + ((A1%2)||(A2%2))*(int)(Pow(2,i)+0.01);
        A1 = A1/2;
        A2 = A2/2;
    }
}
void Nori(int rs,int rt,int C){
    int RS;
    if(for_ex == 1)RS = wbrsd;
    else if(for_id == 1)RS = wbrs;
    else RS = registers[rs];
    unsigned int A1 = C;
    unsigned int A2 = RS;
    int i;
    wbad = rt;
    wbrs = 0;
    for(i = 0;i < 32;i++){
        wbrs = wbrs + (!((A1%2)||(A2%2)))*(int)(Pow(2,i)+0.01);
        A1 = A1/2;
        A2 = A2/2;
    }
}
void Slti(int rs,int rt,int C){
    int RS;
    if(for_ex == 1)RS = wbrsd;
    else if(for_id == 1)RS = wbrs;
    else RS = registers[rs];
    int A2 = (int)RS;
    int A1 = S(C);
    wbad = rt;
    wbrs = (int)A2 < (int)A1;
}
int Cpr(int rs,int rt){
    if(((int)registers[rs] > (int)registers[rt] && (int)(registers[rs] - registers[rt])<0) || ((int)registers[rs] < (int)registers[rt] && (int)(registers[rs] - registers[rt])>0))error_no = 1;
    return registers[rs] - registers[rt];
}
void Lw(int rs,int rt,int C){
    int RS;
    if(for_ex == 1)RS = wbrsd;
    else if(for_id == 1)RS = wbrs;
    else RS = registers[rs];
    int A1 = S(C);
    wbad = rt;
    asad = (int)RS + A1;

}
void Lhu(int rs,int rt,int C){
    int RS;
    if(for_ex == 1)RS = wbrsd;
    else if(for_id == 1)RS = wbrs;
    else RS = registers[rs];
    wbad = rt;
    asad = (int)RS + C;
}
void Lh(int rs,int rt,int C){
    int RS;
    if(for_ex == 1)RS = wbrsd;
    else if(for_id == 1)RS = wbrs;
    else RS = registers[rs];
    int A1 = S(C);
    wbad = rt;
    asad = (int)RS + A1;

}
void Lb(int rs,int rt,int C){
    int RS;
    if(for_ex == 1)RS = wbrsd;
    else if(for_id == 1)RS = wbrs;
    else RS = registers[rs];
    int A1 = S(C);
    wbad = rt;
    asad = (int)RS + A1;

}
void Lbu(int rs,int rt,int C){
    int RS;
    if(for_ex == 1)RS = wbrsd;
    else if(for_id == 1)RS = wbrs;
    else RS = registers[rs];
    wbad = rt;
    asad = (int)RS + C;

}
void Sw(int rs,int rt,int C){
    int RS;
    if(for_ex == 1)RS = wbrsd;
    else if(for_id == 1)RS = wbrs;
    else RS = registers[rs];
    int A1 = S(C);
    wbad = -1;
    asad = RS + A1;

}
void Sh(int rs,int rt,int C){
    int RS;
    if(for_ex == 1)RS = wbrsd;
    else if(for_id == 1)RS = wbrs;
    else RS = registers[rs];
    int A1 = S(C);
    wbad = rt;
    asad = RS + A1;
}
void Sb(int rs,int rt,int C){
    int RS;
    if(for_ex == 1)RS = wbrsd;
    else if(for_id == 1)RS = wbrs;
    else RS = registers[rs];
    int A1 = S(C);
    asad = RS + A1;

}
int CprF(int in1,int in2){
    if(((int)in1 > (int)in2 && (int)(in1 - in2)<0) || ((int)in1 < (int)in2 && (int)(in1 - in2)>0))error_no = 1;
    return in1 - in2;
}
unsigned int rvs(unsigned int buffer){
    unsigned int a = 0;
    int b1,b2,b3;
    b1 = (int)(Pow(2,8)+0.01);
    b2 = (int)(Pow(2,16)+0.01);
    b3 = (int)(Pow(2,24)+0.01);
    a = a+buffer/b3;
    a = a+(buffer%b3)/b2*b1;
    a = a+(buffer%b2)/b1*b2;
    a = a+(buffer%b1)*b3;
    return a;
}
int fwded = 0;
void Rtype(unsigned int b){
    if(func == jr){
        ID = "JR";
        if((wbad == rs && (wbad != -1 && wbad != 0))){
            stall = 1;
        }else if((wbadd == rs && (wbadd != -1 && wbadd != 0))){
            stall = 1;
        }else stall = 0;
    }else if(((wbad == rs || wbad == rt) && (wbad != -1 && wbad != 0))){
        if(idtype == 'L'){
            stall = 1;
        }else if(wbad == rs){
            if(wbadd == rt)stall = 1;
            else{
                for_id = 1;
                forad = 's';
                fwdr = rs;
            }
        }else if(wbad == rt){
            if(wbadd == rs)stall = 1;
            else{
                for_id = 1;
                forad = 't';
                fwdr = rt;
            }
        }
    }else if(((wbadd == rs || wbadd == rt) && (wbadd != -1 && wbadd != 0))){
        if(idtype == 'L'){
            stall = 1;
        }else for_ex = 1;
        stall = 1;
    }else stall = 0;
    if(func == add){
        ID = "ADD";
    }else if(func == addu){
        ID = "ADDU";
    }else if(func == sub){
        ID = "SUB";
    }else if(func == and){
        ID = "AND";
    }else if(func == or){
        ID = "OR";
    }else if(func == xor){
        ID = "XOR";
    }else if(func == nor){
        ID = "NOR";
    }else if(func == nand){
        ID = "NAND";
    }else if(func == slt){
        ID = "SLT";
    }else if(func == sll){
        if(rt == 0 && rd == 0 && shamt == 0){
            ID = "NOP";
        }else ID = "SLL";
    }else if(func == srl){
        ID = "SRL";
    }else if(func == sra){
        ID = "SRA";
    }
}
void Jtype(unsigned int b){
    ads = b%(int)(Pow(2,26)+0.01);
    if(op == 0x03){
        ID = "JAL";
    }else{
        ID = "J";
    }
}
void Itype(unsigned int b){
    idtype = 'I';
    if(op == 0x04){
        if(((wbad == rs || wbad == rt) && (wbad != -1 && wbad != 0))){
            if(idtype == 'L'){
                stall = 1;
            }else{
                if(rs == rt){
                    PC = PC + S(imm)*4 - 4;
                    if(S(imm) != 1)flush = 1;
                }else if(wbad == rs){
                    if(CprF(wbrs,registers[rt]) == 0){
                        PC = PC + S(imm)*4 - 4;
                        if(S(imm) != 1)flush = 1;
                    }else{
                        go = PC+4;
                    }
                    forad = 's';
                    for_Bie = 1;
                    fwdr = rs;
                }else{
                    if(CprF(registers[rs],wbrs) == 0){
                        PC = PC + S(imm)*4 -4;
                        if(S(imm) != 1)flush = 1;
                    }else{
                        go = PC+4;
                    }
                    forad = 't';
                    for_Bie = 1;
                    fwdr = rt;
                }
                stall = 0;
            }
        }else if(((wbadd == rs || wbadd == rt) && (wbadd != -1 && wbadd != 0))){
            if(idtype == 'L'){
                stall = 1;
            }else{
                if(rs == rt){
                    PC = PC + S(imm)*4 - 4;
                    if(S(imm) != 1)flush = 1;
                }else if(wbadd == rs){
                    if(CprF(wbrsd,registers[rt]) == 0){
                        PC = PC + S(imm)*4 - 4;
                        if(S(imm) != 1)flush = 1;
                    }else{
                        go = PC+4;
                    }
                    forad = 's';
                    for_B = 1;
                    fwdr = rs;
                }else{
                    if(CprF(registers[rs],wbrsd) == 0){
                        PC = PC + S(imm)*4 -4;
                        if(S(imm) != 1)flush = 1;
                    }else{
                        go = PC+4;
                    }
                    forad = 't';
                    for_B = 1;
                    fwdr = rt;
                }
                stall = 0;
            }
        }else if(Cpr(registers[rs],registers[rt]) == 0){
            PC = PC + S(imm)*4-4;
            if(S(imm) != 1)flush = 1;
        }else{
            go = PC+4;
        }
        stall = 0;
        ID = "BEQ";
    }else if(op == 0x05){
        if(((wbad == rs || wbad == rt) && (wbad != -1 && wbad != 0))){
            if(idtype == 'L'){
                stall = 1;
            }else{
                if(rs == rt){

                }else if(wbad == rs){
                    if(CprF(wbrs,registers[rt]) != 0){
                        PC = PC + S(imm)*4-4;
                        if(S(imm) != 1)flush = 1;
                    }else{
                        go = PC+4;
                    }
                    forad = 's';
                    for_Bie = 1;
                    fwdr = rs;
                }else{
                    if(CprF(registers[rs],wbrs) != 0){
                        PC = PC + S(imm)*4-4;
                        if(S(imm) != 1)flush = 1;
                    }else{
                        go = PC+4;
                    }
                    forad = 't';
                    for_Bie = 1;
                    fwdr = rt;
                }
                stall = 0;
            }
        }else if(((wbadd == rs || wbadd == rt) && (wbadd != -1 && wbadd != 0))){
            if(idtype == 'L'){
                stall = 1;
            }else{
                if(rs == rt){

                }else if(wbadd == rs){
                    if(CprF(wbrsd,registers[rt]) != 0){
                        PC = PC + S(imm)*4-4;
                        if(S(imm) != 1)flush = 1;
                    }else{
                        go = PC+4;
                    }
                    forad = 's';
                    for_B = 1;
                    fwdr = rs;
                }else{
                    if(CprF(registers[rs],wbrsd) != 0){
                        PC = PC + S(imm)*4-4;
                        if(S(imm) != 1)flush = 1;
                    }else{
                        go = PC+4;
                    }
                    forad = 't';
                    for_B = 1;
                    fwdr = rt;
                }
                stall = 0;
            }
        }else if(Cpr(registers[rs],registers[rt]) != 0){
            PC = PC + S(imm)*4-4;
            if(S(imm) != 1)flush = 1;
        }else{
            go = PC+4;
        }
        stall = 0;
        ID = "BNE";
    }else if(op == 0x07){
        if(((wbad == rs) && (wbad != -1 && wbad != 0))){
            if(idtype == 'L'){
                stall = 1;
            }else{
                if(wbad == rs){
                    if(wbrs > 0){
                        PC = PC + S(imm)*4-4;
                        if(S(imm) != 1)flush = 1;
                    }else{
                        go = PC+4;
                    }
                    forad = 's';
                    for_Bie = 1;
                    fwdr = rs;
                }
                stall = 0;
            }
        }else if(((wbadd == rs) && (wbadd != -1 && wbadd != 0))){
            if(idtype == 'L'){
                stall = 1;
            }else{
                if(wbadd == rs){
                    if(wbrsd > 0){
                        PC = PC + S(imm)*4-4;
                        if(S(imm) != 1)flush = 1;
                    }else{
                        go = PC+4;
                    }
                    forad = 's';
                    for_B = 1;
                    fwdr = rs;
                }
                stall = 0;
            }
        }else{
            if(registers[rs]>0){
                PC = PC + S(imm)*4-4;
                if(S(imm) != 1)flush = 1;
            }else{
                go = PC+4;
            }
            stall = 0;
        }
        ID = "BGTZ";
    }else if((wbad == rs && (wbad != -1 && wbad != 0)) ){
        if(idtype == 'L'){
            stall = 1;
        }else{
            for_id = 1;
            forad = 's';
            fwdr = rs;
        }
    }else if((wbadd == rs && (wbadd != -1 && wbadd != 0))){
        if(idtype == 'L'){
            stall = 1;
        }else{
            for_ex = 1;
            forad = 's';
            fwdr = rs;
        }
        stall = 1;
    }else stall = 0;
    if(op == 0x08){
        ID = "ADDI";
    }else if(op == 0x09){
        ID = "ADDIU";
    }else if(op == 0x23){
        idtype = 'L';
        ID = "LW";
    }else if(op == 0x21){
        idtype = 'L';
        ID = "LH";
    }else if(op == 0x25){
        idtype = 'L';
        ID = "LHU";
    }else if(op == 0x20){
        idtype = 'L';
        ID = "LB";
    }else if(op == 0x24){
        idtype = 'L';
        ID = "LBU";
    }else if(op == 0x2B){
        idtype = 'S';
        ID = "SW";
    }else if(op == 0x29){
        idtype = 'S';
        ID = "SH";
    }else if(op == 0x28){
        idtype = 'S';
        ID = "SB";
    }else if(op == 0x0f){
        idtype = 'L';
        ID = "LUI";
    }else if(op == 0x0c){
        ID = "ANDI";
    }else if(op == 0x0d){
        ID = "ORI";
    }else if(op == 0x0e){
        ID = "NORI";
    }else if(op == 0x0a){
        ID = "SLTI";
    }
}
void decode(unsigned int b){
    flush = 0;
    for_B = 0;
    for_ex = 0;
    for_id = 0;
    func = b%64;
    int b1,b2,b3,b4;
    b1 = (int)(Pow(2,11)+0.01);
    b2 = (int)(Pow(2,16)+0.01);
    b3 = (int)(Pow(2,21)+0.01);
    b4 = (int)(Pow(2,26)+0.01);
    op = b/b4;
    imm = b%b2;
    shamt = (b%b1)/64;
    rs = (b%b4)/b3;
    rt = (b%b3)/b2;
    rd = (b%b2)/b1;
    if(b == 0){
        ID = "NOP";
    }else if(b == -1){
        ID = "HALT";
    }else if(op == 0){

        idtype = 'R';
        Rtype(b);
    }else if(op == 0x02 || op == 0x03){
        idtype = 'J';
        Jtype(b);
    }else{
        Itype(b);
    }
}
void dm(){
    wbrsd = wbrs;
    wbadd = wbad;
    DM = EX;
    dmtype = extype;
    wbad = -1;
    if(strcmp(DM,"LW") == 0){
        if(((int)asad)%4!=0){
            error_dm = 1;
        }
        if((int)asad > 1023 || (int)asad < 0){
            error_ao = 1;
        }

        if(error() == 0){
            wbrsd = Dm[((int)asad)/4];
        }
    }else if(strcmp(DM,"LH") == 0){
        if(((int)asad)%2!=0){
            error_dm = 1;
        }else if((int)asad > 1023 || (int)asad < 0){
            error_ao = 1;
        }
        if(error() == 0){
            if(((int)asad)%4 == 2){
                wbrsd = Dm[((int)asad)/4]%(int)(Pow(2,16));
            }else{
                wbrsd = Dm[((int)asad)/4]/(int)(Pow(2,16));
            }
            if(wbrsd>=(int)(Pow(2,15))){
                wbrsd = wbrsd+0xffff0000;
            }
        }
    }else if(strcmp(DM,"LHU") == 0){
        if(((int)asad)%2!=0){
            error_dm = 1;
        }
        if((int)asad > 1023 || (int)asad < 0){
            error_ao = 1;
        }
        if(error() == 0){
            if(((int)asad)%4 == 2){
                wbrsd = Dm[((int)asad)/4]%(int)(Pow(2,16));
            }else{
                wbrsd = Dm[((int)asad)/4]/(int)(Pow(2,16));
            }
        }
    }else if(strcmp(DM,"LB") == 0){
        if((int)asad > 1023 || (int)asad < 0){
            error_ao = 1;
        }else{
            if(((int)asad)%4 == 3){
                wbrsd = Dm[((int)asad)/4]%(int)(Pow(2,8));
            }else if(((int)asad)%4 == 2){
                wbrsd = Dm[((int)asad)/4]/(int)(Pow(2,8))%(int)(Pow(2,8));
            }else if(((int)asad)%4 == 1){
                wbrsd = Dm[((int)asad)/4]/(int)(Pow(2,16))%(int)(Pow(2,8));
            }else{
                wbrsd = Dm[((int)asad)/4]/(int)(Pow(2,24));
            }
            if(wbrsd>=(int)(Pow(2,7))){
                wbrsd = wbrsd+0xffffff00;
            }
        }
    }else if(strcmp(DM,"LBU") == 0){
        if(asad > 1023){
            error_ao = 1;
        }else{
            if(((int)asad)%4 == 3){
                wbrsd = Dm[((int)asad)/4]%(int)(Pow(2,8));
            }else if(((int)asad)%4 == 2){
                wbrsd = Dm[((int)asad)/4]/(int)(Pow(2,8))%(int)(Pow(2,8));
            }else if(((int)asad)%4 == 1){
                wbrsd = Dm[((int)asad)/4]/(int)(Pow(2,16))%(int)(Pow(2,8));
            }else{
                wbrsd = Dm[((int)asad)/4]/(int)(Pow(2,24));
            }
        }
    }else if(strcmp(DM,"SW") == 0){
        if(((int)asad)%4!=0){
            error_dm = 1;
        }
        if((int)asad > 1023){
            error_ao = 1;
        }
        if(error() == 0){
            Dm[((int)asad)/4] = registers[rt];
        }
    }else if(strcmp(DM,"SH") == 0){
        if(((int)asad)%2!=0){
            error_dm = 1;
        }else if((int)asad > 1023){
            error_ao = 1;
        }
        if(error() == 0){
            if(((int)asad)%4 == 2){
                Dm[((int)asad)/4] = Dm[((int)asad)/4]/0x00010000*0x00010000;
                Dm[((int)asad)/4] = Dm[((int)asad)/4] + registers[rt]%0x00010000;
            }else{
                Dm[((int)asad)/4] = Dm[((int)asad)/4]%0x00010000;
                Dm[((int)asad)/4] = Dm[((int)asad)/4] + registers[rt]%0x00010000*0x00010000;
            }
        }
    }else if(strcmp(DM,"SB") == 0){
        if((int)asad > 1023){
            error_ao = 1;
        }else{
            if(((int)asad)%4 == 3){
                Dm[((int)asad)/4] = Dm[((int)asad)/4]/0x00000100*0x00000100;
                Dm[((int)asad)/4] = Dm[((int)asad)/4] + registers[rt]%0x00000100;
            }else if(((int)asad)%4 == 2){
                Dm[((int)asad)/4] = Dm[((int)asad)/4]/0x00010000*0x00010000 + Dm[((int)asad)/4]%0x00000100;
                Dm[((int)asad)/4] = Dm[((int)asad)/4] + registers[rt]%0x00000100*0x00000100;
            }else if(((int)asad)%4 == 1){
                Dm[((int)asad)/4] = Dm[((int)asad)/4]/0x01000000*0x01000000 + Dm[((int)asad)/4]%0x00010000;
                Dm[((int)asad)/4] = Dm[((int)asad)/4] + registers[rt]%0x00000100*0x00010000;
            }else{
                Dm[((int)asad)/4] = Dm[((int)asad)/4]%0x01000000;
                Dm[((int)asad)/4] = Dm[((int)asad)/4] + registers[rt]%0x00000100*0x01000000;
            }
        }
    }


}
void excute(){
    if(stall){
        extype = 0;
        EX = "NOP";
    }else {
        extype = idtype;
        EX = ID;
    }
    if(strcmp(EX,"ADD") == 0){
        if(Add(rs,rt,rd) < 0){
            error_no = 1;
        }
    }else if(strcmp(EX,"SUB") == 0){
        if(Sub(rs,rt,rd) < 0){
            error_no = 1;
        }
    }else if(strcmp(EX,"ADDU") == 0){
        Add(rs,rt,rd);
    }else if(strcmp(EX,"AND") == 0){
        And(rs,rt,rd);
    }else if(strcmp(EX,"OR") == 0){
        Or(rs,rt,rd);
    }else if(strcmp(EX,"XOR") == 0){
        Xor(rs,rt,rd);
    }else if(strcmp(EX,"NOR") == 0){
        Nor(rs,rt,rd);
    }else if(strcmp(EX,"NAND") == 0){
        Nand(rs,rt,rd);
    }else if(strcmp(EX,"SLT") == 0){
        Slt(rs,rt,rd);
    }else if(strcmp(EX,"SLL") == 0){
        Sll(rt,rd,shamt);
    }else if(strcmp(EX,"SRL") == 0){
        Srl(rt,rd,shamt);
    }else if(strcmp(EX,"SRA") == 0){
        Sra(rt,rd,shamt);
    }else if(strcmp(EX,"JR") == 0){
        PC = registers[rs];
    }else if(strcmp(EX,"ADDI") == 0){
        if(Addi(rs,rt,imm)<0){
            error_no = 1;
        }
    }else if(strcmp(EX,"ADDIU") == 0){
        Addiu(rs,rt,imm);
    }else if(strcmp(EX,"LW") == 0){
        Lw(rs,rt,imm);
    }else if(strcmp(EX,"LH") == 0){
        Lh(rs,rt,imm);
    }else if(strcmp(EX,"LHU") == 0){
        Lhu(rs,rt,imm);
    }else if(strcmp(EX,"LB") == 0){
        Lb(rs,rt,imm);
    }else if(strcmp(EX,"LBU") == 0){
        Lbu(rs,rt,imm);
    }else if(strcmp(EX,"SW") == 0){
        Sw(rs,rt,imm);
    }else if(strcmp(EX,"SH") == 0){
        Sh(rs,rt,imm);
    }else if(strcmp(EX,"SB") == 0){
        Sb(rs,rt,imm);
    }else if(strcmp(EX,"LUI") == 0){
        Lui(rt,imm);
    }else if(strcmp(EX,"ANDI") == 0){
        Andi(rs,rt,imm);
    }else if(strcmp(EX,"ORI") == 0){
        Ori(rs,rt,imm);
    }else if(strcmp(EX,"NORI") == 0){
        Nori(rs,rt,imm);
    }else if(strcmp(EX,"SLTI") == 0){
        Slti(rs,rt,imm);
    }else if(strcmp(EX,"JAL") == 0){
        registers[31] = PC;
        PC = (PC)/(int)(Pow(2,28)+0.01)*(int)(Pow(2,28)+0.01) + ads*4;
    }else if(strcmp(EX,"J") == 0){
        PC = (PC)/(int)(Pow(2,28)+0.01)*(int)(Pow(2,28)+0.01) + ads*4;
    }/*else if(strcmp(EX,"BEQ") == 0 || strcmp(EX,"BNE") == 0 || strcmp(EX,"BGTZ") == 0){
        PC = go;
    }*/


}
void write(){
    wbtype = dmtype;
    WB = DM;
    if(strcmp(WB,"NOP") != 0 && strcmp(WB,"HALT") != 0 && wbtype != 'S')
    {
        if(wbadd == 0)error_w0 = 1;
        registers[wbadd] = wbrsd;
    }
    if(registers[0] != 0){
        error_w0 = 1;
        registers[0] = 0;
    }

    wbadd = -1;
}
int main () {
    FILE* iImage;
    FILE* dImage;
    FILE* snp;
    FILE* edp;
    unsigned int buffer;
    iImage = fopen ("iimage.bin","rb");
    dImage = fopen("dimage.bin","rb");
    snp = fopen("snapshot.rpt","w");
    edp = fopen("error_dump.rpt","w");
    int i=0;
    int j;

    while(fread(&buffer, 4, 1, dImage)){
        i++;
        if(i == 1){
            registers[sp] = rvs(buffer);
        }
        if(i == 2)Dnum = rvs(buffer);
        if(i > 2)Dm[i-3] = rvs(buffer);
    }
    i = 0;
    while(fread(&buffer, 4, 1, iImage)){
        i++;
        if(i > 256){
            error_ao = 1;
            break;
        }
        unsigned int db = rvs(buffer);
        if(i == 1){
            inPC = db;
            PC = db;
        }else if(i == 2){
            Inum = db;
        }else if(i > Inum+2){
            break;
        }else{
            Im[i-3] = db;
        }

    }
    int cycle = 0;
    IF = Im[0];
    fprintf(snp,"cycle %d\n",cycle);
    for(j = 0;j < 32;j++)fprintf(snp,"$%02d: 0x%08X\n",j,registers[j]);
    fprintf(snp,"PC: 0x%08X\n",PC);
    fprintf(snp,"IF: 0x%08X\n",IF);
    fprintf(snp,"ID: %s\n",ID);
    fprintf(snp,"EX: %s\n",EX);
    fprintf(snp,"DM: %s\n",DM);
    fprintf(snp,"WB: %s\n\n\n",WB);
    cycle++;
    char fwdedad = 0;
    int fwdedr = 0;
    int preIF = 0;
    for(i = 0;i < Inum;){

        if(fwded == 1)fwded = 0;
        if(for_id == 1){
            fwdedad = forad;
            fwdedr = fwdr;
            fwded = 1;
        }
        if(error() != 0 || error_w0 == 1 || error_no == 1){
            if(error_w0 == 1)fprintf(edp, "In cycle %d: Write $0 Error\n", cycle);
            if(error_ao == 1){
                fprintf(edp, "In cycle %d: Address Overflow\n", cycle);
                end = 1;
            }
            if(error_dm == 1){
                fprintf(edp, "In cycle %d: Misalignment Error\n", cycle);
                end = 1;
            }

            if(error_no == 1)fprintf(edp, "In cycle %d: Number Overflow\n", cycle);
            error_w0 = 0;
            error_ao = 0;
            error_no = 0;
            error_dm = 0;
        }
        if(end)break;
        if(IF == -1 && strcmp(EX,"HALT") == 0 && strcmp(ID,"HALT") == 0 && strcmp(DM,"HALT") == 0 && strcmp(WB,"HALT") == 0){
            break;
        }
        fprintf(snp,"cycle %d\n",cycle);
        for(j = 0;j < 32;j++){

            fprintf(snp,"$%02d: 0x%08X\n",j,registers[j]);
        }
        if(stall == 0)PC = PC+4;
        if(stall == 0)preIF = IF;
        IF = Im[(PC-inPC)/4];
        write();
        dm();
        excute();
        if(flush == 1){
            decode(0);
        }else{
            decode(preIF);
        }


        fprintf(snp,"PC: 0x%08X\n",PC);
        if(flush == 1){
            fprintf(snp,"IF: 0x%08X to_be_flushed\n",IF);
        }else if(stall == 1){
            fprintf(snp,"IF: 0x%08X to_be_stalled\n",IF);
        }else fprintf(snp,"IF: 0x%08X\n",IF);
        if(stall == 1){
            fprintf(snp,"ID: %s to_be_stalled\n",ID);
        }else if(for_Bie == 1){
            fprintf(snp,"ID: %s fwd_ID_EX_r%c_$%d\n",ID,forad,fwdr);
        }else if(for_B == 1){
            fprintf(snp,"ID: %s fwd_EX-DM_r%c_$%d\n",ID,forad,fwdr);
        }else fprintf(snp,"ID: %s\n",ID);
        if(fwded == 1){
            fprintf(snp,"EX: %s fwd_EX-DM_r%c_$%d\n",EX,fwdedad,fwdedr);
        }else fprintf(snp,"EX: %s\n",EX);
        fprintf(snp,"DM: %s\n",DM);
        fprintf(snp,"WB: %s\n\n\n",WB);
        if(PC%4 != 0)error_dm = 1;
        i = (PC-inPC)/4;
        cycle++;
    }
    return 0;
}

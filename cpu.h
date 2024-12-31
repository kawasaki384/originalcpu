#define MEMSIZE 1024*1024

#define DISPLAY_WIDTH 512
#define DISPLAY_HEIGHT 512

union reg{
    int r[32];
    int r0;
    int r1;
    int r2;
    int r3;
    int r4;
    int r5;
    int r6;
    int r7;
    int r8;
    int r9;
    int r10;
    int r11;
    int r12;
    int r13;
    int r14;
    int r15;
    int r16;
    int r17;
    int r18;
    int r19;
    int r20;
    int r21;
    int r22;
    int r23;
    int r24;
    int r25;
    int r26;
    int r27;
    int r28;
    int r29;
    int r30;
    int r31;
};

typedef struct mem_structure{
    int programmem[0x8ffff];
    int vram[0xbffff-0x8ffff];
    int ioram[0xfffff-0xbffff];
} memstructure;

typedef union Memory{
    int mem[MEMSIZE];
    memstructure structure;
} Mem;

typedef struct Register{
    union reg regs;
    int pc;
} Reg;

typedef struct cpu{
    Reg r;
    Mem mem;
} capsella16;
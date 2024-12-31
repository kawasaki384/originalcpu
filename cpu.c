#include <stdio.h>
#include <sys/stat.h>
#include <assert.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include "cpu.h"
#include <stdbool.h>
#include <time.h>

#define CLOCK 1500
#define DISPLAY_FPS 24
//#define DEBUG

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *texture;
SDL_Event event;

//Memory[adress] = value
void set_mem(struct cpu* CPU,unsigned int address,int value){
    //アドレスが0x8ffffから0xbffffのときは、VRAMへのアクセスなのでVRAMを変更する。
    if(address >= 0x8ffff & address <= 0xbffff){

    }

    CPU->mem.mem[address] = value;
}

char get_type(int opcode){
    char result = '\0';

    switch(opcode){
        case 0b00000:
            result = 'R';
            break;
        case 0b00001:
            result = 'R';
            break;
        case 0b00010:
            result = 'R';
            break;
        case 0b00011:
            result = 'R';
            break;
        case 0b00100:
            result = 'R';
            break;
        case 0b00101:
            result = 'R';
            break;
        case 0b00110:
            result = 'R';
            break;
        case 0b00111:
            result = 'I';
            break;
        case 0b01000:
            result = 'I';
            break;
        case 0b01001:
            result = 'M';
            break;
        case 0b01010:
            result = 'M';
            break;
        case 0b01011:
            result = 'M';
            break;
        case 0b01100:
            result = 'M';
            break;
        case 0b01101:
            result = 'M';
            break;
        case 0b01110:
            result = 'S';
            break;
        case 0b01111:
            result = 'S';
            break;
        default:
            break;
    }
    return result;
}

//usigned int ということはregisterということ、intということは即値ということ
void SLT(struct cpu*CPU,unsigned int R0,unsigned int R1,unsigned int R2){
    if(CPU->r.regs.r[R1] < CPU->r.regs.r[R2]){
        CPU->r.regs.r[R0] = 0;
    }
    else if(CPU->r.regs.r[R1] >= CPU->r.regs.r[R2]){
        CPU->r.regs.r[R0] = 1;
    }
}

void ADD(struct cpu*CPU,unsigned int R0,unsigned int R1,unsigned int R2){
    CPU->r.regs.r[R0] = CPU->r.regs.r[R1] + CPU->r.regs.r[R2];
}

void MUL(struct cpu*CPU,unsigned int R0,unsigned int R1,unsigned int R2){
    CPU->r.regs.r[R0] = CPU->r.regs.r[R1] * CPU->r.regs.r[R2];
}

void SUB(struct cpu*CPU,unsigned int R0,unsigned int R1,unsigned int R2){
    CPU->r.regs.r[R0] = CPU->r.regs.r[R1] - CPU->r.regs.r[R2];
}

void AND(struct cpu*CPU,unsigned int R0,unsigned int R1,unsigned int R2){
    CPU->r.regs.r[R0] = CPU->r.regs.r[R1] & CPU->r.regs.r[R2];
}

void OR(struct cpu*CPU,unsigned int R0,unsigned int R1,unsigned int R2){
    CPU->r.regs.r[R0] = CPU->r.regs.r[R1] | CPU->r.regs.r[R2];
}

void XOR(struct cpu*CPU,unsigned int R0,unsigned int R1,unsigned int R2){
    CPU->r.regs.r[R0] = CPU->r.regs.r[R1] ^ CPU->r.regs.r[R2];
}

void ADDI(struct cpu*CPU,unsigned int R0,unsigned int R1,int I2){
    CPU->r.regs.r[R0] = CPU->r.regs.r[R1] + I2;
}

void SUBI(struct cpu*CPU,unsigned int R0,unsigned int R1,int I2){
    CPU->r.regs.r[R0] = CPU->r.regs.r[R1] - I2;
}

void SD(struct cpu*CPU,unsigned int R0,unsigned int R1,int M2){
    set_mem(CPU,CPU->r.regs.r[R1]+M2,CPU->r.regs.r[R0]);
}

void LD(struct cpu*CPU,unsigned int R0,unsigned int R1,int M2){
    CPU->r.regs.r[R0] = CPU->mem.mem[CPU->r.regs.r[R1]+M2];
}

void JAL(struct cpu*CPU,unsigned int R0,unsigned int R1,int M2){
    CPU->r.regs.r[R0] = CPU->r.pc+4;
    //forでは後々pcを4足すからこの時点で4引いとく
    CPU->r.pc = (CPU->r.regs.r[R1]+M2/4)*4 - 4;
}

void BEQ(struct cpu*CPU,unsigned int R0,unsigned R1,int M2){
    if(CPU->r.regs.r[R0] == CPU->r.regs.r[R1])
        //forでは後々pcを4足すからこの時点で4引いとく
        CPU->r.pc = M2-4;
}

void BNE(struct cpu*CPU,unsigned int R0,unsigned R1,int M2){
    if(CPU->r.regs.r[R0] != CPU->r.regs.r[R1])
        //forでは後々pcを4足すからこの時点で4引いとく
        CPU->r.pc = M2-4;
}

void SLLI(struct cpu*CPU,unsigned int R0,unsigned R1,unsigned int S){
    CPU->r.regs.r[R0] = CPU->r.regs.r[R1] << S;
}

void SRLI(struct cpu*CPU,unsigned int R0,unsigned R1,unsigned int S){
    CPU->r.regs.r[R0] = CPU->r.regs.r[R1] >> S;
}


void execute(struct cpu *CPU,int opcode,int arg0,int arg1,int arg2){
    //命令を仕分ける
    switch(opcode){
        case 0b00000:
            SLT(CPU,arg0,arg1,arg2);
            break;
        case 0b00001:
            ADD(CPU,arg0,arg1,arg2);
            break;
        case 0b00010:
            MUL(CPU,arg0,arg1,arg2);
            break;
        case 0b00011:
            SUB(CPU,arg0,arg1,arg2);
            break;
        case 0b00100:
            AND(CPU,arg0,arg1,arg2);
            break;
        case 0b00101:
            OR(CPU,arg0,arg1,arg2);
            break;
        case 0b00110:
            XOR(CPU,arg0,arg1,arg2);
            break;
        case 0b00111:
            ADDI(CPU,arg0,arg1,arg2);
            break;
        case 0b01000:
            SUBI(CPU,arg0,arg1,arg2);
            break;
        case 0b01001:
            SD(CPU,arg0,arg1,arg2);
            break;
        case 0b01010:
            LD(CPU,arg0,arg1,arg2);
            break;
        case 0b01011:
            JAL(CPU,arg0,arg1,arg2);
            break;
        case 0b01100:
            BEQ(CPU,arg0,arg1,arg2);
            break;
        case 0b01101:
            BNE(CPU,arg0,arg1,arg2);
            break;
        case 0b01110:
            SLLI(CPU,arg0,arg1,arg2);
            break;
        case 0b01111:
            SRLI(CPU,arg0,arg1,arg2);
            break;
        default:
            printf("命令が存在しません。");
            assert(0);
            break;
    }
}

void print_regs(struct cpu *CPU){
    printf("pc=%d:regs=",CPU->r.pc);
    for (int i = 0; i < 32;i++){
        printf("r%d=%d,",i,CPU->r.regs.r[i]);
    }

    printf("\n");
}

void init_display(){
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("Moreton:Display", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0);

    renderer = SDL_CreateRenderer(window, -1, 0);

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_TARGET, DISPLAY_WIDTH, DISPLAY_HEIGHT); 

    //画面を初期化する。
    SDL_SetRenderDrawColor(renderer, 255,255,255,250);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

void update_display(struct cpu *CPU){
    //VRAMを描画する。
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer,texture,NULL,NULL);
    SDL_RenderPresent(renderer);
    SDL_UpdateTexture(texture,NULL,&CPU->mem.structure.vram,DISPLAY_WIDTH*sizeof(uint32_t));
}

int main(){
    FILE* f = fopen("untitled.bin","rb");
    fpos_t filesize;

    init_display();

    fseek(f,0,SEEK_END);

    fgetpos(f,&filesize);

    fseek(f,0,SEEK_SET);

    int CODES_LEN = filesize.__pos;

    struct cpu capsella;

    capsella.r.pc = 0;

    //プログラムをメモリに展開する
    for(int i = 0;i < CODES_LEN;i++)
        fread(&capsella.mem.mem[i],sizeof(int),1,f);

    bool quit = false;
    long display_update = 0;

    for(;capsella.r.pc < CODES_LEN && quit== false;capsella.r.pc+=4){
        long start = clock();
        
        int code = capsella.mem.mem[capsella.r.pc/sizeof(int)];
        
        int opcode = code & 0b11111;

        int args[3] = {0,0,0};
        switch(get_type(opcode)){
            case 'R':
                args[0] = (code >> 5) & 0b11111;
                args[1] = (code >> 10) & 0b11111;
                args[2] = (code >> 15) & 0b11111;
                break;
            case 'I':
                args[0] = (code >> 5) & 0b11111;
                args[1] = (code >> 10) & 0b11111;
                args[2] = (code >> 15) & 0b1111111111111111;
                break;
            case 'M':
                args[0] = (code >> 5) & 0b11111;
                args[1] = (code >> 10) & 0b11111;
                args[2] = (code >> 15) & 0b1111111111111111;
                break;
            case 'S':
                args[0] = (code >> 5) & 0b11111;
                args[1] = (code >> 10) & 0b11111;
                args[2] = (code >> 15) & 0b11111;
                break;
        }
        //各レジスタの値を表示
        //print_regs(&capsella);
        //実行する
        execute(&capsella,opcode,args[0],args[1],args[2]);
        
        if(SDL_PollEvent(&event))
            switch (event.type)
            {
                case SDL_QUIT:
                    quit = true;
                    break;
                default:
                    break;
            }

        #ifdef DEBUG
        printf("%d %d",opcode,args[2]);
        print_regs(&capsella);
        #endif DEBUG

        //SDL_Delay(1000/CLOCK);

        long end = clock();

        display_update += (end-start);

        //ディスプレイのFPSに応じてディスプレイを更新する
        if((double)display_update/CLOCKS_PER_SEC >= (double)1/DISPLAY_FPS){
            update_display(&capsella);
            display_update = 0;
        }
    }

    //各レジスタの値を表示
    print_regs(&capsella);

    fclose(f);

    SDL_DestroyTexture(texture);
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    return EXIT_SUCCESS;

}
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<locale.h>

enum interrupcoes {normal=1,ilegal,violacaomem};

typedef int inte;

typedef struct estado{
  int pc;
  int acumu;
  inte *inter;
}cpu_estado;

typedef struct CPU {
  int pc;
  int acumu;
  inte *inter;
  int *mem_dados;
  char **mem_prog;
  int tam_dados;
  int tam_prog;
}cpu;

void CARGI(cpu *c, int arg)
{
  c->acumu = arg;
}

void CARGM(cpu *c, int pos, int *dados)
{
  c->acumu = dados[pos];
}

void CARGX(cpu *c, int pos, int *dados)
{
  c->acumu = dados[dados[pos]];
}

void ARMM(cpu *c, int pos, int *dados)
{
  dados[pos] = c->acumu;
}

void ARMX(cpu *c, int pos, int *dados)
{
  dados[dados[pos]] = c->acumu;
}

void SOMA(cpu *c, int pos, int *dados)
{
  c->acumu += dados[pos];
}

void NEG(cpu *c)
{
  c->acumu *= -1;
}

void DESVZ(cpu *c, int arg)
{
  if(c->acumu == 0)
    c->pc = arg;
}

void cpu_altera_programa(cpu *c, int tam, char **mem)
{
  c->mem_prog = mem;
  c->tam_prog = tam;
}

void cpu_altera_dados(cpu *c, int tam, int *mem)
{
  c->mem_dados = mem;
  c->tam_dados= tam;
}

inte cpu_interrupcao(cpu *c)
{
  return c->inter;
}

void cpu_retorna_interrupcao(cpu *c)
{
  if(c->inter != normal) {
    c->inter = normal;
    c->pc++;
  }
}

char *cpu_instrucao(cpu *c)
{
  if(c->pc > c->tam_prog){
    return NULL;
  } else {
    return c->mem_prog[c->pc];
  }
}

void cpu_salva_estado(cpu *c, cpu_estado *e)
{
  e->acumu = c->acumu;
  e->pc = c->pc;
  e->inter = c->inter;
}

void cpu_altera_estado(cpu *c, cpu_estado *e)
{
  c->acumu = e->acumu;
  c->pc = e->pc;
  c->inter = e->inter;
}

void cpu_estado_inicializa(cpu_estado *e)
{
  e->acumu = 0;
  e->pc = 0;
  e->inter = normal;
}

int pega_arg_int(char *pont)
{
  int arg = 0;

  if('0' < *pont && *pont < '9'){
    while(*pont != '\0'){
      arg *= 10;
      arg += *pont - '0';
      pont++;
    }
  }

  return arg;
}

void decodifica_executa(cpu *c, char **prog)
{
  char inst[7], *pont;
  int arg, i = 0;

  pont = prog[c->pc];

  while(*pont != ' ' && *pont != '\0'){
    inst[i] = *pont;
    pont++;
    i++;
  }
  inst[i] = '\0';
  if(*pont == ' ')
    pont++;

  if(strcmp(inst,"CARGI") == 0){
    arg = pega_arg_int(pont);
    CARGI(c,arg);
  } else if(strcmp(inst,"CARGM") == 0){
    arg = pega_arg_int(pont);
    CARGM(c,arg,c->mem_dados);
  } else if(strcmp(inst,"CARGX") == 0){
    arg = pega_arg_int(pont);
    CARGX(c,arg,c->mem_dados);
  } else if(strcmp(inst,"ARMM") == 0){
    arg = pega_arg_int(pont);
    ARMM(c,arg,c->mem_dados);
  } else if(strcmp(inst,"ARMX") == 0){
    arg = pega_arg_int(pont);
    ARMX(c,arg,c->mem_dados);
  } else if(strcmp(inst,"SOMA") == 0){
    arg = pega_arg_int(pont);
    SOMA(c,arg,c->mem_dados);
  } else if(strcmp(inst,"NEG") == 0){
    NEG(c);
  } else if(strcmp(inst,"DESVZ") == 0){
    arg = pega_arg_int(pont);
    DESVZ(c,arg);
  } else{
    c->inter = ilegal;
  } //printf("saiu\n");

}

void cpu_executa(cpu *c)
{
  if(cpu_interrupcao(c) == normal){
    if(c->pc < c->tam_prog){
      decodifica_executa(c,c->mem_prog);
    } else {
      c->inter = violacaomem;
    }
  }
  if(cpu_interrupcao(c) == normal)
    c->pc++;
}

int main()
{
  setlocale(LC_ALL,"");

  char *programa[] = {
    "CARGI 10",
    "ARMM 2",
    "CARGI 32",
    "SOMA 2",
    "ARMM 0",
    "PARA"
  };

  int dados[4];
  cpu c;
  cpu_estado e;

  // inicializa o estado da CPU
  cpu_estado_inicializa(&e);
  // inicializa a CPU, com o estado interno ...
  cpu_altera_estado(&c, &e);
  // ... a memória de programa ...
  cpu_altera_programa(&c, 6, programa);
  // ... e a memória de dados
  cpu_altera_dados(&c, 4, dados);
  // faz a CPU executar cada instrução do programa,
  // até que cause uma interrupção (que deve ser por instrução ilegal em PARA)
  while (cpu_interrupcao(&c) == normal) {
    cpu_executa(&c);
  }
  // cpu_salva_dados(&c, 4, dados); // se for o caso
  printf("CPU parou na instrução %s (deve ser PARA)\n", cpu_instrucao(&c));
  printf("O valor de m[0] é %d (deve ser 42)\n", dados[0]);
}


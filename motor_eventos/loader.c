#include <stdio.h>
#include "motor_eventos.h"
#include "loader.h"
/* Formato do codigo objeto
   Primeiros 2 bytes tamanho dos dados em
   bytes.
   2 bytes para posiçao de memoria
   dados na memoria em sequencia.
   Esse loader sempre carrega de program.o
   O formato do código objeto é 2 caracteres ascii representando um byte
*/

int Asc2Int(char c) {
    printf("%c", c);
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'A' && c <= 'F')
        return 10 + (c - 'A');
    return 10 + (c - 'a');
}


void CarregaMemoria(){
    unsigned int uiPc = 0;
    unsigned int iSize, iAux = 0;
    FILE *hfProgram = fopen("program.o", "r");
    if (hfProgram == NULL) {
        printf("\r\nERROR READING FROM FILE ");
        return;
    }
    /* Determina tamanho dos dados */
    iSize = (Asc2Int(fgetc(hfProgram)) << 12) + (Asc2Int(fgetc(hfProgram)) << 8) + (Asc2Int(fgetc(hfProgram)) << 4) + Asc2Int(fgetc(hfProgram));
    printf("\r\ndata lenght %d", iSize);
    uiPc = (Asc2Int(fgetc(hfProgram)) << 12) + (Asc2Int(fgetc(hfProgram)) << 8) + (Asc2Int(fgetc(hfProgram)) << 4) + Asc2Int(fgetc(hfProgram));
    printf("\r\nstart adress %x\r\n", uiPc);
    while (iAux < iSize) {
        printf("\n%d ", uiPc + (iAux));
        if ((uiPc + (iAux)) & 0x0001) {
            Memoria[(uiPc + (iAux)) >> 1][1].n0 = Asc2Int(fgetc(hfProgram));
            Memoria[(uiPc + (iAux)) >> 1][1].n1 = Asc2Int(fgetc(hfProgram));
            printf("\n");
        }
        else{
            Memoria[(uiPc + (iAux)) >> 1][0].n0 = Asc2Int(fgetc(hfProgram));
            Memoria[(uiPc + (iAux)) >> 1][0].n1 = Asc2Int(fgetc(hfProgram));
            printf(" ");
        }
        iAux++;
    }
    fclose(hfProgram);
}

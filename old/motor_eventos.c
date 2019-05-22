/******************************************************************************

                            Online C Compiler.
                Code, Compile, Run and Debug C program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/
/* Simulacao de motor de eventos */
/* Para testar estrutura do motor de eventos, será implementada a MVN */
/* Máquina especificada em https://docs.google.com/viewer?a=v&pid=sites&srcid=ZGVmYXVsdGRvbWFpbnwyMDE5cGNzMzIxNnxneDo2ZTQxNGU0YzE3OTdlZWI5 */
#include <stdio.h>
#include "string.h"
/*Variáveis globais do módulo */
unsigned int uiPc;
unsigned int uiRa;
int iAcumulador;
unsigned char Memoria[1024][4];

/*Eventos */
enum {
    JP = 0x0,
    JZ,
    JN,
    CN,
    ADD,
    MINUS,
    MULT,
    DIV,
    LD,
    MM,
    SC,
    OS,
    IO,
    RUF1,
    RUF2,
    RUF3
};
enum {
    MVN_OK = 0,
    MVN_END,
    MVN_ERR = -100
};
void EscreveLinha(unsigned int iPc, char cV1, char cV2, char cV3, char cV4) {
    Memoria[iPc][0] = cV1;
    Memoria[iPc][1] = cV2;
    Memoria[iPc][2] = cV3;
    Memoria[iPc][3] = cV4;
    
}
void InicializaMaquina(void) {
    uiPc = 0;
    uiRa = 0;
    memset(Memoria, 0, sizeof(Memoria));
    iAcumulador = 0;
    /* Rotina para carregar programa */
    /*Primeira linha pula para inicio programa */
    EscreveLinha(0, JP , 0x0, 0x1, 0x0) ;
    EscreveLinha(0x1, 0x0, 0x0, 0x0, 0x1);
    EscreveLinha(0x2, 0x0, 0x0, 0x0, 0x2);
    EscreveLinha(0x3, 0x0, 0x0, 0x0, 0x0);
    EscreveLinha( 0x10, ADD, 0x0, 0x0, 0x1);
    EscreveLinha( 0x11, MM, 0x0, 0x0, 0x4);
    EscreveLinha( 0x12, LD, 0x0, 0x0, 0x4);
    EscreveLinha( 0x13, CN, 0x0, 0x0, 0x4);
    //EscreveLinha( 0x11, CN, 0x0, 0x0, 0x1);
}
int iRetornaEvento() {
    printf("\r\nEvento Retornado %d", Memoria[uiPc][0]);
    return Memoria[uiPc][0];
}
int RetornaEndereco (){
    int iAux;
    iAux = uiPc >> 12;
    iAux = (iAux << 12) + (Memoria[uiPc][1] << 8) +
           (Memoria[uiPc][2] << 4) + Memoria[uiPc][3];
    return iAux;
}
int iMontaValor(int iPc) {
    return (Memoria[iPc][0] << 12)  + (Memoria[iPc][1] << 8) +
           (Memoria[iPc][2] << 4) + Memoria[iPc][3];
}

void CarregaValor(int iPc) {
    Memoria[iPc][0] = iAcumulador >> 12;
    Memoria[iPc][1] = iAcumulador & 0x0F00;
    Memoria[iPc][2] = iAcumulador & 0x00F0;
    Memoria[iPc][3] = iAcumulador & 0x000F;
    printf("\r\nVALOR CARREGADO %x%x%x%x", Memoria[iPc][0], Memoria[iPc][1], Memoria[iPc][2], Memoria[iPc][3]);
}
int iProcessaEvento(int iEvento) {
    int iAux = 0;
    switch (iEvento) {
        case JP:
            uiPc = RetornaEndereco();
            return MVN_OK;
        break;
        case JZ:
            if (iAcumulador == 0) {
                uiPc = RetornaEndereco();
                return MVN_OK;
            }
        break;
        case JN:
            if (iAcumulador < 0) {
                uiPc = RetornaEndereco();
                return MVN_OK;
            }
        break;
        case CN:
            printf("\r\nExecuta instruçao de controle %x%x%x", Memoria[uiPc][1], 
            Memoria[uiPc][2], Memoria[uiPc][3]);
            return MVN_END;
        break;
        case ADD:
            iAux = RetornaEndereco();
            iAcumulador += iMontaValor(iAux);
            /*Determina pc apontado */
        break;
        case MINUS:
            iAux = RetornaEndereco();
            iAcumulador -= iMontaValor(iAux);
        break;
        case MULT:
            iAux = RetornaEndereco();
            iAcumulador *= iMontaValor(iAux);
        break;
        case DIV:
            iAux = RetornaEndereco();
            iAux = iMontaValor(iAux);
            if (iAux == 0)
                return MVN_ERR;
            iAcumulador = iAcumulador/iMontaValor(iAux);
        break;
        case LD:
            iAux = RetornaEndereco();
            iAcumulador = iMontaValor(iAux);
        break;
        case MM:
            iAux = RetornaEndereco();
            CarregaValor(iAux);
        break;
        case SC:
            uiRa = uiPc + 1;
            uiPc = RetornaEndereco();
            return  MVN_OK;
        break;
        case OS:
            printf("\r\nChamada de sistema operacional");
        break;
        case IO:
            printf("\r\nChamada de IO");
        break;
        default:
            printf("\r\nERRO INTERNO FUNCAO NAO ESPECIFICADA");
            return MVN_ERR;
    }
    uiPc++;
    return MVN_OK;
    
}

void ImprimeStatus() {
    printf("\r\n*************************************");
    printf("\r\n**************STATUS*****************");
    printf("\r\nAcumulador %d", iAcumulador);
    printf("\r\n iPc %d", uiPc);
    printf("\r\n uiRa %d", uiRa);
    printf("\r\n*************************************");
    printf("\r\nDigite qualquer tecla para continuar");
    getchar();
}

int main()
{
    int iEvento, iRet;
    InicializaMaquina();
    /* Loop do motor de eventos */
    for (;;) { 
        iEvento = iRetornaEvento();
        // printf("\r\nEvento retornado %d", iEvento);
        iRet = iProcessaEvento(iEvento);
        if (iRet == MVN_END) {
            printf("\r\nFINAL DA EXECUCAO");
            ImprimeStatus();
            return 0;
        }
        ImprimeStatus();
    }

    return 0;
}

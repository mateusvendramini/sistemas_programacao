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
#include "motor_eventos.h"
#include "loader.h"
/*Variáveis globais do módulo */
unsigned int uiPc;
unsigned int uiRa;
int iAcumulador;
nibles Memoria[1024][2];

/*Eventos */
enum {
    JP = 0x0,
    JZ,
    JN,
    LV,
    ADD,
    MINUS,
    MULT,
    DIV,
    LD,
    MM,
    SC,
    RS,
    HM,
    GD,
    PD,
    OS
};
enum {
    MVN_OK = 0,
    MVN_END,
    MVN_ERR = -100
};
void EscreveLinha(unsigned int iPc, char cV1, char cV2, char cV3, char cV4) {
    Memoria[iPc >> 1][0].n0 = cV1;
    Memoria[iPc >> 1][0].n1 = cV2;
    Memoria[iPc >> 1][1].n0 = cV3;
    Memoria[iPc >> 1][1].n1 = cV4;

}

void DumpMemory() {
    int i = 0;
    for (i = 0; i < 0x20; i = i +2) {
        printf("\r\n[%x] %x%x %x%x", i, Memoria[i>>1][0].n0, Memoria[i>>1][0].n1, Memoria[i>>1][1].n0,Memoria[i>>1][1].n1);
    }
}
void InicializaMaquina(void) {
    uiPc = 0;
    uiRa = 0;
    memset(Memoria, 0, sizeof(Memoria));
    iAcumulador = 0;
    CarregaMemoria();
    return;
    /* Rotina para carregar programa */
    /*Primeira linha pula para inicio programa */
    EscreveLinha(0, JP , 0x0, 0x1, 0x0) ;
    EscreveLinha(0x2, 0x0, 0x0, 0x0, 0x1);
    EscreveLinha(0x4, 0x0, 0x0, 0x0, 0x2);
    EscreveLinha(0x6, 0x0, 0x0, 0x0, 0x0);
    EscreveLinha(0x10, ADD, 0x0, 0x0, 0x3);
    EscreveLinha(0x12, MM, 0x0, 0x0, 0x8);
    EscreveLinha(0x14, LD, 0x0, 0x0, 0x4);
    EscreveLinha(0x16, PD, 0x0, 0x0, 0x0);
    EscreveLinha( 0x18, HM, 0x0, 0x0, 0x4);
    //EscreveLinha( 0x11, CN, 0x0, 0x0, 0x1);
}
int iRetornaEvento() {
    printf("\r\nEvento Retornado %d", Memoria[uiPc >> 1][0].n0);
    return Memoria[uiPc >> 1][0].n0;
}

int RetornaEndereco (){
    int iAux;
    iAux = uiPc >> 12;
    iAux = (iAux << 12) + (Memoria[uiPc >> 1][0].n1 << 8 ) +
           (Memoria[uiPc >> 1][1].n0 << 4) + Memoria[uiPc >> 1][1].n1;
    return iAux;
}
int iMontaValor2bits(int iPc) {
    if (!(iPc & 0x0001))
        return (Memoria[iPc >> 1][0].n0 << 4)  + (Memoria[iPc >> 1][0].n1);
    else
        return (Memoria[iPc >> 1][1].n0 << 4) + Memoria[iPc >> 1][1].n1;
}

int iMontaValor(int iPc) {
    return (Memoria[iPc][0].n0 << 12)  + (Memoria[iPc][0].n1 << 8) +
           (Memoria[iPc][1].n0 << 4) + Memoria[iPc][1].n1;
}

int iMontaValor3Bits(int iPc) {
    return ((Memoria[iPc][0].n1 << 8) +
           (Memoria[iPc][1].n0 << 4) + Memoria[iPc][1].n1);
}

void CarregaValor(int iPc) {
    Memoria[iPc][0].n0 = (char)(iAcumulador >> 12);
    Memoria[iPc][0].n1 = (char)(iAcumulador & 0x0F00);
    Memoria[iPc][1].n0 = (char)(iAcumulador & 0x00F0);
    Memoria[iPc][1].n1 = (char)(iAcumulador & 0x000F);
    printf("\r\nVALOR CARREGADO %x%x%x%x", Memoria[iPc][0].n0, Memoria[iPc][0].n1,
            Memoria[iPc][1].n0, Memoria[iPc][1].n1);
}

void CarregaValorImediato(int iPc, char cValue) {
    if ((iPc & 0x0001)) {
        Memoria[iPc][0].n0 =  cValue & 0xF0;// iAcumulador >> 12;
        Memoria[iPc][0].n1 = cValue & 0x0F;//iAcumulador & 0x0F00;
    }
    else
    {
        Memoria[iPc][1].n0 =  cValue & 0xF0;// iAcumulador >> 12;
        Memoria[iPc][1].n1 = cValue & 0x0F;//iAcumulador & 0x0F00;
    }
    printf("\r\nVALOR CARREGADO %x%x%x%x", Memoria[iPc][0].n0, Memoria[iPc][0].n1,
            Memoria[iPc][1].n0, Memoria[iPc][1].n1);
}

void SalvaPc(int iPc) {
    Memoria[iPc][0].n0 = (uiPc + 2 ) >> 12;
    Memoria[iPc][0].n1 = ((uiPc + 2 ) & 0x0F00) >> 8;
    Memoria[iPc][1].n0 = ((uiPc + 2 ) & 0x00F0) >> 4;
    Memoria[iPc][1].n1 = (uiPc + 2 ) & 0x000F;
    printf("\r\nVALOR CARREGADO %x%x%x%x", Memoria[iPc][0].n0, Memoria[iPc][0].n1,
            Memoria[iPc][1].n0, Memoria[iPc][1].n1);
}

int iProcessaEvento(int iEvento) {
    char cAux;
    int iAux = 0;
    switch (iEvento) {
        case JP:
            uiPc = RetornaEndereco();
            printf("\r\njp called result [%x]", uiPc);
            return MVN_OK;
        break;
        case JZ:
            printf("\r\njz called result [%d]", iAcumulador);
            if (iAcumulador == 0) {
                uiPc = RetornaEndereco();
                return MVN_OK;
            }
        break;
        case JN:
            printf("\r\njn called result [%d]", iAcumulador);
            if (iAcumulador < 0) {
                uiPc = RetornaEndereco();
                return MVN_OK;
            }
        break;
        case LV:
            iAcumulador = iMontaValor3Bits(uiPc >> 1);
            printf("\r\nLV imediative result [%d]", iAcumulador);
            break;
        //case CN:
        //    printf("\r\nExecuta instruçao de controle %x%x%x", Memoria[uiPc][1],
        //    Memoria[uiPc][2], Memoria[uiPc][3]);
        //    return MVN_END;
        //break;
        case ADD:
            iAux = RetornaEndereco();
            iAcumulador += iMontaValor2bits(iAux);
            printf("\r\nadd result [%d]", iAcumulador);
            /*Determina pc apontado */
        break;
        case MINUS:
            iAux = RetornaEndereco();
            iAcumulador -= iMontaValor2bits(iAux);
            printf("\r\nminus result [%d]", iAcumulador);
        break;
        case MULT:
            iAux = RetornaEndereco();
            iAcumulador *= iMontaValor2bits(iAux);
            printf("\r\nmult result [%d]", iAcumulador);
        break;
        case DIV:
            iAux = RetornaEndereco();
            iAux = iMontaValor2bits(iAux);
            if (iAux == 0)
                return MVN_ERR;
            iAcumulador = iAcumulador/iAux;
            printf("\r\ndivision result [%d]", iAcumulador);
        break;
        case LD:
            iAux = RetornaEndereco();
            iAcumulador = iMontaValor(iAux >> 1);
            printf("\r\nloaded from memory [%x] [%d]", uiPc, iAcumulador);
        break;
        case MM:
            iAux = RetornaEndereco();
            CarregaValor(iAux >> 1);
            printf("\r\nmoved to memory [%x] [%d]", iAux, iAcumulador);
        break;
        case SC:
            //uiRa = uiPc + 2;
            iAux = RetornaEndereco();
            SalvaPc(iAux >> 1);
            uiPc = iAux + 2;
            printf("\r\nsubroutine call %x", uiPc);
            return  MVN_OK;
        break;
        case OS:
            printf("\r\nChamada de sistema operacional");
        break;
        case RS:
            iAux = RetornaEndereco();
            uiPc = iMontaValor(iAux >> 1);
            printf("\r\nreturned from subroutine %x", uiPc);
            return MVN_OK;
            //printf("\r\nChamada de IO");
        break;
        case HM:
            return MVN_END;
        case GD:
            printf("\r\nDigite o numero a se colocar no acumulador");
            //iAux = getch();
            scanf("%d", &iAux);
            iAcumulador = iAux;
            //iAux = RetornaEndereco();
            //CarregaValorImediato(iAux, cAux);
            printf("Carregado valor [%d] ", iAux);
            break;
        case PD:
            printf("Acumulador valor [%d] ", iAcumulador);
            break;
        default:
            iAux = RetornaEndereco();

            return MVN_ERR;
    }
    uiPc+= 2;
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
    printf("\r\nMEMORy\r\n");
    DumpMemory();
    printf("\r\n**************End*****************");
    getchar();
}

void RunMVM()
{
    int iEvento, iRet;
    InicializaMaquina();
    DumpMemory();
    /* Loop do motor de eventos */
    for (;;) {
        iEvento = iRetornaEvento();
        // printf("\r\nEvento retornado %d", iEvento);
        iRet = iProcessaEvento(iEvento);
        if (iRet == MVN_END) {
            printf("\r\nFINAL DA EXECUCAO");
            ImprimeStatus();
            return;
        }
        //ImprimeStatus();
    }

    return;
}

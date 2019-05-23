#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* Definicoes de tipos para montador */
enum {
    INST1 = 0,
    INST2,
    PSEUDO
};
typedef struct Simbolo {
    char szSimbolo[64];
    unsigned int uiEndereco;
    int fIsDefined;
} SIMBOLO;

typedef struct Mneumonico {
    char szSimbolo[3];
    unsigned char ucCodigo;
    int iTipo;
}MNEUMONICO;

typedef struct codigo {
    unsigned char ucTamanho[2];
    unsigned char ucEnderecoInicial[2];
    unsigned char vcCodigo[4048];
} CODIGO;

typedef union {
    CODIGO stCodigo;
    unsigned char ucBytes[4052];
}CODIGOTOTAL;

SIMBOLO vstTabelaSimbolos[512];
const MNEUMONICO vstTabelaMneumonicos [] = {
    {{"JP"}, 0, INST1},
    {{"J"}, 0, INST1},
    {{"JZ"}, 1, INST1},
    {{"Z"}, 1, INST1},
    {{"LV"}, 2, INST1},
    {{"V"}, 2, INST1},
    {{"+"}, 4, INST1},
    {{"-"}, 5, INST1},
    {{"*"}, 6, INST1},
    {{"/"}, 7, INST1},
    {{"LD"}, 8, INST1},
    {{"L"}, 8, INST1},
    {{"MM"}, 9, INST1},
    {{"M"}, 9, INST1},
    {{"SC"}, 10, INST1},
    {{"S"}, 10, INST1},
    {{"RS"}, 11, INST1},
    {{"R"}, 11, INST1},
    {{"HM"}, 12, INST1},
    {{"H"}, 12, INST1},
    {{"GD"}, 13, INST1},
    {{"G"}, 13, INST1},
    {{"PD"}, 14, INST1},
    {{"P"}, 14, INST1},
    {{"OS"}, 15, INST1},
    {{"O"}, 15, INST1},
    {{"@"}, 0, PSEUDO},
    {{"#"}, 0, PSEUDO},
    {{"$"}, 0, PSEUDO},
    {{"K"}, 0, PSEUDO},

};

enum {
    OK = 0,
    ERRO,
    END
};

enum {
    LINHA_SEM_LABEL,
    LINHA_COM_LABEL,
    LINHA_COM_COMENTARIO
};

FILE *fp;
char szFileName[100];
char szLine[512];
/* variáveis globais */
int iPasso;
unsigned int iContador;
int iUltimoMneumonicoInserido;

void LeLinha(char* pszLine) {
    char c;
    c = fgetc(fp);
    while (c != '\0' && c != '\n') {
        *pszLine = c;
        pszLine++;
        c = fgetc(fp);
    }
}

void IniciaPrimeiroPasso() {
    memset(vstTabelaSimbolos, 0, sizeof(vstTabelaSimbolos));
    iPasso = 1;
    iContador = 0;
    printf("Digite o nome + extensão do arquivo");
    scanf("%s", szFileName);
    fp = fopen(szFileName, "r");
    if (fp == NULL) {
        printf("Erro ao ler do arquivo :(");
    }
}

int iRetornaEvento () {
    memset(szLine, 0, sizeof(szLine));
    LeLinha(szLine);
    if (szLine[0] == ' ')
        return LINHA_SEM_LABEL;
    else if (szLine[0] == ';')
        return LINHA_COM_COMENTARIO;
    return LINHA_COM_LABEL;
}
/*retorna o indice do label na tabela de labels
  se não encontrar, retorna -1 */
int iBuscaLabel (char* pszLabel) {
    int iAux;
    for (iAux = 0; iAux < iUltimoMneumonicoInserido; iAux++) {
        if(!strcmp(pszLabel, vstTabelaSimbolos[iAux].szSimbolo))
            return iAux;
    }
    return -1;
}

int iBuscaMneumonico (char* pszLabel) {
    int iAux;
    for (iAux = 0; iAux < sizeof(vstTabelaMneumonicos)/sizeof(MNEUMONICO); iAux++) {
        if(!strcmp(pszLabel, vstTabelaMneumonicos[iAux].szSimbolo))
            return iAux;
    }
    return -1;
}

void SalvaSimbolo(char *pszTok) {
    vstTabelaSimbolos[iUltimoMneumonicoInserido].fIsDefined = 1;
    strcpy(vstTabelaSimbolos[iUltimoMneumonicoInserido].szSimbolo, pszTok);
    vstTabelaSimbolos[iUltimoMneumonicoInserido].uiEndereco = iContador;
    iUltimoMneumonicoInserido++;
}

void SalvaSimboloNoDef(char *pszTok) {
    //vstTabelaSimbolos[iUltimoMneumonicoInserido].fIsDefined = 1;
    strcpy(vstTabelaSimbolos[iUltimoMneumonicoInserido].szSimbolo, pszTok);
    vstTabelaSimbolos[iUltimoMneumonicoInserido].uiEndereco = iContador;
    iUltimoMneumonicoInserido++;
}

int iProcessaEvento (int iEvento) {
    char *pszTok;
    int iRet;
    if (iPasso == 1) {
        switch (iEvento){
        case LINHA_COM_LABEL:
            /* Primeiro token deve ser um label */
            pszTok = strtok(szLine, " ");
            iRet = iBuscaLabel(pszTok);
            if (iRet < 0) {
                SalvaSimbolo(pszTok);
            }
            else {
                if (vstTabelaSimbolos[iRet].fIsDefined) {
                    printf("\r\nErro de redefinição do simbolo %s na linha %x"
                            "\r\nDefinicao anterior estava em %x", pszTok, iContador, vstTabelaSimbolos[iRet].uiEndereco);
                    return ERRO;
                }
                else {
                    vstTabelaSimbolos[iRet].fIsDefined = 1;
                    vstTabelaSimbolos[iRet].uiEndereco = iContador;
                }
            }
        case LINHA_SEM_LABEL:
            /*Encontra operando */
            pszTok = NULL;
            pszTok = strtok(NULL, " ");
            if (pszTok == NULL) {
                /*Linha vazia não é erro */
                return OK;
            }
            iRet = iBuscaMneumonico(pszTok);
            if (iRet < 0) {
                printf("\r\n%s desconhecido na linha %x", pszTok, iContador);
                return ERRO;
            }
            iContador+= 2;
            /* procura simbolo no operando */
            pszTok = NULL;
            pszTok = strtok(NULL, " ");
            if (pszTok != NULL) {
                    pszTok = strtok(pszTok, ";");
                    iRet = iBuscaLabel(pszTok);
                    if ((iRet < 0) && (pszTok[0] != '"') && (pszTok[0] != '/') && (pszTok[0] != '+')
                          && (pszTok[0] != '-'))
                        SalvaSimboloNoDef(pszTok);
            }
            return OK;
        case LINHA_COM_COMENTARIO:
            return OK;
            break;
        default :
            printf("\r\nEvento desconhecido");
            return ERRO;
        }
    }
    else {

    }
    return OK;
}
/* Toda linha precisa de operando */
int main()
{
    int iEvento, iRet;
    /* Loop do motor de eventos para passo 1 */
    IniciaPrimeiroPasso();
    for (;;) {
        iEvento = iRetornaEvento();
        // printf("\r\nEvento retornado %d", iEvento);
        iRet = iProcessaEvento(iEvento);
        if (iRet == END) {
            printf("\r\nFINAL DA EXECUCAO");
            break;
        }
    }
}

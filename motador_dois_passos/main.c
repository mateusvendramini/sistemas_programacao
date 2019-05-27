#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* Definicoes de tipos para montador */
enum {
    INST1 = 0, /* instruções cujo operando é um end de memória */
    INST2,     /* instruções cujo operando é um valor imediato */
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
    CODIGO stCodObj;
    unsigned char ucBytes[4052];
}CODIGOTOTAL;

SIMBOLO vstTabelaSimbolos[512];
const MNEUMONICO vstTabelaMneumonicos [] = {
    {{"JP"}, '0', INST1},
    {{"J"}, '0', INST1},
    {{"JZ"}, '1', INST1},
    {{"Z"}, '1', INST1},
    {{"JN"}, '2', INST1},
    {{"N"}, '2', INST1},
    {{"LV"}, '3', INST1},
    {{"V"}, '3', INST1},
    {{"+"}, '4', INST1},
    {{"-"}, '5', INST1},
    {{"*"}, '6', INST1},
    {{"/"}, '7', INST1},
    {{"LD"}, '8', INST1},
    {{"L"}, '8', INST1},
    {{"MM"}, '9', INST1},
    {{"M"}, '9', INST1},
    {{"SC"}, 'A', INST1},
    {{"S"}, 'A', INST1},
    {{"RS"}, 'B', INST1},
    {{"R"}, 'B', INST1},
    {{"HM"}, 'C', INST1},
    {{"H"}, 'C', INST1},
    {{"GD"}, 'D', INST1},
    {{"G"}, 'D', INST1},
    {{"PD"}, 'E', INST1},
    {{"P"}, 'E', INST1},
    {{"OS"}, 'F', INST1},
    {{"O"}, 'F', INST1},
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
CODIGOTOTAL stCodigo;

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

void IniciaSegundoPasso() {
    iPasso = 2;
    iContador = 0;
    memset(&stCodigo, 0, sizeof(CODIGOTOTAL));
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
int Asc2Int(char c) {
    printf("%c", c);
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'A' && c <= 'F')
        return 10 + (c - 'A');
    return 10 + (c - 'a');
}
/* Recebe uma sting no formato /yyyy e retorna 0xyyyy */
unsigned char asc2hex4bytes(char *pszAsc) {
    pszAsc++;
    return (Asc2Int(*(pszAsc++)) << 12 ) + (Asc2Int(*(pszAsc++)) << 8 ) +
     (Asc2Int(*(pszAsc++)) << 4 ) + (Asc2Int(*(pszAsc)));
}

/* Recebe uma sting no formato /yyyy e retorna 0xyyyy */
unsigned char asc2hex2bytes(char *pszAsc) {
    pszAsc++;
    return (Asc2Int(*(pszAsc++)) << 4 ) + (Asc2Int(*(pszAsc)));
}

unsigned char hex2asc (unsigned char c) {
    if (c >= 0 && c <= 9){
        return '0' + c;
    }
    return c - 10 + 'A';
}

void ImprimeListagem () {
    printf("\r\n%c%c%c%c %s",stCodigo.stCodObj.vcCodigo[(iContador << 1)], stCodigo.stCodObj.vcCodigo[(iContador << 1) + 1],
                       stCodigo.stCodObj.vcCodigo[(iContador << 1) + 2], stCodigo.stCodObj.vcCodigo[(iContador << 1) + 3], szLine);
}

int iProcessaEvento (int iEvento) {
    char *pszTok = NULL;
    int iRet, iAux;
    if (iPasso == 1) {
        switch (iEvento){
        case LINHA_COM_LABEL:
            /* Primeiro token deve ser um label */
            pszTok = strtok(szLine, " ;");
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
            if (pszTok != NULL)
                pszTok = strtok(NULL, " ;");
            else
                pszTok = strtok(szLine, " ;");
            if (pszTok == NULL) {
                /*Linha vazia não é erro */
                return OK;
            }
            iRet = iBuscaMneumonico(pszTok);
            if (iRet < 0) {
                printf("\r\n%s desconhecido na linha %x", pszTok, iContador);
                return ERRO;
            }
            if (vstTabelaMneumonicos[iRet].iTipo == INST1 || vstTabelaMneumonicos[iRet].iTipo == INST2)
                iContador+= 2;
            else if (vstTabelaMneumonicos[iRet].iTipo == PSEUDO) {
                if (!strcmp(vstTabelaMneumonicos[iRet].szSimbolo, "@"))
                    iContador = asc2hex4bytes(strtok(NULL, " ;"));
                else if (!strcmp(vstTabelaMneumonicos[iRet].szSimbolo, "K"))
                    iContador += 1;
                else {
                    printf("Erro na linha %d", iContador);
                    return ERRO;
                }

            }
            /* procura simbolo no operando */
            pszTok = NULL;
            pszTok = strtok(NULL, " ;");
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
    } /*Passo 2 */
    else {
        switch(iEvento) {
        case LINHA_COM_LABEL:
            pszTok = strtok(szLine, " ;");

        case LINHA_SEM_LABEL:
            if (iEvento == LINHA_SEM_LABEL)
                pszTok = strtok(szLine, " ;");
            else
                pszTok = strtok(NULL, " ;");
            if (pszTok == NULL) {
                printf("Linha sem operando não é erro");
                return OK;
            }
            iRet = iBuscaMneumonico(pszTok);
            switch (vstTabelaMneumonicos[iRet].iTipo) {
            case INST1:
                /* Operando é um endereço de memória */
                stCodigo.stCodObj.vcCodigo[iContador << 1] = vstTabelaMneumonicos[iRet].ucCodigo;
                pszTok = strtok(NULL, " ;");
                if (pszTok == NULL) {
                    printf("\nErro na instrução %s", szLine);
                    printf("\nInstrução sem operador");
                    return ERRO;
                }
                if (pszTok[0] == '/') {
                        stCodigo.stCodObj.vcCodigo[(iContador << 1) + 1] = pszTok[1];
                        stCodigo.stCodObj.vcCodigo[(iContador << 1) + 2] = pszTok[2];
                        stCodigo.stCodObj.vcCodigo[(iContador << 1) + 3] = pszTok[3];
                }
                else if (pszTok[0] == '+' || pszTok[0] == '-' || pszTok[0] == '"' ) {
                    printf("\nErro na instrução %s", szLine);
                    printf("\nOperando esperado do tipo endereço, encontrado %c", pszTok[0]);
                }
                else { /* é um operador simbólico */
                    iAux = iBuscaLabel(pszTok);
                    if (iAux < 0){
                        printf("\nErro na instrução %s", szLine);
                        printf("\nSimbolo  %s nao encontrado", pszTok);
                    }
                    stCodigo.stCodObj.vcCodigo[(iContador << 1) + 1] = hex2asc((vstTabelaSimbolos[iAux].uiEndereco &0xF00) >> 8);
                    stCodigo.stCodObj.vcCodigo[(iContador << 1) + 2] = hex2asc((vstTabelaSimbolos[iAux].uiEndereco &0x0F0) >> 4);
                    stCodigo.stCodObj.vcCodigo[(iContador << 1) + 3] = hex2asc((vstTabelaSimbolos[iAux].uiEndereco &0x00F));
                }
                ImprimeListagem();

                iContador+= 2;
                break;
            case INST2:
                stCodigo.stCodObj.vcCodigo[iContador << 1] = vstTabelaMneumonicos[iRet].ucCodigo;
                pszTok = strtok(NULL, " ;");
                if (pszTok == NULL || pszTok[0] != '/') {
                    printf("\r\nErro na linha %s", szLine);
                    printf("Operador %c incorreto para instrução K", pszTok[0]);
                    return ERRO;
                }
                stCodigo.stCodObj.vcCodigo[(iContador << 1) + 1] = pszTok[1];
                stCodigo.stCodObj.vcCodigo[(iContador << 1) + 2] = pszTok[2];
                stCodigo.stCodObj.vcCodigo[(iContador << 1) + 3] = pszTok[3];
                //////////////////////////////////////////////////////////
                iContador += 2;
                break;
            case PSEUDO:
                if (!strcmp(vstTabelaMneumonicos[iRet].szSimbolo, "@")) {
                    iContador = asc2hex4bytes(strtok(NULL, " ;"));
                    stCodigo.stCodObj.ucEnderecoInicial[0] = (iContador & 0xFF00) >> 2;
                    stCodigo.stCodObj.ucEnderecoInicial[1] = (iContador & 0x00FF);
                }
                else if (!strcmp(vstTabelaMneumonicos[iRet].szSimbolo, "K")) {
                    pszTok = strtok(NULL, " ;");
                    if (pszTok[0] != '/'){
                        printf("\r\nErro na linha %s", szLine);
                        printf("Operador %c incorreto para instrução K", pszTok[0]);
                        return ERRO;
                    }
                    stCodigo.stCodObj.vcCodigo[iContador << 1 ] = pszTok[1];
                    stCodigo.stCodObj.vcCodigo[(iContador << 1) + 1] = pszTok[2];
                    iContador += 1;
                }
                else if (!strcmp(vstTabelaMneumonicos[iRet].szSimbolo, "#")){
                    return END;
                }
                else {
                    printf("\r\nErro na linha %s", szLine);
                }

            }
        }

    }
    return OK;
}
/* Toda linha precisa de operando */
int main()
{
    int iEvento, iRet, i;
    /* Loop do motor de eventos para passo 1 */
    IniciaPrimeiroPasso();
    for (;;) {
        iEvento = iRetornaEvento();
        // printf("\r\nEvento retornado %d", iEvento);
        iRet = iProcessaEvento(iEvento);
        if (iRet == END) {
            printf("\r\nFINAL DA do primeiro passo");
            for (i = 0; i < iUltimoMneumonicoInserido; i++)
                if (!vstTabelaSimbolos[i].fIsDefined) {
                    printf("Símbolo %s indefinido", vstTabelaSimbolos[i].szSimbolo);
                    return -1;
            }
            printf("\r\nPrimeiro passo ok");
            fclose(fp);
            break;
        }
        if (iRet == ERRO) {
            return -1;
        }
    }
    IniciaSegundoPasso();
    for (;;) {
        iEvento = iRetornaEvento();
        // printf("\r\nEvento retornado %d", iEvento);
        iRet = iProcessaEvento(iEvento);
        if (iRet == END) {
            printf("\r\nFINAL  do segundo passo");
            printf("\r\nPrimeiro passo ok");
            fclose(fp);
            break;
        }
        if (iRet == ERRO) {
            return -1;
        }
    }
}

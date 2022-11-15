// Trabalho de programação: Project GPS Style
// Versão: 2022-10-27
//
// Para compilar:
// g++ arquivo.cpp -o arquivo

#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <stdio.h>

int AreaGeografica[10][10];     // Matriz que representa a região gráfica dos pontos, buraco e caminho
int Ax, Ay;                     // Posição inicial
int Bx, By;                     // Posição final
int Cx, Cy;                     // Posicao do buraco/cratera. Optei para letra C de cratera, pois a letra B já está sendo usada pelo ponto B.
int Px, Py;                     // Ponto Auxiliar
int CaminhoX[20], CaminhoY[20]; // Um array com a capacidade máxima de posições de uma reta na matriz, prevendo o maior caminho. O tamanho máximo é a (altura + largura) da matriz.
int contCaminho;                // Posicao atual do caminho. Usado para inserir na posicao seguinte. Depois auto-incrementa.

void cls(void);                                  // Funcao que limpa a tela do terminal e posiciona o cursos na posicao xy(linha 0, coluna 0) da tela.
void Inicio(void);                               // Este método é a apresentação do programa e coleta os dados do usuário
int VerificaSeEstouNoBuraco(int X, int Y);       // Verifica se o caminho está no buraco, se sim retorna 1, caso contrário 0
int VerificaSeEstaForaDaMatriz(int X, int Y);    // Verifica se o caminho está fora da matriz, se sim retorna 1, caso contrário 0
int VerificaSeEstouNaMesmaPosicaoAnterior(void); // Verifica se saí da posição anterior. Retorna 1 se sim e 0 se não.
void VerificaPosicao(void);                      // Verifica e altera posicao de Px e Py. Se está no buraco ou fora da matriz. Decrementa em 1 Px e Py se for o caso
void CorrigeNovaPosicaoDeP(void);                // Corrige posicao de Px e Py
void CalculaCaminho(void);                       // Calcula o caminho e desvio do buraco
void GoToXY(int x, int y);                       // GoToXY eh uma funcao que pula o cursor para uma posicao da tela
void ApresentaMapaDaReta(void);                  // Imprime o mapa com os pontos A, B, Buraco e caminho

int main(int argc, char *argv[])
{
    Inicio();              // Coleta os dados do usuário
    CalculaCaminho();      // Calcula o caminho e desvio do buraco
    ApresentaMapaDaReta(); // Imprime na tela os pontos, buraco e o caminho com desvio do buraco

    GoToXY(0, 12); // Posiciona o cursos para fora do ambiente do GPS, para não sobrescrever o mapa
    return 0;
}

// Funcao que limpa a tela do terminal e posiciona o cursos na posicao xy(linha 0, coluna 0) da tela.
void cls(void)
{
    printf("\033[2J");   // Limpa a tela
    printf("\033[0;0H"); // Devolve o cursor para a linha 0, coluna 0
};

// Este método é a apresentação do programa e coleta os dados do usuário
void Inicio(void)
{
    cls(); // Limpa a tela do terminal

    printf("PROGRAMA GPS DA ESTER \n\n");

    printf("Informe a posição Ax: ");
    scanf("%i", &Ax);
    printf("Informe a posição Ay: ");
    scanf("%i", &Ay);

    printf("Informe a posição Bx: ");
    scanf("%i", &Bx);
    printf("Informe a posição By: ");
    scanf("%i", &By);

    printf("Informe a posição Cx do Buraco: ");
    scanf("%i", &Cx);
    printf("Informe a posição Cy do Buraco: ");
    scanf("%i", &Cy);
};

// Verifica se o caminho está no buraco, se sim retorna 1, caso contrário 0
int VerificaSeEstouNoBuraco(int X, int Y)
{
    if (Cx == X && Cy == Y)
    {
        return 1;
    }
    return 0;
};

// Verifica se o caminho está fora da matriz, se sim retorna 1, caso contrário 0
int VerificaSeEstaForaDaMatriz(int X, int Y)
{
    if (X > 9 || Y > 9 || X < 0 || Y < 0)
    {
        return 1;
    }
    return 0;
};

// Verifica se saí da posição anterior. Retorna 1 se sim e 0 se não.
int VerificaSeEstouNaMesmaPosicaoAnterior(void)
{
    if (CaminhoX[contCaminho] == Px && CaminhoY[contCaminho] == Py)
    {
        return 1;
    }
    return 0;
};

// Ponto auxiliar recebe a posição seguinte ao buraco. Posicao 22 é o meu atual passo.
// 11  21  31
// 12 [22] 32
// 13  23  33
void RecebePosicaoVizinhaDoBuraco(void)
{
    // Variáveis auxiliares para verificar posicao do buraco na vizinhança do passo atual
    int diferencaX;
    int diferencaY;

    // Ponto auxiliar recebe o passo anterior
    contCaminho--;
    Px = CaminhoX[contCaminho];
    Py = CaminhoY[contCaminho];

    diferencaX = Px - Cx;
    diferencaY = Py - Cy;

    if (diferencaX == 1 && diferencaY == 1)
    { // Se buraco pos 11, P = 21
        //printf("Buraco pos 11");
        Py--;
    }
    else if (diferencaX == 0 && diferencaY == 1)
    { // Se buraco pos 21, P = 31
    //printf("Buraco pos 21");
        Px++;
        Py--;
    }
    else if (diferencaX == -1 && diferencaY == 1)
    { // Se buraco pos 31, P = 32
    //printf("Buraco pos 31");
        Px++;
    }
    else if (diferencaX == -1 && diferencaY == 0)
    { // Se buraco pos 32, P = 33
    //printf("Buraco pos 32");
        Px++;
        Py++;
    }
    else if (diferencaX == -1 && diferencaY == -1)
    { // Se buraco pos 33, P = 23
    //printf("Buraco pos 33");
        Py++;
    }
    else if (diferencaX == 0 && diferencaY == -1)
    { // Se buraco pos 23, P = 13
    //printf("Buraco pos 23");
        Px--;
        Py++;
    }
    else if (diferencaX == -1 && diferencaY == 0)
    { // Se buraco pos 13, P = 12
    //printf("Buraco pos 13");
        Px--;
    }
    else if (diferencaX == 1 && diferencaY == 0)
    { // Se buraco pos 12, P = 11
    //printf("Buraco pos 12");
        Px--;
        Py--;
    }
}

// Verifica e altera posicao de Px e Py. Se está no buraco ou fora da matriz.
void VerificaPosicao(void)
{
    if (VerificaSeEstouNoBuraco(Px, Py) == 1)
    {
        //printf("Estou no Buraco P (%i, %i)", Px, Py);
        RecebePosicaoVizinhaDoBuraco();
    }

    if (VerificaSeEstaForaDaMatriz(Px, Py) == 1)
    {
        //printf("Estou fora da Matriz P (%i, %i)", Px, Py);        
        if (Py < 0){
            Py = Py + 2;
        }else if (Py > 9){
            Py = Py - 2;
        }    
        
        if (Px < 0){
            Px = Px + 2;            
        }else if (Px > 9){
            Px = Px - 2;
        }        
    }
};

// Corrige posicao de Px e Py
void CorrigeNovaPosicaoDeP(void)
{
    VerificaPosicao();

    if (VerificaSeEstouNaMesmaPosicaoAnterior() == 1)
    {
        // Se estiver na mesma linha ou coluna de B ele não corrige
        if (Px < Bx)
        {
            Px++;
        }
        if (Px > Bx)
        {
            Px--;
        }
        if (Py < By)
        {
            Py++;
        }
        if (Py > By)
        {
            Py--;
        }
    }
};

// Calcula o caminho e desvio do buraco
void CalculaCaminho(void)
{
    contCaminho = 0;  // contador do caminho
    int terminei = 0; // Recebe 1 quando termina o cálculo

    // Inicia na posição A
    Py = Ay;
    Px = Ax;

    // Grava posicao inicial
    CaminhoX[contCaminho] = Px;
    CaminhoY[contCaminho] = Py;

    while (terminei == 0)
    {
        // Corrige nova posição até chegar em B
        CorrigeNovaPosicaoDeP();

        // Grava na variável caminho
        contCaminho++;
        CaminhoX[contCaminho] = Px;
        CaminhoY[contCaminho] = Py;

        // Terminei
        if (Px == Bx && Py == By)
        {
            terminei = 1;
        }
    }
};

// GoToXY eh uma funcao que pula o cursor para uma posicao da tela
void GoToXY(int x, int y)
{
    printf("%c[%d;%df", 0x1B, y, x);
    return;
};

// Imprime o mapa com os pontos A, B, Buraco e caminho
void ApresentaMapaDaReta(void)
{
    cls(); // limpa a tela antes

    // Imprime o caminho
    for (int cont = 0; cont <= contCaminho; cont++)
    {
        GoToXY(CaminhoX[cont], CaminhoY[cont]);
        printf("0");
    }

    // Imprime o ponto A
    GoToXY(Ax, Ay);
    printf("A");

    // Imprime o ponto B
    GoToXY(Bx, By);
    printf("B");

    // Imprime o ponto Buraco
    GoToXY(Cx, Cy);
    printf("C");
};

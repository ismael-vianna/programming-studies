// Tic Tac Toe ESP v1
// Jogo da velha entre cliente e servidor.
// A comunicação é feita por bluetooth entre Celular Android e o ESP32.
// ESP32 assume o papel de servidor. Ele registra o jogo, verifica quem ganhou e joga a partida contra o cliente.
// Versão Android: Programa desenvolvido no App Inventor.
// Versão ESP32: Programa desenvolvido em Arduino IDE.
// Desenvolvido por Ismael Soller Vianna.
// Disciplina Sistemas Distribuidos 2022/2

// Estrutura do string de envio de ambos os lados:
// Exemplo: "r:0;s:0;w:0;p:33"
// r: reset_game
// s: player_shift
// w: who_wins
// p: pos_??


#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

int PLAYER_O = 1;  // Cliente
int PLAYER_X = 2;  // Servidor

String btMsgClient;                                                          // Mensagem do cliente
String btMsgServ;                                                            // Mensagem para o cliente
int player_shift;                                                            // X=1, O=0 - turno atual
int reset_game;                                                              // 0=reset, 1=playing
int pos_11, pos_12, pos_13, pos_21, pos_22, pos_23, pos_31, pos_32, pos_33;  // Pos_??=0 (vazio), Pos_??=1 (O), Pos_??=2 (X)
int who_wins;                                                                // playing:0; O:1; X:2; KO:3
int server_choice;                                                           // Atual escolha do servidor para qual posição vai jogar
int casasDisponiveis;                                                        // Casas vazias e disponiveis para escolha do servidor. 0 a 8 posicoes

void setup() {
  Serial.begin(115200);
  SerialBT.begin("TicTacToeSrv");  // Nome do dispositivo Bluetooth device
  RestartGame();
  btMsgServ = MontaMsgParaCliente();
  Serial.println("Servidor iniciado, agora pode pareá-lo com bluetooth!");
}

void loop() {
  Serial.print("Cliente:");
  Serial.println(btMsgClient);

  Serial.print("Servidor:");
  Serial.println(btMsgServ);

  // Envia mensagem ao cliente
  EnviandoMensagemAoCLiente();

  // Recebe mensagem do cliente
  if (SerialBT.available()) {
    String btMsgClient_new = SerialBT.readString();

    if (btMsgClient_new.compareTo(btMsgClient) != 0) {
      // Atualiza mensagem do cliente
      btMsgClient = btMsgClient_new;

      // Computa cliente (registra decisao do cliente)
      ComputaDecisaoCliente();

      // Computa servidor (registra decisao do servidor)
      ComputaDecisaoServidor();

      // Monta mensagem ao cliente
      // Verifica quem ganhou o jogo em cada rodada
      if (VerificaQuemGanhou(PLAYER_O) == PLAYER_O) {
        who_wins = PLAYER_O;
      } else {
        if (VerificaQuemGanhou(PLAYER_X) == PLAYER_X) {
          who_wins = PLAYER_X;
        } else {
          who_wins = 0;
        }
      }

      if (casasDisponiveis == 0 && who_wins == 0) {
        // KO: Empatou
        who_wins = 3;
      }

      btMsgServ = MontaMsgParaCliente();
    }
  }

  delay(100);
}

// Método que envia mensagem ao Cliente
void EnviandoMensagemAoCLiente() {
  uint8_t buf[btMsgServ.length()];
  memcpy(buf, btMsgServ.c_str(), btMsgServ.length());
  SerialBT.write(buf, btMsgServ.length());
  SerialBT.println();
}

// Método de reinicio do jogo
void RestartGame() {
  btMsgClient = "";
  btMsgServ = "";
  player_shift = 0;
  reset_game = 0;
  who_wins = 0;
  server_choice = 99;    // Esta é uma posicao fora do tabuleiro pq o usuário não jogou ainda
  casasDisponiveis = 9;  // Todas as casas liberadas

  // Zera todas as casas do tabuleiro
  pos_11 = 0;
  pos_12 = 0;
  pos_13 = 0;
  pos_21 = 0;
  pos_22 = 0;
  pos_23 = 0;
  pos_31 = 0;
  pos_32 = 0;
  pos_33 = 0;
}

// Método de montagem da mensagem ao cliente.
String MontaMsgParaCliente() {
  //estrutura do string de envio:
  //r:0;s:0;w:0;p:33
  //r:0s:0w:0p:33
  //r: reset_game
  //s: player_shift
  //w: who_wins
  //p: server_choice

  String msgServ = "r:";
  msgServ += reset_game;

  msgServ += ";s:";
  msgServ += player_shift;

  msgServ += ";w:";
  msgServ += who_wins;

  msgServ += ";p:";

  if (server_choice > 9) {
    msgServ += server_choice;
  } else {
    msgServ += "99"; // Em caso de erro retorna uma caso fora do tabuleiro
  }

  return msgServ;
}

// Método que registra a decisão do cliente
void ComputaDecisaoCliente() {
  //estrutura do string de envio:
  //r:0;s:0;w:0;p:33
  //r: reset_game
  //s: player_shift
  //w: who_wins
  //p: server_choice

  int decisao_cliente = btMsgClient.substring(14, 16).toInt();
  reset_game = btMsgClient.substring(2, 3).toInt();
  player_shift = btMsgClient.substring(6, 7).toInt();

  Serial.print("reset_game_client: ");
  Serial.println(reset_game);

  if (reset_game == 0) {
    RestartGame();
  } else {
    if (VerificaSePosicaoEstaOcupada(decisao_cliente) == false) {
      //Registra posição do cliente
      RegistraPosicao(decisao_cliente, PLAYER_O);  //Cliente é o O, servidor é o X
    }
  }
}

// Método de decisão do jogo do lado do servidor
void ComputaDecisaoServidor() {
  player_shift = PLAYER_O - 1;  // próximo a jogar é o cliente
  if (who_wins == 0 && reset_game == 1) {
    long decisao_servidor = random(0, casasDisponiveis);  //decide randomicamente    
    server_choice = ProximaPosicaoLivre(decisao_servidor);  //retorna a posicao escolhida

    //Registra posição do cliente
    RegistraPosicao(server_choice, PLAYER_X);  //Cliente é o O, servidor é o X
  }
}

// Retorna a casa que está livre, baseada na posição "qualPosicao". QualPosicao deve ser de 0 a 8, Pq são 9 casas.
// método usado para escolha de casa no jogo do servidor.
int ProximaPosicaoLivre(int qualPosicao) {
  int posicoesLivres[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  int indexPos = 0;

  if (VerificaSePosicaoEstaOcupada(11) == false) {
    posicoesLivres[indexPos] = 11;  // Casa 11 está livre.
    indexPos++;                     // Ponteiro para próxima casa a ser gravada. Esta posicao é a identificacao da casa, como 11, 12, 13. por exemplo.
  }
  if (VerificaSePosicaoEstaOcupada(12) == false) {
    posicoesLivres[indexPos] = 12;
    indexPos++;
  }
  if (VerificaSePosicaoEstaOcupada(13) == false) {
    posicoesLivres[indexPos] = 13;
    indexPos++;
  }
  if (VerificaSePosicaoEstaOcupada(21) == false) {
    posicoesLivres[indexPos] = 21;
    indexPos++;
  }
  if (VerificaSePosicaoEstaOcupada(22) == false) {
    posicoesLivres[indexPos] = 22;
    indexPos++;
  }
  if (VerificaSePosicaoEstaOcupada(23) == false) {
    posicoesLivres[indexPos] = 23;
    indexPos++;
  }
  if (VerificaSePosicaoEstaOcupada(31) == false) {
    posicoesLivres[indexPos] = 31;
    indexPos++;
  }
  if (VerificaSePosicaoEstaOcupada(32) == false) {
    posicoesLivres[indexPos] = 32;
    indexPos++;
  }
  if (VerificaSePosicaoEstaOcupada(33) == false) {
    posicoesLivres[indexPos] = 33;
  }

  Serial.print("ProximaPosicaoLivre:indexPos: ");
  Serial.println(indexPos);

  Serial.print("ProximaPosicaoLivre:qualPosicao: ");
  Serial.println(qualPosicao);

  Serial.print("ProximaPosicaoLivre:posicoesLivres[qualPosicao]: ");
  Serial.println(posicoesLivres[qualPosicao]);

  if (qualPosicao > -1 && qualPosicao < 9) {
    return posicoesLivres[qualPosicao];
  }
  return posicoesLivres[0];
}

// Consulta se a posição/casa está ocupada. As posições são: 11, 12, 13, 21, 22, 23, 31, 32, 33
bool VerificaSePosicaoEstaOcupada(int pos) {
  //Pos_??=0 (vazio), Pos_??=1 (O), Pos_??=2 (X)

  if ((pos == 11 && pos_11 == 0) || (pos == 12 && pos_12 == 0) || 
  (pos == 13 && pos_13 == 0) || (pos == 21 && pos_21 == 0) || 
  (pos == 22 && pos_22 == 0) || (pos == 23 && pos_23 == 0) || 
  (pos == 31 && pos_31 == 0) || (pos == 32 && pos_32 == 0) || 
  (pos == 33 && pos_33 == 0)) {
    return false;  // Náo está ocupada
  }
  return true;  // Está ocupada
}

// Grava a posicao do jogador e atualiza as casas disponiveis
//Pos_??=1 (O), Pos_??=2 (X)
void RegistraPosicao(int pos, int jogador) {

  // Serial.print("RegistraPosicao:pos: ");
  // Serial.println(pos);

  // Serial.print("RegistraPosicao:jogador: ");
  // Serial.println(jogador);

  // Serial.print("RegistraPosicao:casasDisponiveis: ");
  // Serial.println(casasDisponiveis);

  // if ((pos == 11 || pos == 12 || pos == 13 ||
  //     pos == 21 || pos == 22 || pos == 23 ||
  //     pos == 31 || pos == 32 || pos == 33) && casasDisponiveis > 0){

  if (casasDisponiveis > 0) {

    casasDisponiveis--;

    if (pos == 11) {
      pos_11 = jogador;
    }
    if (pos == 12) {
      pos_12 = jogador;
    }
    if (pos == 13) {
      pos_13 = jogador;
    }
    if (pos == 21) {
      pos_21 = jogador;
    }
    if (pos == 22) {
      pos_22 = jogador;
    }
    if (pos == 23) {
      pos_23 = jogador;
    }
    if (pos == 31) {
      pos_31 = jogador;
    }
    if (pos == 32) {
      pos_32 = jogador;
    }
    if (pos == 33) {
      pos_33 = jogador;
    }
  }
}

// Testa todas as 8 opções possíveis de ganhar: 3xHorizontal, 3xVertical, 2xCruzado
// int who_wins; //playing:0; O:1; X:2; KO:3
int VerificaQuemGanhou(int jogador) {

  Serial.print("VerificaQuemGanhou:jogador: ");
  Serial.println(jogador);

  if ((pos_11 == jogador && pos_12 == jogador && pos_13 == jogador) || 
  (pos_21 == jogador && pos_22 == jogador && pos_23 == jogador) || 
  (pos_31 == jogador && pos_32 == jogador && pos_33 == jogador) || 
  (pos_11 == jogador && pos_21 == jogador && pos_31 == jogador) || 
  (pos_12 == jogador && pos_22 == jogador && pos_32 == jogador) || 
  (pos_13 == jogador && pos_23 == jogador && pos_33 == jogador) || 
  (pos_11 == jogador && pos_22 == jogador && pos_33 == jogador) || 
  (pos_13 == jogador && pos_22 == jogador && pos_31 == jogador)) {
    return jogador;  // Ganhou
  }
  return 0;  // Jogador verificado náo ganhou
}
# Jogo de Xadrez em C++ com SFML

Projeto de xadrez com interface gráfica moderna e responsiva, arquitetura orientada a objetos e separação clara entre lógica de jogo e renderização.

## Funcionalidades implementadas

- Tabuleiro 8x8 renderizado com layout adaptativo
- Interface redimensionável com painel de status lateral
- Seleção de peça com destaque visual
- Destaque de jogadas válidas (incluindo capturas)
- Movimento por clique (origem e destino)
- Alternância correta de turnos
- Validação de jogadas ilegais
- Captura de peças
- Detecção de xeque
- Detecção de xeque-mate
- Detecção de afogamento (stalemate)
- Bloqueio de jogadas que deixem o próprio rei em xeque
- Mensagens visuais de status e erro
- Indicação do turno atual
- Botão para reiniciar partida
- Roque (rei e torre sem movimentação prévia + casas livres e não atacadas)
- Promoção de peão com escolha visual (Rainha, Torre, Bispo, Cavalo)
- Destaque visual do rei em xeque

> Observação: a versão atual usa representação visual de peças com token + letra (`P`, `R`, `N`, `B`, `Q`, `K`) para não depender de assets externos.

## Requisitos

- `g++` com suporte a C++17
- CMake 3.16+
- SFML (graphics, window, system)

## Instalação da SFML

### Ubuntu / Debian

```bash
sudo apt update
sudo apt install libsfml-dev
```

### Fedora

```bash
sudo dnf install SFML-devel
```

### Arch Linux

```bash
sudo pacman -S sfml
```

## Compilar

```bash
cmake -S . -B build
cmake --build build -j
```

## Executar

```bash
./build/chess_game
```

## Estrutura do projeto

```text
.
├── CMakeLists.txt
├── README.md
├── assets/
├── include/
│   ├── Board.h
│   ├── Game.h
│   ├── Piece.h
│   └── Types.h
└── src/
    ├── Board.cpp
    ├── Game.cpp
    ├── Piece.cpp
    └── main.cpp
```

## Visão geral das classes

- **Game**: ciclo principal da aplicação, eventos do mouse, renderização da UI, responsividade do layout e painel de status.
- **Board**: estado do tabuleiro, validação de regras, geração de jogadas legais, controle de turno, xeque/xeque-mate/afogamento, roque e promoção.
- **Piece (abstrata)**: interface comum para peças de xadrez.
- **Pawn / Rook / Knight / Bishop / Queen / King**: geração de movimentos pseudo-legais por tipo.
- **Types**: tipos básicos reutilizáveis (`Position`, `Move`, enums de cor e tipo).

## Boas práticas aplicadas

- Código separado em `.h` e `.cpp`
- Responsabilidades bem definidas por classe
- Evita lógica concentrada em `main.cpp`
- API coesa para lógica de regras
- Base pronta para expansão (IA, histórico, animações, temas, sprites)

## Melhorias futuras sugeridas

- En passant
- Histórico e notação PGN simplificada
- Animação de movimento de peças
- Efeitos sonoros
- Menu inicial com botão "Jogar"
- Carregamento de sprites reais em `assets/`
- Modo jogador vs IA


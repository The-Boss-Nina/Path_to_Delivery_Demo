# Path to Delivery — Entrega 30%

Projeto desenvolvido para a disciplina de Desenvolvimento de Jogos usando **C++** e **SDL2**.

Nesta versão inicial, o jogo implementa a base dos **30%** da proposta:

- renderização do mapa urbano da primeira fase;
- renderização do jogador;
- movimentação básica;
- câmera acompanhando o jogador;
- seleção de veículo antes de iniciar a fase.

## Controles

Na tela de seleção:

- `1` — Harley
- `2` — Vespa
- `3` — Buggy
- `ESC` — sair

Durante o jogo:

- `W`, `A`, `S`, `D` ou setas — movimentar o veículo
- `ESC` — sair

## Veículos disponíveis

- **Harley**: mais rápida.
- **Vespa**: velocidade média.
- **Buggy**: mais lento, pensado para ser mais resistente em futuras entregas.

## Estrutura principal

```text
include/
src/
recursos/
├── img/
├── map/
├── audio/
└── font/
```

## Como executar

No MSYS2 UCRT64, dentro da pasta do projeto:

```bash
make run
```

Caso queira limpar o executável:

```bash
make clean
```

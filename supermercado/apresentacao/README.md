# Apresentação — Sistema de Supermercado e Filas de Caixa

Slides em `reveal.js` (arquivo HTML único) demonstrando um caso de uso completo do sistema.

## Conteúdo

| Arquivo | O que é |
|---|---|
| `slides.html` | A apresentação (12 slides). Reveal.js via CDN — precisa de internet na primeira carga. |
| `slides.pdf` | Export pronto, 12 páginas 13,33 × 7,5 in. |
| `demo.txt` | Roteiro de entradas usado no caso de uso (Ana, Bruno, Carla). |
| `saida-demo.txt` | Saída real do binário para esse roteiro (fonte dos blocos de terminal dos slides). |
| `exportar-pdf.sh` | Regera o PDF depois de editar os slides. |

## Abrir

```bash
xdg-open apresentacao/slides.html

# ou com um servidor local (evita restrições de file://)
python3 -m http.server 8000 -d apresentacao
# http://localhost:8000/slides.html
```

Navegação: `→` avança, `←` volta, `S` abre as notas do apresentador, `F` tela cheia,
`Esc` visão geral. `#/6` abre direto um slide.

## Reproduzir a demo ao vivo

```bash
make
./mercado < apresentacao/demo.txt
```

Ou digite `./mercado` (3 caixas) e siga o roteiro:

1. Cadastrar 101 Arroz 5kg, 102 Feijão 1kg e 103 Café 500g (estoque **3**)
2. Tentar cadastrar 101 de novo → erro de código duplicado
3. Listar produtos
4. Compra da **Ana** (2 arroz + 2 café) → caixa automático 1
5. Compra do **Bruno** (3 feijão) → caixa automático 2 (fila menor)
6. Compra da **Carla** (2 café) → caixa automático 3
7. Listar filas
8. Finalizar caixa 1 (Ana) → café cai de 3 para 1
9. Finalizar caixa 3 (Carla) → **bloqueado** por estoque, ela continua na fila
10. Finalizar caixa 2 (Bruno)
11. Histórico (pilha)
12. Relatório de vendas (matriz)

## Exportar para PDF

```bash
./apresentacao/exportar-pdf.sh      # chromium + ImageMagick
```

O script tira um screenshot por slide e monta o PDF — assim o resultado fica idêntico
ao que se vê no navegador (o modo `?print-pdf` do Reveal sai em branco no Chromium
headless).

Ao adicionar ou remover slides, ajuste `TOTAL=` no topo do script.

## Editar

Tudo em `apresentacao/slides.html`: CSS no `<style>` do topo, slides como
`<section>` dentro de `.reveal .slides`, e as falas de cada slide em
`<aside class="notes">`. As cores ficam nas variáveis CSS em `:root`.

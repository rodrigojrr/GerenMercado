# Apresentação — Sistema de Supermercado e Filas de Caixa

Slides em `reveal.js` (arquivo HTML único) demonstrando um caso de uso completo do sistema,
com os trechos de código de cada conceito destacados linha a linha.

## Conteúdo

| Arquivo | O que é |
|---|---|
| `slides.html` | A apresentação (17 slides). Reveal.js via CDN — precisa de internet na primeira carga. |
| `slides.pdf` | Export pronto, 17 páginas 13,33 × 7,5 in. |
| `demo.txt` | Roteiro de entradas usado no caso de uso (Ana, Bruno, Carla). |
| `saida-demo.txt` | Saída real do binário para esse roteiro (fonte dos blocos de terminal dos slides). |
| `exportar-pdf.sh` | Regera o PDF depois de editar os slides. |

## Estrutura dos slides

| # | Slide | Tipo |
|---|---|---|
| 1 | Título | — |
| 2 | O que o sistema faz | texto |
| 3 | Onde cada conceito aparece | tabela |
| 4 | `struct Mercado` | **código animado** |
| 5 | Fila — `enfileirar` | **código animado** |
| 6 | Pilha — `empilhar` | **código animado** |
| 7 | Recursão — `buscarProduto` | **código animado** + pilha de chamadas |
| 8 | Matriz — `totalVendidoProduto` | **código animado** |
| 9 | Compilar e rodar | terminal + tabela |
| 10 | Caso de uso | texto |
| 11–16 | Demo 1 a 5 e relatório final | saída real do programa |
| 17 | Conclusão | texto |

Nos slides de código, cada `→` avança uma etapa do destaque: o trecho inteiro aparece
primeiro e depois as linhas relevantes são acesas uma a uma (as demais ficam em 50% de
opacidade). O recurso é o `data-line-numbers` do Reveal, documentado em
<https://revealjs.com/code/>.

## Abrir

```bash
xdg-open apresentacao/slides.html

# ou com um servidor local (evita restrições de file://)
python3 -m http.server 8000 -d apresentacao
# http://localhost:8000/slides.html
```

Navegação: `→` avança (inclusive dentro das animações de código), `←` volta, `S` abre as
notas do apresentador, `F` tela cheia, `Esc` visão geral. `#/6` abre direto um slide.

## Reproduzir a demo ao vivo

```bash
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp -o mercado
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

Os screenshots usam a URL `slides.html?capture#/N`. O parâmetro `capture` faz o
`slides.html` mostrar **todos** os fragments de uma vez, então o PDF sai com o código
completo e as notas visíveis, sem depender das animações:

```bash
# ver como o slide vai sair no PDF
xdg-open "apresentacao/slides.html?capture#/4"
```

Ao adicionar ou remover slides, ajuste `TOTAL=` no topo do script (o total aparece
também no canto inferior direito de cada slide, como `4 / 17`).

## Editar

Tudo em `apresentacao/slides.html`: CSS no `<style>` do topo, slides como
`<section>` dentro de `.reveal .slides`, e as falas de cada slide em
`<aside class="notes">`. As cores ficam nas variáveis CSS em `:root`.

Para um trecho de código, use o componente `.code`; o destaque por etapa é o
`data-line-numbers` do Reveal, com as etapas separadas por `|`:

```html
<div class="code">
  <div class="bar"><i class="dot r"></i><i class="dot y"></i><i class="dot g"></i><span>main.cpp · funcao</span></div>
<pre><code data-trim class="language-cpp" data-line-numbers="1-14|2-4|6-8">/* código */</code></pre>
</div>
```

A primeira etapa costuma ser o bloco inteiro (`1-14`), para o trecho aparecer legível
assim que o slide entra; as seguintes destacam só as linhas da explicação. O
`<` do código é escrito como `&lt;`; o `>` pode ficar literal.

# Dungeon_Generator / Gerador de Masmorra

## English Explanation 🇺🇸
Just to clarify before i begin, all the images are in portuguese, i was designing this page just to portuguese speakers, but it's very simple to understand the  algorithm concept, so i guess it will not be a difficult thing to understand some images as i will explain those representations.

I'll add the English explanation in the next commit.

## Explicação em português 🇧🇷
Este é um projeto em C++ puro para a geração procedural de mapas de masmorras 2D. Ele utiliza o algoritmo Binary Space Partitioning (BSP) para criar layouts complexos e organizados de salas, corredores e portas.

O problema se baseia em uma necessidade de fazer jogos digitais do tipo dungeon crawlers e outros que utilizavam-se de mapas como aspecto fundamental da diversão terem experiências mais únicas, aumentar sua rejogabilidade, imprevisibilidade e no geral, sua qualidade como um todo.

Imagine que você tem que resolver esse problema: faça um algoritmo que gere uma masmorra com salas aleatórias numa grid de um tamanho qualquer, as salas devem ter tamanhos variados e todas as salas da masmorra devem estar conectadas, sem exceção, e com portas no início e fim de corredores para indicar a entrada e saída de uma sala.

Então são 3 passos principais: gerar as salas, conectar as salas com corredores e adicionar as portas.

### Primeiro passo 🥇
Talvez o primeiro pensamento que uma pessoa pode ter é gerar salas aleatoriamente até eu atingir a quantidade de salas desejadas. O problema dessa primeira abordagem é que na maioria das vezes, uma sala pode acabar sendo gerada uma por cima da outra, e desse modo, fazendo muitas tentativas falhas de geração, fazendo com que o algoritmo fique ineficiente. Veja um exemplo:

<img width="552" height="493" alt="sobreposição" src="https://github.com/user-attachments/assets/3681ef7d-8b7f-4b62-b6eb-4521f72fc4c3" /> <br>
Para resolver esse problema, utilizamos o BSP para dividir o espaço total da masmorra em 2 partes, e para cada parte, vamos gerar uma sala de tamanho aleatório seguindo a regra que ela tem que ser sempre menor que o tamanho da divisão dela. Veja um exemplo:

<img width="530" height="483" alt="exemplo de partição" src="https://github.com/user-attachments/assets/de54b154-3108-436d-8285-d28f5907dcba" /> <br>
Desse modo, é impossível que uma sala se sobreponha com outra, talvez possa ficar uma ao lado da outra, mas nunca sobreposta.

Se continuarmos a fazer esse processo, escolhendo entre divisões verticais e horizontais, em algumas iterações teremos algo desse tipo:

<img width="526" height="489" alt="particionamento" src="https://github.com/user-attachments/assets/2c76beb2-a501-49a2-810f-767df4061d11" /> <br>

O primeiro jogo que implementou um algortimo de BSP similar foi DOOM(1993), ele não usava BSP para gerar salas, mas usava para determinar quais elementos 3D a GPU deveria renderizar e em que ordem. Entretanto, Um jogo chamado Rogue(1980) que foi a inpiração para esse projeto, já contava com um algoritmo que gerava salas, corredores e portas aleatoriamente e de forma que fazia sentido, só não contava com o BSP para isso, e por esse motivo a grid do jogo era fixa em 3x3 sempre.

O resultado após a implementação do primeiro passo deve ser algo como isso:

<img width="638" height="644" alt="resultado primeiro passo" src="https://github.com/user-attachments/assets/050a039f-be18-4ce4-acaa-e16f536093fd" /> <br>

### Suco técnico 🍹 (opcional e recomendado pular caso você queira apenas o conceito geral)
Para os mais curiosos, todas as partições feitas são representadas por nós numa árvore binária, isso quer dizer que todos os estados até a partição final estão armazenadas nessa estrutura, mas como só vamos criar as salas no resultado final, usaremos apenas as menores partições, que são as folhas da árvore. Veja um exemplo:

<img width="444" height="368" alt="exemplo com nós" src="https://github.com/user-attachments/assets/9940fcf7-be07-4908-94ef-4e5631e29986" /> <br>
Para criar as partições de um jeito mais organizado, vamos adicionar os nós em uma estrutura de fila(queue), o motivo disso por trás disso depende do seu conhecimento com algoritmo de busca de árvores binárias, então saiba que exista o DFS(depth-first-search) e o BFS(breadth-first-search), o primeiro busca pelo nó mais profundo na árvore primeiro, já o segundo procura por cada nível da árvore em sequência. 

Para entender a próxima frase que eu vou falar, é necessário que você saiba sobre ponteiros em C++, vou dar uma visão geral aqui:

Ponteiros são variáveis que armazenam endereços de outras variável, eles são úteis quando você trabalha com estruturas de grafos. 

O problema comum deles é que como eles são armazenados numa memória diferente do resto da aplicação na maioria das vezes (memória heap ao invés da memória stack), você precisa manualmente deletar eles depois que não for mais usar, e isso é muito chato de fazer, por esse motivo, acabei usando ponteiros inteligentes que são deletados automaticamente quando necessário, então é uma coisa a menos para pensar, o problema deles é que somente um ponteiro pode ser o dono do endereço da variável apontada.

Enquanto na árvore binária, vamos colocar ponteiros únicos que referenciam os nós da árvore, na estrutura de fila, vamos colocar ponteiros brutos na stack que também referenciam os nodes, mas não são "donos" deles, evitando conflito com os ponteiros únicos.

Nesse caso, o que estou usando é o BFS, já que eu pretendo iterar por cada nível da árvore ao invés de iterar por partição, então no código, use uma variável <em>ITER</em> que define quantos níveis a árvore vai ter (ignorando a raiz).

As fórmulas gerais são: <br>

<em>
  NN = ITER+1 <br>
  NS = 2<sup> ITER</sup> <br>
</em> <br>

Onde <em>NN</em> é o número de níveis e <em>NS</em> é o número de salas geradas na masmorra. 

É importante que consigamos gerar o tamanho da sala baseada em uma porcentagem do tamanho total da partição. Para gerar a sala, multiplicamos o tamanho da partição por um número aleatório entre (0.5 - PARTITION_RANGE, 0.5 + PARTITION_RANGE), contanto que PARTITION_RANGE <= 0.5, o algoritmo funcionará perfeitamente.

OBS: Se PARTITION_RANGE for 0.5, existe uma pequena chance de ocorrer uma multiplicação por 0 que vai comprometer a geração daquela sala em específico, já que o tamanho dela vai ser 0, mas não quebra o código em termos de compilação 🙂

Quando geramos uma sala de tamanho aleatório, também precisamos de uma posição aleatória para ele estar, o problema é que dependendo do tamanho da sala, a raiz do retângulo dela pode estar em um lugar que faz a sala vazar de sua partição.

Então o valor máximo onde o retângulo da sala pode ser criado,  para x e y serão, respectivamente: <br>

<em>x_max = LP - LS</em> <br>
<em>y_max = AP - AS</em> <br>

Onde <em>LP e AP</em> é a largura e altura da partição, respectivamente, e <em>LS e AS</em> é a largura e altura da sala gerada.


Se você estiver prestando atenção, vai perceber que estamos multiplicando um número inteiro que representa o tamanho da masmorra, e multiplicando por um valor decimal, o que numa grid, não faria nenhum sentido representar um valor como 5,3 ou 12,8. Grids funcionam com números inteiros, então para representar o tamanho correto, simplesmente arredonde o valor obtido para o número inteiro mais próximo.

### Segundo passo 🥈
Agora que já geramos as salas para as masmorras, é necessário conectá-las através de corredores para que o jogador possa explorar entre elas de forma apropriada. Para isso, precisamos conectar algum ponto de uma sala com um ponto de outra sala, mas como vamos fazer isso exatamente? 🤔

Imagine que temos uma masmorra que contenha apenas 2 salas (para facilitar a explicação), e precisamos conectar as duas, vamos imaginar que os pontos escolhidos de cada sala seria o meio por exemplo. Veja como ficaria isso visualmente:

<img width="534" height="480" alt="pontos centrais" src="https://github.com/user-attachments/assets/e620e265-0faf-4511-812f-3bf9d1631bfb" /> <br>
As coordenadas dos pontos são arbitrárias, esse exemplo não demanda precisão, somente que você entenda como conectar 2 pontos numa grid de maneira inteligente.

Para conectar esses dois pontos, selecione um ponto intermediário entre os dois pontos, na maioria das vezes vão existir dois desses pontos intermediários, então escolha um deles aleatoriamente. Aqui, veja o exemplo:

<img width="531" height="475" alt="pontos médios" src="https://github.com/user-attachments/assets/59525361-387b-48f4-bc50-605ef26dc52c" /> <br>
Digamos que o ponto (12, 25) foi escolhido, ele será a nossa referência de quando parar o corredor em cada eixo da grid, pegamos os menores valores de x e y dentre os 3 pontos, e começaremos a criar um corredor para os dois eixos que começa dos menores valores de x e y e vai até o x e o y do nosso ponto de referência. Desse modo, garantimos que sempre exista uma conexão entre as duas salas. Veja como ficaria visualmente:

<img width="635" height="488" alt="conexão das salas" src="https://github.com/user-attachments/assets/2f57192f-c0ac-434b-b32e-ade7ebf70140" /> <br>
E com essa abordagem, agora temos duas salas conctadas, só que ainda tem um defeito nessa representação, caso a masmorra precise de mais de duas salas, não será possível conectar todas, temos que lembrar que esse código só conecta esses dois pontos, não mais que dois.

Imagine um caso onde uma partição maior tem mais de uma sala, teríamos mais de dois pontos para conectar no total e o nosso sistema deixaria de funcionar. Por esse motivo, para garantir que sempre teremos exatamente dois pontos, faça com que assim que o algoritmo termine uma conexão entre duas salas, ele usa o ponto médio como retorno para a próxima conexão com partições maiores, garantindo que sempre calcularemos de dois em dois.

Quando a repetição é feita para todos as partições da masmorra, o resultado que obtemos é algo parecido com isso:

<img width="638" height="642" alt="corredores completos" src="https://github.com/user-attachments/assets/8ff8fe9b-dbb8-4520-b50c-0848e60c7652" /> <br>

### Terceiro passo 🥉
O terceiro e último passo para concluir o algoritmo é a adição de portas na masmorra. Visualmente, o nosso cérebro sabe intuitivamente onde as portas deveriam estar, então se o nosso cérebro sabe uma lógica para gerar essas portas, o nosso algoritmo também pode. O padrão que eu notei (já que não achei quase documentação alguma sobre isso) é que em todos os lugares onde imaginamos portas, é sempre um quadrado onde ele é cercado por paredes na esquerda ou direita, ou cima e baixo. Veja um exemplo:

<img width="641" height="644" alt="lógica das portas" src="https://github.com/user-attachments/assets/274f7a8a-6997-46c4-a36f-6a5569330bab" /> <br>
Se você estiver prestando atenção, notou que existem algums quadrados da grid que atendem às condições que eu mencionei, mas que eu não adicionei portas pois não era apropriado, como no meio de corredores, afinal, só precisamos de duas portas, a que sai da primeira sala, e a que entra na segunda sala.

O jeito correto de fazer isso é checar se uma porta é válida no eixo, e se sim, seguimos até o final daquele corredor, o corredor acaba quando o quadrado que checarmos não atender mais às condições propostas. Desse modo fazendo com que saibamos quais quadrados que podemos e não podemos adicionar uma porta.

O resultado final é um algoritmo que atende a todos os requisitos necessários e de maneira otimizada.

### Suco técnico 🍹 (opcional e recomendado pular caso você queira apenas o conceito geral)
A maneira em que percorremos pelo corredor é primeiro checando em qual eixo o as paredes do quadrado está, e por conta da borda mínima que está configurada no código como <em>BORDER_SIZE</em>, é seguro checar os quadrados para todos os lados sem ter problema de index fora do alcance.

OBS: Pelo jeito que o código está feito e já que é a primeira versão postada aqui no github, eu esqueci de fazer validações para todos os casos possíveis de erro, por agora, caso for alterar as constantes do código, não deixe a variável <em>BORDER_SIZE</em> sendo <= 0, vou refatorar o código assim que possível.

Para melhorar a performance e funcionamento do código na checagem dos quadrados, precisamos salvar quais quadrados já foram explorados na grid. Dessa forma, garantimos que não estamos selecionando um mesmo quadrado num corredor que já exploramos, se não fizermos isso, ele vai criar portas desde o início até o final do corredor. Use um std::set<std::pair<int, int>> para que a busca de pontos seja O(1) ao invés de O(n).

## Resultado Final
<img width="637" height="643" alt="resultado final" src="https://github.com/user-attachments/assets/1a129b6d-4853-46d3-8275-83a7ead913d7" /> <br>
A grid é exibida diretamente no console, utilizando cores ANSI para diferenciar paredes (#), chão (.) e portas (+).

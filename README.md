# Dungeon_Generator / Gerador de Masmorra

## English Explanation 🇺🇸
Just to clarify before i begin, all the images are in portuguese, i was designing this page just to portuguese speakers, but it's very simple to understand the  algorithm concept, so i guess it will not be a difficult thing to understand some images as i will explain those representations.

This is a pure C++ project for the procedural generation of 2D dungeon maps. It uses the Binary Space Partitioning (BSP) algorithm to create complex and organized layouts of rooms, corridors, and doors.

The problem is based on the need for digital games like dungeon crawlers and others that use maps as a fundamental aspect of fun to have more unique experiences, increase their replayability, unpredictability, and overall quality.

Imagine you have to solve this problem: create an algorithm that generates a dungeon with random rooms on a grid of any size. The rooms must have varied sizes, and all rooms in the dungeon must be connected, without exception, with doors at the beginning and end of corridors to indicate the entrance and exit of a room.

So, there are 3 main steps: generating the rooms, connecting the rooms with corridors, and adding the doors.

### First step 🥇
Perhaps the first thought one might have is to randomly generate rooms until the desired number of rooms is reached. The problem with this first approach is that, most of the time, a room might end up being generated on top of another, leading to many failed generation attempts and making the algorithm inefficient. Here's an example:

<img width="552" height="493" alt="overlap" src="https://github.com/user-attachments/assets/3681ef7d-8b7f-4b62-b6eb-4521f72fc4c3" />


To solve this problem, we use BSP to divide the total dungeon space into 2 parts, and for each part, we will generate a randomly-sized room following the rule that it must always be smaller than the size of its partition. Here's an example:

<img width="530" height="483" alt="partition example" src="https://github.com/user-attachments/assets/de54b154-3108-436d-8285-d28f5907dcba" />


This way, it's impossible for one room to overlap another; they might be next to each other, but never overlapping.

If we continue this process, choosing between vertical and horizontal splits, after a few iterations we will have something like this:

<img width="526" height="489" alt="partitioning" src="https://github.com/user-attachments/assets/2c76beb2-a501-49a2-810f-767df4061d11" />


The first game to implement a similar BSP algorithm was DOOM (1993). It didn't use BSP to generate rooms, but to determine which 3D elements the GPU should render and in what order. However, a game called Rogue (1980), which was the inspiration for this project, already had an algorithm that generated rooms, corridors, and doors randomly and in a way that made sense, it just didn't use BSP for it, and for that reason, the game's grid was always fixed at 3x3.

The result after implementing the first step should be something like this:

<img width="638" height="644" alt="first step result" src="https://github.com/user-attachments/assets/050a039f-be18-4ce4-acaa-e16f536093fd" />


### Technical Juice 🍹 (optional and recommended to skip if you just want the general concept)
For the more curious, all partitions made are represented by nodes in a binary tree. This means that all states up to the final partition are stored in this structure, but since we will only create the rooms in the final result, we will only use the smallest partitions, which are the leaves of the tree. Here's an example:

<img width="444" height="368" alt="example with nodes" src="https://github.com/user-attachments/assets/9940fcf7-be07-4908-94ef-4e5631e29986" />


To create the partitions in a more organized way, we will add the nodes to a queue structure. The reason for this depends on your knowledge of binary tree search algorithms, so just know that there is DFS (depth-first-search) and BFS (breadth-first-search). The first searches for the deepest node in the tree first, while the second searches through each level of the tree in sequence.

To understand the next sentence I'm going to say, you need to know about pointers in C++. I'll give you a general overview here:

Pointers are variables that store the addresses of other variables. They are useful when working with graph structures.

The common problem with them is that since they are often stored in a different memory area than the rest of the application (heap memory instead of stack memory), you need to manually delete them after you are done using them, and this is very annoying to do. For this reason, I ended up using smart pointers, which are deleted automatically when necessary, so it's one less thing to think about. The issue with them is that only one pointer can be the owner of the address of the pointed-to variable.

While in the binary tree, we will put unique pointers that reference the tree nodes, in the queue structure, we will put raw pointers on the stack that also reference the nodes but do not "own" them, thus avoiding conflict with the unique pointers.

In this case, what I'm using is BFS, as I intend to iterate through each level of the tree instead of iterating by partition. So in the code, use a variable ITER that defines how many levels the tree will have (ignoring the root).

The general formulas are: <br>

<em> 
NN = ITER+1 <br>
NS = 2<sup> ITER</sup> <br> <br>
</em>

Where NN is the number of levels and NS is the number of rooms generated in the dungeon.

It is important that we can generate the room size based on a percentage of the total partition size. To generate the room, we multiply the partition size by a random number between (0.5 - PARTITION_RANGE, 0.5 + PARTITION_RANGE), as long as PARTITION_RANGE <= 0.5, the algorithm will work perfectly.

NOTE: If PARTITION_RANGE is 0.5, there is a small chance of a multiplication by 0 occurring, which will compromise the generation of that specific room, as its size will be 0, but it doesn't break the code in terms of compilation 🙂

When we generate a randomly-sized room, we also need a random position for it. The problem is that depending on the size of the room, the origin of its rectangle could be in a place that makes the room "leak" out of its partition.

So the maximum value where the room's rectangle can be created, for x and y respectively, will be:


<em>x_max = PW - RW</em> <br>
<em>y_max = PH - RH</em>


Where PW and PH are the partition's width and height, respectively, and RW and RH are the generated room's width and height.

If you're paying attention, you'll notice that we are multiplying an integer (representing the dungeon size) by a decimal value, which in a grid, wouldn't make any sense to represent a value like 5.3 or 12.8. Grids work with integers, so to represent the correct size, simply round the obtained value to the nearest integer.

### Second step 🥈
Now that we have generated the rooms for the dungeons, we need to connect them using corridors so the player can explore between them properly. For this, we need to connect a point from one room to a point from another room, but how exactly are we going to do that? 🤔

Imagine we have a dungeon that contains only 2 rooms (to simplify the explanation), and we need to connect them. Let's imagine the chosen points from each room are their centers, for example. Here's what that would look like visually:

<img width="534" height="480" alt="center points" src="https://github.com/user-attachments/assets/e620e265-0faf-4511-812f-3bf9d1631bfb" />


The coordinates of the points are arbitrary; this example doesn't require precision, only that you understand how to connect 2 points on a grid intelligently.

To connect these two points, select an intermediate point between them. Most of the time, there will be two such intermediate points, so choose one of them randomly. Here, see the example:

<img width="531" height="475" alt="midpoints" src="https://github.com/user-attachments/assets/59525361-387b-48f4-bc50-605ef26dc52c" />


Let's say the point (12, 25) was chosen. It will be our reference point for when to stop the corridor on each axis of the grid. We take the smallest x and y values among the 3 points, and we will start creating a corridor for both axes that starts from the smallest x and y values and goes up to the x and y of our reference point. This way, we guarantee that there is always a connection between the two rooms. Here's what it would look like visually:

<img width="635" height="488" alt="room connection" src="https://github.com/user-attachments/assets/2f57192f-c0ac-434b-b32e-ade7ebf70140" />


And with this approach, we now have two connected rooms. But there's still a flaw in this representation. If the dungeon needs more than two rooms, it won't be possible to connect them all. We have to remember that this code only connects these two points, not more than two.

Imagine a case where a larger partition has more than one room. We would have more than two points to connect in total, and our system would stop working. For this reason, to ensure we always have exactly two points, make it so that as soon as the algorithm finishes a connection between two rooms, it uses the midpoint as the return for the next connection with larger partitions, ensuring we always calculate two by two.

When this repetition is done for all partitions of the dungeon, the result we get is something like this:

<img width="638" height="642" alt="complete corridors" src="https://github.com/user-attachments/assets/8ff8fe9b-dbb8-4520-b50c-0848e60c7652" />


### Third step 🥉
The third and final step to complete the algorithm is adding doors to the dungeon. Visually, our brain intuitively knows where the doors should be, so if our brain knows a logic for generating these doors, our algorithm can too. The pattern I noticed (since I found almost no documentation on this) is that in all the places where we imagine doors, it's always a square that is surrounded by walls on the left or right, or top and bottom. Here's an example:

<img width="641" height="644" alt="door logic" src="https://github.com/user-attachments/assets/274f7a8a-6997-46c4-a36f-6a5569330bab" />


If you're paying attention, you'll notice that there are some squares on the grid that meet the conditions I mentioned, but where I didn't add doors because it wasn't appropriate, like in the middle of corridors. After all, we only need two doors: the one leaving the first room and the one entering the second room.

The correct way to do this is to check if a door is valid on the axis, and if so, follow it to the end of that corridor. The corridor ends when the square we check no longer meets the proposed conditions. This way, we know which squares we can and cannot add a door to.

The final result is an algorithm that meets all the necessary requirements in an optimized way.

### Technical Juice 🍹 (optional and recommended to skip if you just want the general concept)
The way we traverse the corridor is by first checking on which axis the square's walls are. Because of the minimum border configured in the code as BORDER_SIZE, it's safe to check the squares on all sides without having an index out of bounds problem.

NOTE: Due to the way the code is written and since this is the first version posted here on GitHub, I forgot to add validation for all possible error cases. For now, if you are going to change the constants in the code, do not set the BORDER_SIZE variable to be <= 0. I will refactor the code as soon as possible.

To improve the code's performance and functionality in checking the squares, we need to save which squares have already been explored on the grid. This way, we ensure we are not selecting the same square in a corridor that we have already explored. If we don't do this, it will create doors from the beginning to the end of the corridor. Use a std::set<std::pair<int, int>> so that the point lookup is O(1) instead of O(n).

# Final Result
<img width="637" height="643" alt="final result" src="https://github.com/user-attachments/assets/1a129b6d-4853-46d3-8275-83a7ead913d7" />

The grid is displayed directly in the console, using ANSI colors to differentiate walls (#), floors (.), and doors (+).

<br> <br> <br>

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

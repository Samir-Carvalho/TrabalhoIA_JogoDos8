/**
Inteligência Artificial
 - Samir Avelino Carvalho
 - Vitor de Paula Batista

  Exemplo de entrada para a matriz da solução do jogo:
1 2 3
8 0 4
7 6 5

  Exemplo de entrada para a matriz do jogo com os números embaralhados
3 4 2
5 1 7
6 0 8

Digite 1 para executar a busca em profundidade e 2 para a busca A*

**/

#include <cstdio>
#include <iostream>
#include <queue>
#include <string>
#include <stack>
#include <stdlib.h>
#include <set>
#include <chrono>
#include <cmath>
using namespace std;

static const int SIZE = 3;

struct Node
{
	int matriz;
	int zero_x;
	int zero_y;
	const Node *prev;
	int profundidade;
	int custo;
};

// matriz codificada em inteiro usando definindoEntrada e matrizDeEntrada

/*
0 1 2
3 4 5  ->  876543210
6 7 8
*/

int objetivo;
int objetivo_tabela[9][2];

int num_nos_vizitados = 0;
int num_gerada_nodes = 1;
int max_profundidade = 0;

chrono::time_point<chrono::system_clock> start;

vector<Node *> filhos;

static const int LOOKUP_tabela[] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000};

// matriz[i][j]
int matrizDeEntrada(int matriz, int i, int j)
{
	int index = i * SIZE + j;

	int divisor = LOOKUP_tabela[index]; // 10^index

	return (matriz / divisor) % 10;
}

// matriz[i][j] = k
void definindoEntrada(int &matriz, int i, int j, int k)
{
	int index = i * SIZE + j;

	int multiplier = LOOKUP_tabela[index]; // 10^index

	matriz = (matriz - matrizDeEntrada(matriz, i, j) * multiplier + k * multiplier);
}

void print(const Node *p)
{
	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
			cout << matrizDeEntrada(p->matriz, i, j) << " ";

		cout << endl;
	}
	cout << endl;
}

void print_solucao(const Node *no_final)
{
	chrono::time_point<chrono::system_clock> end;

	end = chrono::system_clock::now();

	chrono::duration<double> elapsed = end - start;

	int solucao_profundidade = no_final->profundidade;

	stack<const Node *> movimentos;

	const Node *p = no_final;

	while (p != NULL)
	{
		movimentos.push(p);
		p = p->prev;
	}

	int c = 0;

	while (!movimentos.empty())
	{
		print(movimentos.top());

		cout << endl;

		movimentos.pop();

		c++;
	}

	cout << endl
		 << "   "
		 << "Movimentos: " << c - 1 << endl
		 << endl;
	cout << "   "
		 << "Segundos: " << elapsed.count() << endl
		 << endl;
	cout << "   "
		 << "Nos Visitados: " << num_nos_vizitados << endl
		 << endl;
	cout << "   "
		 << "Nos Gerados: " << num_gerada_nodes << endl
		 << endl;
	cout << "   "
		 << "Profundidade da Solucao: " << solucao_profundidade << endl
		 << endl;
	cout << "   "
		 << "Profundidade Maxima: " << max_profundidade << endl
		 << endl;
}

bool possivel(int i, int j)
{
	return (i >= 0 && i < SIZE && j >= 0 && j < SIZE);
}

int calcular_custo(int matriz)
{
	int counter = 0;

	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			counter += abs(i - objetivo_tabela[matrizDeEntrada(matriz, i, j)][0]) + abs(j - objetivo_tabela[matrizDeEntrada(matriz, i, j)][1]);
		}
	}

	return counter;
}

void gerar_filhos(const Node *node)
{
	int is[4] = {0, 0, 1, -1};
	int js[4] = {1, -1, 0, 0};

	filhos.clear();

	for (int a = 0; a < 4; a++)
	{
		if (possivel(node->zero_x + is[a], node->zero_y + js[a]))
		{
			Node *filho = (Node *)malloc(sizeof(Node));
			num_gerada_nodes++;

			filho->matriz = node->matriz;

			definindoEntrada(filho->matriz, node->zero_x, node->zero_y, matrizDeEntrada(filho->matriz, node->zero_x + is[a], node->zero_y + js[a]));
			definindoEntrada(filho->matriz, node->zero_x + is[a], node->zero_y + js[a], 0);

			filho->zero_x = node->zero_x + is[a];
			filho->zero_y = node->zero_y + js[a];
			filho->prev = node;
			filho->profundidade = node->profundidade + 1;

			if (filho->profundidade > max_profundidade)
			{
				max_profundidade = filho->profundidade;
			}

			filhos.push_back(filho);
		}
	}
}

void dfs(Node *inicial) // Busca em profundidade
{
	stack<Node *> node_stack;
	node_stack.push(inicial);

	while (!node_stack.empty())
	{
		Node *current = node_stack.top();
		num_nos_vizitados++;

		if (current->matriz == objetivo)
		{
			print_solucao(current);
			return;
		}
		else
		{
			node_stack.pop();

			gerar_filhos(current);

			for (unsigned int i = 0; i < filhos.size(); i++)
			{
				const Node *parent = current->prev;

				while (parent != NULL)
				{
					if (parent->matriz == filhos[i]->matriz)
					{
						break;
					}

					parent = parent->prev;
				}

				if (parent == NULL)
					node_stack.push(filhos[i]);
			}
		}
	}
}

auto cmp = [](Node *left, Node *right)
{ return (left->custo) > (right->custo); }; // funçao para inserir o node com menor custo no topo da pq

void aEstrela(Node *inicial) // busca A*
{
	inicial->custo = calcular_custo(inicial->matriz);

	priority_queue<Node *, vector<Node *>, decltype(cmp)> node_pq(cmp);
	node_pq.push(inicial);

	while (!node_pq.empty())
	{
		Node *current = node_pq.top();
		num_nos_vizitados++;

		if (current->matriz == objetivo)
		{
			print_solucao(current);
			return;
		}
		else
		{
			node_pq.pop();

			gerar_filhos(current);

			for (unsigned int i = 0; i < filhos.size(); i++)
			{
				const Node *parent = current->prev;

				while (parent != NULL)
				{
					if (parent->matriz == filhos[i]->matriz)
					{
						break;
					}

					parent = parent->prev;
				}

				if (parent == NULL)
				{
					filhos[i]->custo = filhos[i]->profundidade + calcular_custo(filhos[i]->matriz);
					node_pq.push(filhos[i]);
				}
			}
		}
	}
}

int paridade(int arr)
{
	int count = 0;

	for (int i = 0; i < SIZE * SIZE - 1; i++)
	{
		for (int j = i + 1; j < SIZE * SIZE; j++)
		{
			if (matrizDeEntrada(arr, j / SIZE, j % SIZE) &&
				matrizDeEntrada(arr, i / SIZE, i % SIZE) &&
				matrizDeEntrada(arr, i / SIZE, i % SIZE) > matrizDeEntrada(arr, j / SIZE, j % SIZE))
			{
				count++;
			}
		}
	}

	return count;
}

bool is_possivel(int inicial, int objetivo)
{
	int countInicial = paridade(inicial);
	int countObjetivo = paridade(objetivo);

	return (countInicial % 2 == countObjetivo % 2);
}

int main()
{
	cout << "Digite a matriz da solução do jogo dos oitos: " << endl;
	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			int k;
			scanf("%d", &k);

			objetivo_tabela[k][0] = i; // guarda as coordenadas de cada numero do objetivo para depois utilizar na funcao calcular_custo
			objetivo_tabela[k][1] = j;

			definindoEntrada(objetivo, i, j, k);
		}
	}

	Node inicial;
	cout << "Digite como a matriz do jogo esta embaralhada: " << endl;
	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			int k;
			scanf("%d", &k);

			if (k == 0)
			{
				inicial.zero_x = i;
				inicial.zero_y = j;
			}

			definindoEntrada(inicial.matriz, i, j, k);
		}
	}

	inicial.profundidade = 0;
	inicial.prev = NULL;
	inicial.custo = 0;

	cout << "Metodo de busca (1 - DFS, 2 - A*): ";

	int search;
	scanf("%d", &search);

	if (is_possivel(inicial.matriz, objetivo))
	{

		start = chrono::system_clock::now();
		if (search == 1)
		{
			dfs(&inicial);
		}
		else if (search == 2)
		{
			aEstrela(&inicial);
		}
		else
		{
			cout << "Metodo de busca invalido." << endl;
		}
	}
	else
		cout << "Nao ha soluçao!" << endl;

	return 0;
}
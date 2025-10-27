#include <iostream>
#include <vector>
#include <random>
#include <memory>
#include <cmath>
#include <queue>
#include <cstdlib>
#include <algorithm>
#include <set>

#define ANSI_COLOR_RED     "\x1b[31m" 
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RESET    "\x1b[0m"

std::random_device rd; // Gerador não determinístico
std::mt19937 gen(rd()); // Mersenne Twister para alta qualidade
std::vector<std::vector<char>> GRID;
int    const ROWS = 40;
int    const COLS = 40;
int    const ITER = 4; // 4

// Não deixe que o produto da largura e altura mínima ultrapasse o tamanho mínimo da partição, caso contrário, todos as salas serão do tamanho da partição
int    const MIN_PARTITION_SIZE = 10;
int    const MIN_ROOM_WIDTH = 2;
int    const MIN_ROOM_HEIGHT = 2;
int    const BORDER_SIZE = 1;

double const PARTITION_RANGE = 0.2;
double const MIN_ROOM_SIZE_PERCENTAGE = 0.6;
double const MAX_ROOM_SIZE_PERCENTAGE = 0.7;

char   const WALL_TILE = '#';
char   const GROUND_TILE = '.';
char   const DOOR_TILE = '+';

void static fill_grid(std::vector<std::vector<char>>& grid, int rows, int cols, char const tile) {
	for (int i = 0; i < rows; i++) {
		grid.push_back({});
		for (int j = 0; j < cols; j++) {
			grid[i].push_back(tile);
		}
	}
}

void static display_grid(std::vector<std::vector<char>>& grid) {
	for (int i = 0; i < grid.size(); i++) {
		for (int j = 0; j < grid[i].size(); j++) {
			if (grid[i][j] == WALL_TILE) {
				std::cout << ANSI_COLOR_RED << grid[i][j] << " " << ANSI_COLOR_RESET;
			}
			else if (grid[i][j] == DOOR_TILE) {
				std::cout << ANSI_COLOR_YELLOW << grid[i][j] << " " << ANSI_COLOR_RESET;
			}
			else {
				std::cout << grid[i][j] << " ";
			}
		}
		std::cout << "\n";
	}
}

struct Rectangle {
	int x, y, width, height;
	
	Rectangle()
		: x(0), y(0), width(0), height(0) {}
	
	Rectangle(int x, int y, int width, int height) 
	: x(x), y(y), width(width), height(height) {}

};

struct Node {
	Rectangle partition;
	Rectangle room;
	std::unique_ptr<Node> child_left = nullptr;
	std::unique_ptr<Node> child_right = nullptr;
	int depth;

	Node() : depth(0) {}

	bool const Is_leaf() {
		return (child_left == nullptr && child_right == nullptr);
	}

	int const Get_size() {
		return partition.width * partition.height;
	}

};

std::pair<int, int> static create_corridors(std::unique_ptr<Node>& node) {
	std::uniform_int_distribution<> shape_l(0, 1);
	std::uniform_real_distribution<> startp(1, 2);

	if (node->Is_leaf()) {
		if (shape_l(gen)) {
			return { node->room.x + node->room.width / 2 , node->room.y + node->room.height / 2 };
		}
		return { node->room.x + node->room.width / startp(gen) , node->room.y + node->room.height / startp(gen)};
	}
	std::pair<int, int> left_point = create_corridors(node->child_left);
	std::pair<int, int> right_point = create_corridors(node->child_right);

	std::uniform_int_distribution<> align(0, 1); // 0 para alinhamento em X, 1 para alinhamento em Y
	int align_type = align(gen);
	std::pair<int, int> connection = { 0,0 };

	int start_x = 0, start_y = 0, target_x = 0, target_y = 0;

	if (align_type) {
		connection.first = right_point.first;
		connection.second = left_point.second;
	}
	else {
		connection.first = left_point.first;
		connection.second = right_point.second;
	}
	// I = ROW; J = COL

	target_x = std::max(left_point.first, right_point.first);
	target_x = std::max(connection.first, target_x);

	target_y = std::max(left_point.second, right_point.second);
	target_y = std::max(connection.second, target_y);

	start_x = std::min(left_point.first, right_point.first);
	start_x = std::min(connection.first, start_x);

	start_y = std::min(left_point.second, right_point.second);
	start_y = std::min(connection.second, start_y);
	

	for (int i = start_x; i <= target_x; i++) {
		GRID[connection.second][i] = GROUND_TILE;

	}

	for (int j = start_y; j <= target_y; j ++) {
		GRID[j][connection.first] = GROUND_TILE;
		
	}
	return connection;
}

void static create_doors(std::vector<std::vector<char>>& grid, int start_x, int start_y, int size_col, int size_row) {
	std::set<std::pair<int, int>> viewed;
	for (int i = start_x; i < size_col; i++) {
		for (int j = start_y; j < size_row; j++) {
			
			if (grid[i + 1][j] == WALL_TILE && grid[i - 1][j] == WALL_TILE && grid[i][j] == GROUND_TILE) {

				if (viewed.count({ i, j })) {
					continue;
				}

				int pointer_x = j;
				while (grid[i + 1][pointer_x] == WALL_TILE && grid[i - 1][pointer_x] == WALL_TILE && grid[i][j] == GROUND_TILE) {
					viewed.insert({ i, pointer_x });
					pointer_x++;
				}

				grid[i][j] = DOOR_TILE;
				grid[i][pointer_x - 1] = DOOR_TILE;
			}

			else if (grid[i][j + 1] == WALL_TILE && grid[i][j - 1] == WALL_TILE && grid[i][j] == GROUND_TILE) {

				if (viewed.count({ i, j })) {
					continue;
				}

				int pointer_y = i;
				while (grid[pointer_y][j + 1] == WALL_TILE && grid[pointer_y][j - 1] == WALL_TILE && grid[i][j] == GROUND_TILE) {
					viewed.insert({ pointer_y, j });
					pointer_y++;
				}

				grid[i][j] = DOOR_TILE;
				grid[pointer_y - 1][j] = DOOR_TILE;
			}
		}
	}
}

void static build_room(std::unique_ptr<Node>& node, std::vector<std::vector<char>>& grid, char const tile) {
	int rows = node->room.width + node->room.x;
	int cols = node->room.height + node->room.y;
	int start_row = node->room.y;
	int start_col = node->room.x;

	for (int i = start_row; i < cols; i++) {
		for (int j = start_col; j < rows; j++) {
			grid[i][j] = tile;
		}
	}
}

void static create_room_block(std::unique_ptr<Node>& node) {
	if (node->Is_leaf()) {
		std::uniform_real_distribution<> room_size_percentage(MIN_ROOM_SIZE_PERCENTAGE, MAX_ROOM_SIZE_PERCENTAGE);

		double room_width = room_size_percentage(gen);
		double room_height = room_size_percentage(gen);


		int width = (int)round(node->partition.width * room_width);
		int height = (int)round(node->partition.height * room_height);

		node->room.width = (width >= MIN_ROOM_WIDTH) ? width : node->partition.width;
		node->room.height = (height >= MIN_ROOM_HEIGHT) ? height : node->partition.height;

		// Definir aonde a origem de room é possível
		int max_x = node->partition.width - width;
		int max_y = node->partition.height - height;

		std::uniform_int_distribution<> x(node->partition.x, node->partition.x + max_x);
		std::uniform_int_distribution<> y(node->partition.y, node->partition.y + max_y);

		node->room.x =  x(gen);
		node->room.y =  y(gen);
		build_room(node, GRID, GROUND_TILE);
		return;
	}

	create_room_block(node->child_left);
	create_room_block(node->child_right);
}

void static partition(std::queue<Node*>& node_queue, int& max_depth, int& counter) {
	if (node_queue.empty()) {
		return;
	}
	counter++;

	Node* node = node_queue.front();

	node_queue.pop();

	int node_depth = node->depth;


	if (node_depth > max_depth) {
		max_depth = node_depth;
	}

	int left_partition_size = 0, right_partition_size = 0;

	std::uniform_real_distribution<> dist((0.5-PARTITION_RANGE), (0.5 + PARTITION_RANGE)); // Intervalo da partição
	std::uniform_int_distribution<> hor_ver(0, 1); // Horizontal = 0; Vertical = 1;
	int partition_type = hor_ver(gen);
	bool equal = (node->partition.width == node->partition.height);

	std::unique_ptr<Node> left = std::make_unique<Node>();
	std::unique_ptr<Node> right = std::make_unique<Node>();

	if ((node->partition.width > node->partition.height) || (equal && partition_type)) { // Partição Vertical
		left_partition_size = (int)round(node->partition.width * dist(gen));
		right_partition_size = node->partition.width - left_partition_size;
		left->partition = Rectangle(node->partition.x, node->partition.y, left_partition_size, node->partition.height);
		right->partition = Rectangle(node->partition.x + left_partition_size, node->partition.y, right_partition_size, node->partition.height);

	}
	else if ((node->partition.width < node->partition.height) || (equal && !partition_type)) { // Partição Horizontal
		left_partition_size = (int)round(node->partition.height * dist(gen));
		right_partition_size = node->partition.height - left_partition_size;
		left->partition = Rectangle(node->partition.x, node->partition.y, node->partition.width, left_partition_size);
		right->partition = Rectangle(node->partition.x, node->partition.y + left_partition_size, node->partition.width, right_partition_size);

	}
	else {
		std::cout << "PARTITION ERROR";
	}

	if (max_depth < ITER && left->Get_size() >= MIN_PARTITION_SIZE && right->Get_size() >= MIN_PARTITION_SIZE){
		left->depth = node_depth + 1;
		right->depth = node_depth + 1;

		node_queue.push(left.get());
		node_queue.push(right.get());

		node->child_left = std::move(left);
		node->child_right = std::move(right);
	}

}

int main() {

	fill_grid(GRID, ROWS, COLS, WALL_TILE);

	int max_depth = 0;
	int counter = 0;

	std::unique_ptr<Node> root = std::make_unique<Node>();

	root->partition = Rectangle(0 + BORDER_SIZE, 0 + BORDER_SIZE, ROWS - (BORDER_SIZE * 2), COLS - (BORDER_SIZE * 2));
	root->depth = 0;

	std::queue<Node*> node_queue;
	node_queue.push(root.get());

	while (!node_queue.empty()) {
		partition(node_queue, max_depth, counter);
	}

	create_room_block(root);
	create_corridors(root);
	create_doors(GRID, 0 + BORDER_SIZE, 0 + BORDER_SIZE, ROWS - (BORDER_SIZE * 2), COLS - (BORDER_SIZE * 2));

	display_grid(GRID);
	return 0;
}
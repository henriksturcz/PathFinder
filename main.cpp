#include <SFML/Graphics.hpp>
#include <vector>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>

const int CELL_SIZE = 40;
const int UI_WIDTH = 300;
const int GRID_WIDTH = 15;
const int GRID_HEIGHT = 15;
const float OBSTACLE_CHANCE = 0.2f;

std::vector<std::vector<int>> grid(GRID_HEIGHT, std::vector<int>(GRID_WIDTH, 0));
sf::Vector2i startPos(-1, -1);
sf::Vector2i endPos(-1, -1);
std::vector<sf::Vector2i> path;
bool useAStar = true;

void generateGrid() {
    std::srand(std::time(nullptr));
    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            grid[y][x] = (std::rand() / static_cast<float>(RAND_MAX)) < OBSTACLE_CHANCE ? 1 : 0;
        }
    }
}

struct Node {
    sf::Vector2i pos;
    int cost, heuristic;
    Node* parent;
    Node(sf::Vector2i p, int c, int h, Node* par) : pos(p), cost(c), heuristic(h), parent(par) {}
    int totalCost() const { return cost + heuristic; }
};

struct CompareNode {
    bool operator()(const Node* a, const Node* b) { return a->totalCost() > b->totalCost(); }
};

std::vector<sf::Vector2i> reconstructPath(Node* node) {
    std::vector<sf::Vector2i> path;
    while (node) {
        path.push_back(node->pos);
        node = node->parent;
    }
    std::reverse(path.begin(), path.end());
    return path;
}

int heuristic(sf::Vector2i a, sf::Vector2i b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

std::vector<sf::Vector2i> findPath() {
    if (startPos.x == -1 || endPos.x == -1) return {};

    std::priority_queue<Node*, std::vector<Node*>, CompareNode> openSet;
    std::vector<std::vector<bool>> visited(GRID_HEIGHT, std::vector<bool>(GRID_WIDTH, false));
    openSet.push(new Node(startPos, 0, heuristic(startPos, endPos), nullptr));

    while (!openSet.empty()) {
        Node* current = openSet.top();
        openSet.pop();

        if (current->pos == endPos) return reconstructPath(current);
        if (visited[current->pos.y][current->pos.x]) continue;
        visited[current->pos.y][current->pos.x] = true;

        std::vector<sf::Vector2i> neighbors = {
            {current->pos.x + 1, current->pos.y}, {current->pos.x - 1, current->pos.y},
            {current->pos.x, current->pos.y + 1}, {current->pos.x, current->pos.y - 1}
        };

        for (auto& n : neighbors) {
            if (n.x >= 0 && n.x < GRID_WIDTH && n.y >= 0 && n.y < GRID_HEIGHT && !visited[n.y][n.x] && grid[n.y][n.x] == 0) {
                int newCost = current->cost + 1;
                int heuristicValue = useAStar ? heuristic(n, endPos) : 0;
                openSet.push(new Node(n, newCost, heuristicValue, current));
            }
        }
    }
    return {};
}

struct Button {
    sf::RectangleShape shape;
    sf::Text text;
    sf::Font font;
    sf::Color normalColor = sf::Color(180, 180, 180);
    sf::Color clickedColor = sf::Color(150, 150, 150);
    bool isPressed = false;

    Button(float x, float y, float width, float height, const std::string& label) {
        font.loadFromFile("C:/Windows/Fonts/consola.ttf");
        shape.setSize(sf::Vector2f(width, height));
        shape.setPosition(x, y);
        shape.setFillColor(normalColor);
        shape.setOutlineThickness(2);
        shape.setOutlineColor(sf::Color::Black);

        text.setFont(font);
        text.setString(label);
        text.setCharacterSize(18);
        text.setFillColor(sf::Color::Black);
        text.setPosition(x + 10, y + 10);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(shape);
        window.draw(text);
    }

    bool isClicked(sf::Vector2i mousePos) {
        return shape.getGlobalBounds().contains(sf::Vector2f(mousePos));
    }

    void setPressed(bool pressed) {
        isPressed = pressed;
        shape.setFillColor(pressed ? clickedColor : normalColor);
    }
};

int main() {
    int windowWidth = GRID_WIDTH * CELL_SIZE + UI_WIDTH;
    int windowHeight = GRID_HEIGHT * CELL_SIZE;
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Robot Navigation", sf::Style::Titlebar | sf::Style::Close);

    generateGrid();

    Button generateButton(windowWidth - UI_WIDTH + 20, 50, 200, 50, "Generate Grid");
    Button setStartButton(windowWidth - UI_WIDTH + 20, 120, 200, 50, "Set Start");
    Button setEndButton(windowWidth - UI_WIDTH + 20, 190, 200, 50, "Set End");
    Button aStarButton(windowWidth - UI_WIDTH + 20, 260, 200, 50, "Use A*");
    Button dijkstraButton(windowWidth - UI_WIDTH + 20, 330, 200, 50, "Use Dijkstra");
    Button findPathButton(windowWidth - UI_WIDTH + 20, 400, 200, 50, "Find Path");

    bool settingStart = false, settingEnd = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                if (generateButton.isClicked(mousePos)) { generateGrid(); path.clear(); }
                else if (setStartButton.isClicked(mousePos)) { settingStart = true; settingEnd = false; }
                else if (setEndButton.isClicked(mousePos)) { settingEnd = true; settingStart = false; }
                else if (aStarButton.isClicked(mousePos)) { useAStar = true; }
                else if (dijkstraButton.isClicked(mousePos)) { useAStar = false; }
                else if (findPathButton.isClicked(mousePos)) { path = findPath(); }
                else {
                    int x = mousePos.x / CELL_SIZE, y = mousePos.y / CELL_SIZE;
                    if (x < GRID_WIDTH && y < GRID_HEIGHT) {
                        if (settingStart) { startPos = {x, y}; settingStart = false; }
                        else if (settingEnd) { endPos = {x, y}; settingEnd = false; }
                    }
                }
            }
        }

        window.clear(sf::Color::White);
        for (int y = 0; y < GRID_HEIGHT; ++y) {
            for (int x = 0; x < GRID_WIDTH; ++x) {
                sf::RectangleShape cell(sf::Vector2f(CELL_SIZE, CELL_SIZE));
                cell.setPosition(x * CELL_SIZE, y * CELL_SIZE);
                cell.setOutlineColor(sf::Color::Black);
                cell.setOutlineThickness(1);
                cell.setFillColor(grid[y][x] == 1 ? sf::Color::Red : sf::Color::White);
                if (grid[y][x] == 1)
                    cell.setFillColor(sf::Color(80, 80, 80));
                else
                    cell.setFillColor(sf::Color(220, 220, 220,180));

                if (sf::Vector2i(x, y) == startPos)
                    cell.setFillColor(sf::Color(100, 200, 255, 80));
                if (sf::Vector2i(x, y) == endPos)
                    cell.setFillColor(sf::Color(130, 230, 255, 150));
                if (std::find(path.begin(), path.end(), sf::Vector2i(x, y)) != path.end())
                    cell.setFillColor(sf::Color(100, 200, 255, 180));

                window.draw(cell);
            }
        }

        generateButton.draw(window);
        setStartButton.draw(window);
        setEndButton.draw(window);
        aStarButton.draw(window);
        dijkstraButton.draw(window);
        findPathButton.draw(window);
        window.display();
    }

    return 0;
}

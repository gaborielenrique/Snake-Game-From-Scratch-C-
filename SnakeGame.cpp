#include <iostream>    // Printing
#include <map>    // Snake's coordinates
#include <utility>   // Something idk
#include <random>    // Generating the nake's food
#include <algorithm>    // Used to fill the board array
#include <chrono>    // alongside sleep function
#include <thread>    // Used for sleep function
#include <string>    // For string operations
#include "keyboard.hpp" // For keyboard input


// This script is for the snake game from sratch

// TODO: Finish the main game loop

using namespace std;


const int BOARDLENGTH = 31, BOARDWIDTH = 31;



// Takes care of body/bodypart creation, collision detection, and updating body coordinates
class body
{

public:


    // Every part of the snake. Includes coordinates, and connections between parts
    class link
    {

    public:

	int xCoordinates, yCoordinates;    // Coordinates for the link
	link* next;    // Pointer for the next link
	link* prev;    // Pointer for the previous link

	// Initialize all variables
	link(int y, int x, link* next = nullptr, link* prev = nullptr): yCoordinates(y), xCoordinates(x), next(next), prev(prev) {}

    };



    link* head;    // Head node for the snake
    map<link*, pair<int,int>> coordinatesMap;    // Map for every link and its coordinates

    // Initialize all variables
    body() 
    {

	head = new link(int(BOARDLENGTH / 2), int(BOARDWIDTH / 2));
	coordinatesMap[head] = {head->yCoordinates, head->xCoordinates};

    }


    // Grows the body by one link
    void addLink(const pair<int, int> change)
    {

	// Move head to new position
	head->xCoordinates += change.second;
	head->yCoordinates += change.first;

	// Create new link at the previous head position
	link* newLink = new link(head->yCoordinates - change.first, head->xCoordinates - change.second, head->next, 
			  head);

	//if there was a link after the head, update its previous link
	if (head->next != NULL)
	{

	    head->next->prev = newLink;

	}

	// Insert new link after head
	head->next = newLink;
	coordinatesMap[newLink] = {newLink->xCoordinates, newLink->yCoordinates};

	return;

    }


    // Checks for Collision
    bool collisionDetection()
    {

	// Check for collision with walls
	if (!(head->xCoordinates >= 0 && head->xCoordinates < BOARDWIDTH) || 
	    !(head->yCoordinates >= 0 && head->yCoordinates < BOARDLENGTH))
	{

	    cout << "You hit a wall!" << endl;
	    return true;

	}

	// Check for collision with self
	for (const auto& [link, coordinates] : coordinatesMap)
	{

	    if ((link != head) && (coordinates.first == head->xCoordinates) && 
		(coordinates.second == head->yCoordinates))
	    {

		cout << "You hit yourself" << endl;
		return true;

	    }

	}

	return false;

    }


    // Update the coordinates for each link in the snake
    void updateCoordinatesMap()
    {

	// Clear the coordinates map
	coordinatesMap.clear();

	// Assign a variable for traversing the body
	link* body = head;
	
	// Refill the map with the new coordinates
	while (body)
	{

	    coordinatesMap[body] = {body->xCoordinates, body->yCoordinates};
	    body = body->next;

	}

	return;

    }


    // Class destructor for when the game ends
    ~body()
    {

	link* temp = head;    // For traversing the body
	
	while (temp)
	{

	    link* deleteLink = temp;
	    temp = temp->next;
	    delete deleteLink;

	}

    }

};



// Takes care of food spawning
class food
{

public:

    int xCoordinates, yCoordinates;

    food(int x, int y) : xCoordinates(x), yCoordinates(y) {}


    void spawn(const body& body)
    {

	// Make the random number generator
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> x(0, BOARDLENGTH - 1);    // Generator on the x axis
	uniform_int_distribution<> y(0, BOARDWIDTH - 1);    // Generator on the y axis


	while (true)
	{

	    // Generate random location of the food
	    int xLocation = x(gen);
	    int yLocation = y(gen);

	    //for checking if the food is on the body
	    bool isOnBody = false;

	    // Check if the food is on the body of the snake
	    for (const auto& [link, coordinates] : body.coordinatesMap)
	    {

		if ((xLocation == coordinates.first) && (yLocation == coordinates.second))
		{


		   isOnBody = true;
		   break;

		}

	    }

	    // If the food is not on the body, keep the coordinates
	    if (isOnBody == false)
	    {

		xCoordinates = xLocation;
		yCoordinates = yLocation;

		break;

	    }

	}

	return;

    }



    ~food(){}

};



// takes care of moving and displaying the snake/game as a whole
class movements
{

private:

    body& snake;
    food& apple;

public:

    char board[BOARDLENGTH][BOARDWIDTH];
    movements(body& b, food& f) : snake(b), apple(f)
    {

	fill(&board[0][0], &board[0][0] + BOARDLENGTH * BOARDWIDTH, '.');

    }


    // For displaying the board in a more readable way
    string join(const string& separator)
    {

	string formattedBoard;

	for (int i = 0; i < BOARDLENGTH; i++)
	{

	    for (int j = 0; j < BOARDWIDTH; j++)
	    {

		formattedBoard += board[i][j];

		if (j < BOARDWIDTH - 1)
		{

		    formattedBoard += separator;

		}

	    }

	    formattedBoard += "\n";

	}

	return formattedBoard;

    }


    // Moves the boo - ttie
    void moving(const char& keyHit)
    {

	// Set up temporary variable to hold pevious coordinates
	body::link* bod = snake.head;
	int tempx = bod->xCoordinates, tempy = bod->yCoordinates;

	//set up for array comparison
	const char upDown[] = {'w', 's'};
	const char leftRight[] = {'a', 'd'};

	// Define direction based on keyHit
	if (std::find(std::begin(upDown), std::end(upDown), keyHit) != std::end(upDown))
	{

	    if (keyHit == 'w')
	    {

		// Move up
		bod->yCoordinates -= 1;

	    }else{

		// Move down
		bod->yCoordinates += 1;

	    }

	}else if (std::find(std::begin(leftRight), std::end(leftRight), keyHit) != std::end(leftRight)){

	    if (keyHit == 'a')
	    {

		// Move left
		bod->xCoordinates -= 1;

	    }else{

		// Move right
		bod->xCoordinates += 1;

	    }

	}

	// Shift the rest of the body
	while (bod->next)
	{

	    bod = bod->next;
	    int temp2x = bod->xCoordinates, temp2y = bod->yCoordinates;
	    bod->xCoordinates = tempx;
	    bod->yCoordinates = tempy;
	    tempx = temp2x;
	    tempy = temp2y;

	}

	// Reister the changes in the coordinates map
	snake.updateCoordinatesMap();

	return;

    }


    // "Render" the playing board
    void display(const int& SCORE)
    {

	// Move cursor to upper left to "refresh" the screen
	cout << "\033[H";

	// Reset the board for later modifications
	fill(&board[0][0], &board[0][0] + BOARDLENGTH * BOARDWIDTH, '.');

	body::link* link = snake.head;

	// Display the head as 'S' if within the range of the board
	if ((link->xCoordinates >= 0 && link->xCoordinates < BOARDWIDTH) && 
	    (link->yCoordinates >= 0 && link->yCoordinates < BOARDLENGTH))
	{

	    board[link->yCoordinates][link->xCoordinates] = 'S';

	}

	// Display the body as a line of 'O'
	while (link->next)
	{

	    link = link->next;
	    board[link->yCoordinates][link->xCoordinates] = 'O';

	}

	// Display the food as 'Q'
	board[apple.yCoordinates][apple.xCoordinates] = 'Q';

	// Display with empty spaces
	cout << join("") << endl;
	cout << "Score: " << SCORE << endl;

	// Sleep so the snake doesn't go brrrrr
	std::this_thread::sleep_for(std::chrono::milliseconds(83));

	return;

    }


};


//main game loop
int main()
{

    int score = 0;

    // Initialize body, food, and movements
    body snake;
    food apple(5, 5);
    movements move(snake, apple);

    // Map the directions
    const map<char, pair<int, int>> DIRECTIONS = {
	{'w', {-1, 0}},
	{'s', {1, 0}},
	{'a', {0, -1}},
	{'d', {0, 1}}
    };

    // Initial direction/key press
    char keyHit = 's';

    // Make space for the first link
    move.moving(keyHit);

    // Start with one head and one link
    snake.addLink(DIRECTIONS.at(keyHit));
    apple.spawn(snake);
    move.display(score);

    // List of possible inputs
    char inputs[] = {'w', 'a', 's', 'd'}, upDown[] = {'w', 's'}, leftRight[] = {'a', 'd'};

    #ifndef _WIN32
	
    // Enalble raw mode
    keyboard::setBufferedInput(false);

    #endif

    while (true)
    {

	if (keyboard::kbhit())
	{

	    char newKey = keyboard::getch();
	    
	    // If the key press is one of the move keys, and movement is neither into or against movement, accept key
	    if (((std::find(std::begin(upDown), std::end(upDown), newKey) != std::end(upDown)) &&
	    (std::find(std::begin(upDown), std::end(upDown), keyHit) == std::end(upDown))) ||
	    ((std::find(std::begin(leftRight), std::end(leftRight), newKey) != std::end(leftRight)) && 
	    (std::find(std::begin(leftRight), std::end(leftRight), keyHit) == std::end(leftRight))))
	    {

		keyHit = newKey;

	    }

	    // Quit the game if q is pressed
	    if (newKey == 'q')
	    {

		cout << "Game Exited!" << endl;
		break;

	    }
	}

	// Check for collisions before moving
	if (snake.collisionDetection())
	{

	    cout << "Game Over!" << endl;
	    break;

	}


	// Check if the food was reached, add a link if so
	if ((snake.head->xCoordinates == apple.xCoordinates) && (snake.head->yCoordinates == apple.yCoordinates))
	{

	    snake.addLink(DIRECTIONS.at(keyHit));
	    apple.spawn(snake);
	    score++;

	}

	move.moving(keyHit);

	move.display(score);

    }

    #ifndef _WIN32

    // Disable raw mode
    keyboard::setBufferedInput(true);

    #endif

    return 0;

}

#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <random>


//making a dodging game

enum GameState {MENU , START , SETTING , DIFFICULTY, EASY, NORMAL, HARD, PRO, GAME_OVER, PAUSE, STORE, EXIT};
GameState CurrentState = MENU;
GameState PreviousState;

//=======================Declaring variables=======================//
const int SCREEN_WIDTH = 1020;          //Screen Width
const int SCREEN_HEIGHT = 850;          //Screen Height
const int MAX_ENEMY = 100;              //Max Number of enemies

Color BackGround = { 233, 233, 233, 1 };
Color Border = BLACK;
Color Text_Color = BLACK;

//UI elements
int Score = 0;
int Coins = 0;
bool Reset_or_Not = false;
bool toggle = false;


//player variables [For Jumping]
float Gravity = 0.5f;
float JumpVelocity = 0;
int Ground = 650;
bool Is_Player_Jumping = false;


//Enemy variables
static int Random_Speed_Enemy;
static int Random_Position_Enemy;
static int Random_Radius_Enemy;


//Store 
int Price1 = 50;           //speed
int Price2 = 70;           //jump
int Price3 = 100;          //Shrink
int Price4 = 250;          //health
int Prices[4] = { Price1, Price2, Price3, Price4 };


//Alert Box
float Alert_Timer = 0;
bool Alert_Show = false;
float Alpha = 0;
float DirX = 400;


//shrink function
float Player_Size = 50.0f;
float Normal_Player_Size = 50.0f;
float Shrink_Player_Size = 25.0f;

bool Shrink_Active = false;
float Shrink_Timer = 0.0f;
float Shrink_Duration = 10.0f; 


//=======================Declaring Vectors and Sounds==================//
Vector2 Mouse_Position;                 //Mouse Position

Sound Jump;
Sound Land;
Sound Button_Click;
Sound No_Money;

Music BG_Music;
Music Pause_Menu_Music;
Music Easy_Normal_Music;
Music Hard_Music;
Music* CurrentMusic = nullptr;


//making structures
struct Player {
	int Player_Health = 1;                                //Player Health
	Vector2 Player_Speed = { 400 , 400 };                 //Player Speed
	Vector2 Player_Position = { 500 , 650 };              //Player Position
	float Player_JumpForce = -12;                         //Player jump-force [how high it can jump]
	Color Player_Color = BLACK;                           //Default color
};
Player Player_1;


struct Enemy {
	Vector2 Enemy_Speed;                                  //Enemy speed
	Vector2 Enemy_Position;                               //Enemy Position
	int Enemy_Radius;                                     //Size of enemy
	bool Is_Enemy_Alive = false;                          //Check , is enemy alive
	Color Enemy_Color = BLACK;                            //Default color
};
Enemy Enemies[MAX_ENEMY];


//making buttons and Asthetics
void Make_Button(Rectangle Button, Color Normal, Color Hover, const char* Button_Name, int SizeX, int SizeY, int Size, GameState NewState);
void Make_Button_Reset(Rectangle Button, Color Normal, Color Hover, const char* Button_Name, int SizeX, int SizeY, int Size, GameState NewState);
void Exit_Key();                //Exit Key
void UI_ELEMENTS();             //UI elements [Like Coins and score]
void Game_Over_Function();      //Game over screen
void Pause_Function();          //Pause Screen
void Start_Game();              //contains necessary Game elements to run
void Reset_Resources();
void Asthetic_Function(bool Show) {
	DrawLineEx({ 0,20 }, { 1020 , 20 }, 50, Border);           //Upper
	DrawLineEx({ 20,850 }, { 20 , 0 }, 50, Border);            //Left
	DrawLineEx({ 1000 , 0 }, { 1000 , 850 }, 50, Border);      //Right
	DrawLineEx({ 0 , 830 }, { 1000 , 830 }, 50, Border);       //Lower
	if (Show) {
		DrawText("A & D for Left and Right Movement \nSpace for Jumping \nEsc for Escape", 600, 730, 20, Text_Color);
	}
}


//Difficulty scale and buttons Functions
void Difficulty_Button(Rectangle Button, Color Normal, Color Hover, const char* Difficulty_Name, int SizeX, int SizeY, int Size, GameState NewState);
void Difficulty_Choose();       //Choose Difficulty
void Difficulty_Elements();     //Enemy speed , movement and more


//Player Functions
void Player_Movement();        //Player Movement
void Player_Jumping();         //Player Jumping


//Enemy functions
static void Spawn_Enemy(float Spawn_Time, int Speed1, int Speed2);    //Spawning the enemy
void Update_Enemy();           //Updating their position
void CheckCollision();         //Checking collision between enemy and player


//Store elemets Functions
void Store_Button(Rectangle Button, Color Normal, Color Hover, const char* Option, int SizeX, int Index);
void Store_Function();                            //Store menu
void Update_Shrink_Buff();


//setting related function
void Make_Button_Setting(Rectangle Button, Color Normal, Color Hover, const char* Button_Name, int SizeX, int SizeY, int Size);
void Setting_Function(Rectangle Button , bool Show);       //Shows setting 
void Music_Funtion();
void Alert_Box(const char* Alert_Msg, bool& Show_Alert, float Timer);


//main function
int main() {
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Dodge Master");
	InitAudioDevice();
	SetExitKey(KEY_NULL);
	SetTargetFPS(60); 
	
	//Sounds
	Jump = LoadSound("Assets/Sound/Jumping.mp3");
	Land = LoadSound("Assets/Sound/Landing_Effect.mp3");
	Button_Click = LoadSound("Assets/Sound/Button_Press.mp3");
	No_Money = LoadSound("Assets/Sound/Store_No_Money.mp3");

	//Music
	BG_Music = LoadMusicStream("Assets/Sound/BackGround_Music_MENU.mp3");
	Pause_Menu_Music = LoadMusicStream("Assets/Sound/Pause_Game_Music.mp3");
	Easy_Normal_Music = LoadMusicStream("Assets/Sound/EasyANDNormal_Music.mp3");
	Hard_Music = LoadMusicStream("Assets/Sound/Hard_Lvl_Music.mp3");

	SetSoundVolume(No_Money, 10);

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(BackGround);
		Mouse_Position = GetMousePosition();
		Music_Funtion();

		if (CurrentState == MENU) {
			DrawText("Dodge Master", 180, 100, 100, RED);         //Title

			Make_Button({ 400 , 400 , 250 , 70 }, GREEN, RED, "Start", 45, 15, 50, DIFFICULTY);
			Make_Button({ 400 , 500 , 250 , 70 }, BLUE, PINK, "Setting", 45, 15, 50, SETTING);
			Make_Button({ 400 , 600 , 250 , 70 }, BROWN, YELLOW, "Exit", 45, 15, 50, EXIT);
			Asthetic_Function(true);
		}


		else if (CurrentState == DIFFICULTY) {
			Difficulty_Choose();
		}


		else if (CurrentState == SETTING) {
			Setting_Function({ 50 , 100 , 250 , 50 } , true);
		}


		else if (CurrentState == EASY) {
			Start_Game();
		}


		else if (CurrentState == NORMAL) {
			Start_Game();
		}


		else if (CurrentState == HARD) {
			Start_Game();
		}


		else if (CurrentState == PRO) {
			Start_Game();
		}


		else if (CurrentState == PAUSE) {
			Pause_Function();
		}


		else if (CurrentState == STORE) {
			Store_Function();
		}


		else if (CurrentState == GAME_OVER) {
			Game_Over_Function();
		}


		else if (CurrentState == EXIT) {
			break;
		}

		EndDrawing();
	}

	UnloadSound(Jump);
	UnloadSound(Land);
	UnloadSound(Button_Click);
	UnloadSound(No_Money);

	UnloadMusicStream(BG_Music);
	UnloadMusicStream(Pause_Menu_Music);
	UnloadMusicStream(Easy_Normal_Music);
	UnloadMusicStream(Hard_Music);

	CloseWindow();
	return 0;
}


//making button for UI
void Make_Button(Rectangle Button, Color Normal, Color Hover, const char* Button_Name, int SizeX, int SizeY, int Size, GameState NewState) {
	bool Clicked = CheckCollisionPointRec(Mouse_Position, Button);

	DrawRectangleRec(Button, Clicked ? Hover : Normal);
	DrawText(Button_Name, Button.x + SizeX, Button.y + SizeY, Size, Text_Color);

	if (Clicked && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
		CurrentState = NewState;
		PlaySound(Button_Click);
	}
}


void Make_Button_Reset(Rectangle Button, Color Normal, Color Hover, const char* Button_Name, int SizeX, int SizeY, int Size, GameState NewState) {
	bool Clicked = CheckCollisionPointRec(Mouse_Position, Button);

	DrawRectangleRec(Button, Clicked ? Hover : Normal);
	DrawText(Button_Name, Button.x + SizeX, Button.y + SizeY, Size, Text_Color);

	if (Clicked && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
		PlaySound(Button_Click);
		CurrentState = NewState;
		Reset_Resources();
	}
}


//Exit key function
void Exit_Key() {
	bool Button_Hit = false;
	if (IsKeyPressed(KEY_ESCAPE) && (CurrentState == EASY || CurrentState == NORMAL || CurrentState == HARD || CurrentState == PRO) && !Button_Hit) {
		CurrentState = PAUSE;
		Button_Hit = true;
	}
	if (IsKeyPressed(KEY_ESCAPE) && (CurrentState == MENU || CurrentState == SETTING || CurrentState == DIFFICULTY || CurrentState == GAME_OVER)) {
		CurrentState = MENU;
		Reset_Resources();
	}
}


//UI Elemets
void UI_ELEMENTS() {
	DrawText(TextFormat("Score : %d", Score), 50, 50, 30, Text_Color);
	DrawText("Coins :",50, 100, 30, Text_Color);
	DrawText(TextFormat("%d", Coins), 180, 100, 30, GOLD);
	DrawText(TextFormat("Health : %d", Player_1.Player_Health), 50, 150, 30, RED);

	DrawLineEx({ 0, 800 }, { 1020 , 800 }, 200, BROWN);
	Setting_Function({ 870 , 90 , 250 , 50 } , false);
	Make_Button({ 870 , 30 , 150 , 50 }, GOLD, LIGHTGRAY, "Store", 25, 15, 30, STORE);
}


//Game over function
void Game_Over_Function() {
	Asthetic_Function(false);
	DrawText("Game Over!!", 300, 70, 90, RED);
	DrawText(TextFormat("Score : %d", Score), 400, 200, 50 , Text_Color);
	Make_Button_Reset({ 400 , 400 , 320 , 70 }, PINK, GRAY, "Play again", 45, 15, 50, DIFFICULTY);
	Make_Button_Reset({ 400 , 500 , 320 , 70 }, GOLD, GRAY, "Back", 45, 15, 50, MENU);
}


//Pause menu
void Pause_Function() {
    DrawText(TextFormat("Score : %d", Score), 400, 200, 50, Text_Color);
	Make_Button({ 400 , 500 , 250 , 90 }, GREEN, GRAY, "Continue", 45, 15, 50, PreviousState);
	Make_Button_Reset({ 400 , 600 , 250 , 90 }, GOLD, GRAY, "Back", 45, 15, 50, MENU);
	Asthetic_Function(false);
}


//contains all necessary game elements
void Start_Game() {
	Exit_Key();
	UI_ELEMENTS();

	Player_Movement();
	Update_Shrink_Buff();

	Difficulty_Elements();
	Update_Enemy();
	CheckCollision();
}


//reset resources
void Reset_Resources() {
	Player_1.Player_Health = 1;
	Player_1.Player_JumpForce = -12;
	Player_1.Player_Speed = { 400 , 400 };
	Player_1.Player_Position = { 500 , 650 };

	Price1 = 50;
	Price2 = 70;
	Price3 = 100;
	Price4 = 250;

	Score = 0;
	Coins = 0;

	for (int i = 0; i < MAX_ENEMY; i++) {
		Enemies[i].Is_Enemy_Alive = false;
	}
}


//Make Buttons for difficulties
void Difficulty_Button(Rectangle Button, Color Normal, Color Hover, const char* Difficulty_Name, int SizeX, int SizeY, int Size, GameState NewState) {
	bool Clicked = CheckCollisionPointRec(Mouse_Position , Button);

	DrawRectangleRec(Button , Clicked ? Hover : Normal);
	DrawText(Difficulty_Name, Button.x + SizeX, Button.y + SizeY, Size, Text_Color);

	if (Clicked && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
		PlaySound(Button_Click);
		CurrentState = NewState;
		PreviousState = NewState;
	}
}


//Choose Difficulty
void Difficulty_Choose() {
	Exit_Key();
	Difficulty_Button({ 350 , 200 , 250 , 70 }, GREEN , GRAY , "EASY" , 35 , 15 , 50 , EASY);
	Difficulty_Button({ 350 , 300 , 250 , 70 }, GOLD, GRAY, "NORMAL", 35, 15, 50, NORMAL);
	Difficulty_Button({ 350 , 400 , 250 , 70 }, PINK, GRAY, "HARD", 35, 15, 50, HARD);
	Difficulty_Button({ 350 , 500 , 250 , 70 }, RED, GRAY, "PRO", 35, 15, 50, PRO);
	Asthetic_Function(false);
}


//Enemy speed , movement and more
void Difficulty_Elements() {
	if (CurrentState == EASY) {
		Spawn_Enemy(0.5, 3, 5);
	}

	if (CurrentState == NORMAL) {
		Spawn_Enemy(0.4 , 3 , 7);
	}

	if (CurrentState == HARD) {
		Spawn_Enemy(0.3 , 4 , 8);
	}

	if (CurrentState == PRO) {
		Spawn_Enemy(0.2 , 6 , 10);
	}
}


//Player Movement
void Player_Movement() {
	if (IsKeyDown(KEY_A)) Player_1.Player_Position.x -= Player_1.Player_Speed.x * GetFrameTime();
	if (IsKeyDown(KEY_D)) Player_1.Player_Position.x += Player_1.Player_Speed.x * GetFrameTime();

	if(IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) Player_1.Player_Position.y += Player_1.Player_Speed.y * GetFrameTime();

	Player_Jumping();
	DrawRectangle(
		Player_1.Player_Position.x,
		Player_1.Player_Position.y,
		Player_Size,
		Player_Size,
		Player_1.Player_Color
	);


	if (Player_1.Player_Position.x <= -10) Player_1.Player_Position.x = 1020;
	else if (Player_1.Player_Position.x >= 1020) Player_1.Player_Position.x = -10;
}


//Player Jumping
void Player_Jumping() {

	if (IsKeyPressed(KEY_SPACE) && !Is_Player_Jumping) {
		PlaySound(Jump);
		JumpVelocity = Player_1.Player_JumpForce;
		Is_Player_Jumping = true;
	}

	JumpVelocity += Gravity;
	Player_1.Player_Position.y += JumpVelocity;

	if (Player_1.Player_Position.y > 650) {
		Player_1.Player_Position.y = 650;
		JumpVelocity = 0;
		Is_Player_Jumping = false;
	}

	if (Player_1.Player_Position.y == 650 && Is_Player_Jumping) {
		PlaySound(Land);
	}
}


//spawing enemies
static void Spawn_Enemy(float Spawn_Time , int Speed1 , int Speed2) {

	static float Timer;
	Timer += GetFrameTime();
	if (Timer <= Spawn_Time) return;
	Timer = 0;

	for (int i = 0; i < MAX_ENEMY; i++) {
		if (Enemies[i].Is_Enemy_Alive) continue;

		Random_Speed_Enemy = GetRandomValue(Speed1, Speed2);
		Random_Position_Enemy = GetRandomValue(50, 980);
		Random_Radius_Enemy = GetRandomValue(30, 50);

		Enemies[i].Enemy_Speed = { 0.0f , (float)Random_Speed_Enemy};
		Enemies[i].Enemy_Position = { (float)Random_Position_Enemy , 0.0f };
		Enemies[i].Enemy_Radius = Random_Radius_Enemy;

		Enemies[i].Is_Enemy_Alive = true;
		break;
	}
}


//update enemy
void Update_Enemy() {

	for (int i = 0; i < MAX_ENEMY; i++) {
		if (!Enemies[i].Is_Enemy_Alive) continue;

		Enemies[i].Enemy_Position.y += Enemies[i].Enemy_Speed.y;
		DrawCircleV(Enemies[i].Enemy_Position, Enemies[i].Enemy_Radius, Enemies[i].Enemy_Color);
	}
}


//check for player collision
void CheckCollision() {
	for (int i = 0; i < MAX_ENEMY; i++) {
		if (CheckCollisionCircleRec(Enemies[i].Enemy_Position, (float)Enemies[i].Enemy_Radius, { Player_1.Player_Position.x , Player_1.Player_Position.y , Player_Size , Player_Size }
		) && Enemies[i].Is_Enemy_Alive) {
			Enemies[i].Is_Enemy_Alive = false; 
			Player_1.Player_Health -= 1;
		}

		if (Player_1.Player_Health <= 0) { CurrentState = GAME_OVER; }

		if (Enemies[i].Enemy_Position.y >= 700 && Enemies[i].Is_Enemy_Alive) {
			Score += 1;
			Coins += 5;
			Enemies[i].Is_Enemy_Alive = false;
		}
	}
}


//Making buttons for store
void Store_Button(Rectangle Button, Color Normal, Color Hover,
	const char* Buff, int SizeX, int Index)
{
	bool Hovered = CheckCollisionPointRec(Mouse_Position, Button);

	DrawRectangleRec(Button, Hovered ? Hover : Normal);
	DrawText(Buff, Button.x + 45, Button.y + 15, 40, BLACK);

	DrawText(TextFormat("%d", Prices[Index - 1]),
		Button.x + SizeX, Button.y + 15, 40, BLACK);

	if (Hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
		if (Coins < Prices[Index - 1])
		{
			PlaySound(No_Money);
			Alert_Show = true;
			Alpha = 1.0f;
			DirX = 400;
			return;
		}

		PlaySound(Button_Click);
		Coins -= Prices[Index - 1];

		switch (Index)
		{
		case 1:
			Player_1.Player_Speed.x += 75;
			Prices[0] += 25;
			break;

		case 2:
			Player_1.Player_JumpForce -= 3;
			Prices[1] += 25;
			break;

		case 3:
			if (!Shrink_Active)   // prevent stacking
			{
				Player_Size = Shrink_Player_Size;
				Shrink_Active = true;
				Shrink_Timer = 0.0f;
			}
			Prices[2] += 30;
			break;

		case 4:
			Player_1.Player_Health += 2;
			Prices[3] += 100;
			break;
		}
	}
	
	Prices[0] = Clamp(Prices[0], 0, 500);
	Prices[1] = Clamp(Prices[1], 0, 500);
	Prices[2] = Clamp(Prices[2], 0, 500);
	Prices[3] = Clamp(Prices[3], 0, 1000);
}


//Store
void Store_Function() {
	
	DrawText("Coins :", 820, 50, 30, BLACK);
	DrawText(TextFormat("%d", Coins), 930, 50, 30, GOLD);

	Store_Button({ 100 , 100 , 700 , 70 }, LIGHTGRAY, GRAY, "Increase Speed      : ", 500 , 1);
	Store_Button({ 100 , 220 , 700 , 70 }, LIGHTGRAY, GRAY, "Increase Jump        :", 500 , 2);
	Store_Button({ 100 , 340 , 700 , 70 }, LIGHTGRAY, GRAY, "Shrink [10s]           : ", 500 , 3);
	Store_Button({ 100 , 460 , 700 , 70 }, LIGHTGRAY, GRAY, "Gain One more Chance : ", 550 , 4);
	
	Make_Button({ 450 , 700 , 250 , 70 }, GOLD, LIGHTGRAY, "Return", 25, 15, 50, PreviousState);
	Asthetic_Function(false);
	if (Alert_Show)
	{
		Alert_Box("Not Enough Money", Alert_Show, 3.0f);
	}
}


//setting buttons
void Make_Button_Setting(Rectangle Button, Color Normal, Color Hover, const char* Button_Name, int SizeX, int SizeY, int Size) {
	bool Clicked = CheckCollisionPointRec(Mouse_Position, Button);

	DrawRectangleRec(Button, Clicked ? Hover : Normal);
	DrawText(Button_Name, Button.x + SizeX, Button.y + SizeY, Size, Text_Color);

	if (Clicked && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
		toggle = !toggle;
		PlaySound(Button_Click);
		if (toggle) {
			Player_1.Player_Color = WHITE;
			BackGround = { 19, 3, 3, 1};
			Border = BLACK;
			Text_Color = WHITE;

			for (int i = 0; i < MAX_ENEMY; i++) {
				Enemies[i].Enemy_Color = WHITE;
			}
		}

		else {
			Player_1.Player_Color = BLACK;
			BackGround = { 233, 233, 233, 1 };
			Border = BLACK;
			Text_Color = BLACK;

			for (int i = 0; i < MAX_ENEMY; i++) {
				Enemies[i].Enemy_Color = BLACK;
			}
		}
	}
}


void Setting_Function(Rectangle Button , bool Show) {
	Exit_Key();
	if (Show) {
		DrawText("SETTINGS", 400, 40, 70, Text_Color);
		Asthetic_Function(true);
	}
	Make_Button_Setting( Button, BLUE, GREEN, "Theme", 35, 15, 30);
}


void Music_Funtion() {
	Music* NewMusic = nullptr;
	if (CurrentState == MENU || CurrentState == START || CurrentState == DIFFICULTY || CurrentState == SETTING) {
		NewMusic = &BG_Music;
	}


	else if (CurrentState == GAME_OVER || CurrentState == PAUSE || CurrentState == STORE) {
		NewMusic = &Pause_Menu_Music;
	}
	

	else if (CurrentState == EASY || CurrentState == NORMAL) {
		NewMusic = &Easy_Normal_Music;
	}
	

	else if (CurrentState == HARD || CurrentState == PRO) {
		NewMusic = &Hard_Music;
	}

	if (CurrentMusic != NewMusic) {
		if (CurrentMusic != nullptr) PauseMusicStream(*CurrentMusic);
		if (NewMusic != nullptr) {
			if (!IsMusicStreamPlaying(*NewMusic)) PlayMusicStream(*NewMusic);
			else ResumeMusicStream(*NewMusic);
		}
		CurrentMusic = NewMusic;
	}

	if (CurrentMusic != nullptr) UpdateMusicStream(*CurrentMusic);
	
}


void Alert_Box(const char* Alert_Msg, bool& Show_Alert, float Timer) {
	if (!Show_Alert) return;

	Alert_Timer += GetFrameTime();
	float FadeDir = Timer * 0.7;
	float Duration = Timer - FadeDir;

	if (Alert_Timer >= FadeDir) {
		Alpha -= GetFrameTime() / Duration;
		DirX -= 300 * GetFrameTime();
	}

	if (Alpha <= 0) {
		Alpha = 0;
	}

	DrawRectangle(DirX, 600, 400, 60, Fade(RED, Alpha));
	DrawRectangleLines(DirX, 600, 400, 60, Fade(BLACK, Alpha));

	if (DirX <= 0) {
		DirX = 0;
	}

	DrawText(Alert_Msg, DirX+10 , 610, 30, Fade(YELLOW, Alpha));

	if (Alert_Timer >= Timer)
	{
		Alert_Timer = 0.0f;
		Alert_Show = false;
	}
}


void Update_Shrink_Buff(){
	if (!Shrink_Active) return;

	Shrink_Timer += GetFrameTime();

	if (Shrink_Timer >= Shrink_Duration)
	{
		// Reset to normal
		Player_Size = Normal_Player_Size;
		Shrink_Timer = 0.0f;
		Shrink_Active = false;
	}
}

//Login/Sign up system
//shrink the player for a certain amount of time
//add maximum limit for buffs
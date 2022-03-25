#include <simple2d.h> 
#include <wiringPi.h> 
#include <time.h> 

/* 
Game    Room                Device                  WiringPi    Pi Pin  I/O 
Bunker	Exterieur	        Gun Switch	            5	        18	    In 
					        Gun Door	            6	        22	    Out 
                            Gun Bypass	            6	        22	    Out                     
                            Gun Light 1	            14	        23	    Out                     
                            Gun Light 2	            15	        8	    Out                     
                            Gun Light 3	            16	        10	    Out                     
                     
        Bunker              Simon rouge	            0	        11	    In  
                            Simon bleu	            1	        12	    In  
                            Simon jaune	            2	        13	    In  
                            Simon vert	            3	        15	    In  
                            Simon blanc	            4	        16	    In  
                            Laser Key	            7	        7	    In  
                            Laser Key Bypass	    10	        24	    Out 
                            Laser power	            10	        24	    Out 
                            TV lift up	            11	        26	    Out 
                            TV lift up Bypass	    11	        26	    Out 
                            TV lift down	        12	        19	    Out 
                            Hand Sensor Bypass      13	        21	    Out 
*/ 


#define MAX_STAGES              11
#define MIN_STAGE               1 
#define MAX_COLOR               5 
#define CLEAR_SCREEN_DELAY      100 
#define DISPLAY_TIME            40 
#define RED                     0 
#define BLUE                    1 
#define YELLOW                  2 
#define GREEN                   3 
#define WHITE                   4 
#define MAX_ELAPSED_TIME        700 


#define SIMON_RED_BUTTON        0 
#define SIMON_BLUE_BUTTON       1 
#define SIMON_YELLOW_BUTTON     2 
#define SIMON_GREEN_BUTTON      3 
#define SIMON_WHITE_BUTTON      4 


#define GUN_SWITCH              5 
#define GUN_DOOR                6 
#define GUN_LIGHT_1             14 
#define GUN_LIGHT_2             15 
#define GUN_LIGHT_3             16 


#define BYPASS_GUN              6 
#define LASER_KEY               7 
#define LASER_POWER             10 
#define BYPASS_LASER_KEY        10 
#define TV_LIFT_UP              11 
#define BYPASS_TV_LIFT_UP       11 
#define TV_LIFT_DOWN            12 
#define BYPASS_TV_LIFT_DOWN     12 
#define BYPASS_HAND_SENSOR      13 
#define TV_LIFT_UP_DELAY        60 
#define TV_LIFT_DOWN_DELAY      60 
#define DOOR_GACHE_DELAY        10 


#define MAX_CODE_NUMBER_DIGIT   7 


#define DOWN                    0 
#define UP                      1 


/* 
 * Variable definition 
 */ 
  
char gunBypass[30] = {"BYPASS_GUN"}; 
time_t gunBypassTimer = 0; 
char laserKeyBypass[30] = {"BYPASS_LASER_KEY"}; 
char tvLiftUpBypass[30] = {"BYPASS_TV_LIFT_UP"}; 
char tvLiftDownBypass[30] = {"BYPASS_TV_LIFT_DOWN"}; 
char handSensorBypass[30] = {"BYPASS_HAND_SENSOR"}; 
char stopBunker[20] = {"STOP_BUNKER"}; 


time_t gunDoorTimer = 0; 
time_t tvLiftUpTimer = 0; 
time_t tvLiftDownTimer = 0; 
time_t noTimer = -1; 


char RefPhoneNumber[7] = {'B', '9', '5', '6', 'J', 'F', '6'}; 
char phoneNumber[7] = {' ', ' ', ' ', ' ', ' ', ' ', ' '}; 
int didgitIndex = 0; 
int validPhoneNumber = 0; 
int laserActivated = 0; 
int lightsState = 0;
int colorState[5] = {0, 0, 0, 0, 0};


bool missionCompleted = false; 
int i = 0; 
int k = 0; 
int j = -640; 
int angle = 0; 
int voiceOffset = 300; 
int barLength = 0; 
int offset = 1; 
int MaxStage = MIN_STAGE; 
int seed=0; 
int nbTime = 0; 
int	clearScreenDelay = 100; 
int lastColorPressed = -1; 
int pressed = 0; 
int currentStage = 0; 
int sequenceElapsedTime = 0; 
char *message; 
char *letter; 
int c=0; 
int keyState = UP; 
int gunLights = 3; 


S2D_Image *img; 
S2D_Image *scan; 
S2D_Image *Missile; 
S2D_Image *Launched; 
S2D_Image *Lance; 
S2D_Image *Deactivated; 
S2D_Image *Desactive; 
S2D_Image *voice; 
S2D_Text *txtTop; 
S2D_Text *txtTop2; 
S2D_Text *txtBot; 
S2D_Text *txtBot2; 
S2D_Sound *endSnd; 
S2D_Sound *succesSnd; 
S2D_Sound *failSnd; 
S2D_Text *txt; 
S2D_Music *mus; 
/* 
 * Structures 
 */ 
  
static const int color[5][4] = { 
 /* R    G    B    Button */ 
    1,   0,   0,   0,      // RED 
    0,   1,   0,   3,      // GREEN 
    0,   0,   1,   1,      // BLUE 
    1,   1,   0,   2,      // YELLOW 
    1,   1,   1,   4,      // WHITE 
}; 


static int randColor[MAX_STAGES]; 


struct BAR 
{ 
    int x; 
    int y; 
    int width; 
    int height; 
    int actual; 
    int increment; 
    float seed; 
    int orientation; // 1 vertical, 0 horizontal 
     
}; 


struct BAR h0 =  {90, 32, 5, 10, 0, 1, 20.0,  0}; 
struct BAR h1 =  {90, 39, 5, 50, 0, 1, 100.0, 0}; 
struct BAR h2 =  {90, 46, 5, 5, 0, 1, 10.0,  0}; 
struct BAR h3 =  {90, 53, 5, 40, 0, 1, 80.0,  0}; 
struct BAR h4 =  {90, 60, 5, 35, 0, 1, 76.0,  0}; 
struct BAR h5 =  {90, 67, 5, 15, 0, 1, 33.0,  0}; 


struct BAR v0 =  {470, 460, 5, 5, 0, 1, 10.0,  1}; 
struct BAR v1 =  {477, 460, 5, 20, 0, 1, 40.0,  1}; 
struct BAR v2 =  {484, 460, 5, 50, 0, 1, 100.0, 1}; 
struct BAR v3 =  {491, 460, 5, 25, 0, 1, 50.0,  1}; 
struct BAR v4 =  {498, 460, 5, 45, 0, 1, 90.0,  1}; 
struct BAR v5 =  {505, 460, 5, 12, 0, 1, 25.0,  1}; 
struct BAR v6 =  {512, 460, 5, 25, 0, 1, 45.0,  1}; 
struct BAR v7 =  {519, 460, 5, 43, 0, 1, 85.0,  1}; 
struct BAR v8 =  {526, 460, 5, 28, 0, 1, 55.0,  1}; 
struct BAR v9 =  {533, 460, 5, 3, 0, 1, 5.0,   1}; 
struct BAR v10 = {540, 460, 5, 35, 0, 1, 70.0,  1}; 


S2D_Window *window; 


/* 
 * Function :   InsertPhoneDidgit 
 * Description: Insert the input caracter in the sirial string 
 *  
 * Parameters:  c           Input character 
 * 
 * Return       No return value 
 *  
 */ 


void InsertPhoneDidgit(char c) 
{ 
    if (validPhoneNumber == 0) 
    { 
        phoneNumber[didgitIndex++] = c; 
        if (didgitIndex >= MAX_CODE_NUMBER_DIGIT) 
            didgitIndex = 0; 
        //DisplayPhoneNumber(); 
     } 
} 


/* 
 * Function :   CheckPhoneNumber 
 * Description: Check if the serial string is the same as the sample one 
 *  
 * Parameters:  None 
 * 
 * Return       No return value 
 *  
 */ 
int CheckPhoneNumber() 
{ 
	if (laserActivated == 1)
	{
        validPhoneNumber = 1; 
		int i;  
		for (int j = 0; j < MAX_CODE_NUMBER_DIGIT; j++) 
		{ 
			validPhoneNumber = 1; 
			for (i = 0; i < MAX_CODE_NUMBER_DIGIT; i++) 
				if (phoneNumber[(i+j) % MAX_CODE_NUMBER_DIGIT] != RefPhoneNumber[i]) 
				{ 
					validPhoneNumber = 0; 
					break; 
				} 
			if (validPhoneNumber == 1) 
				break; 
		} 
	}    
    return validPhoneNumber; 
} 

/* 
 * Function :   checkBypass 
 * Description: Check if the corresponding bypass file is present in the current directory 
 *              if it is it means that we need to bypass the correcponding switch 
 *  
 * Parameters:  file        Name of the file to check 
 *              pin         Pin to drive for the bypass 
 *              state       State of the pin to set 
 *              startTime   Needed when a 10 second delay is required before restoring initial value 
 * 
 * Return       No return value 
 *  
 */ 
void checkBypass(char *file, int pin, int state, time_t *startTime, int delay) 
{ 
	FILE *file1; 
     


	file1 = fopen(file, "rb"); 
	if (file1) 
	{
 
        if (*startTime == noTimer) 
        { 
            if ((pin == BYPASS_LASER_KEY) && (laserActivated == 0))
            {
              for (int i = 0; i < 8; i++)
                    phoneNumber[i] = ' ';
                S2D_PlayMusic(mus, true);  // play on a loop 
                laserActivated = 1;
            }
            digitalWrite(pin, state); 
            remove(file); 
        } 
        else if (*startTime == 0) 
        { 
            *startTime = time(NULL); 
            digitalWrite(pin, state); 
        } 
        else 
        { 
            time_t curTime = time(NULL); 
            if ((curTime - *startTime) > delay) 
                { 
                    if (state == LOW) 
                        digitalWrite(pin, HIGH); 
                    else 
                        digitalWrite(pin, LOW); 

                    remove(file); 

                } 
        } 
        fclose(file1); 
	} 
} 


/* 
 * Function :   checkEndRequest 
 * Description: Check if the end file is present 
 *              if it is it means that we need to stop the program 
 *  
 * Parameters:  file        Name of the file to check 
 * 
 * Return       No return value 
 *  
 */ 
int checkEndRequest(char *file) 
{ 
	FILE *file1; 
     


	file1 = fopen(file, "rb"); 
	if (file1) 
	{ 
        fclose(file1); 
        return 1; 
	} 
    else 
    { 
        return 0; 
    } 
} 


/* 
 * Function :   TimedActivate 
 * Description: Activate n IO pin for 10 seconds1 
 *  
 * Parameters:  pin         Pin to drive for the bypass 
 *              state       State of the pin to set 
 *              startTime   Needed when a 10 second delay is required before restoring initial value 
 * 
 * Return       No return value 
 *  
 */ 
void TimedActivate(int pin, int state, time_t *startTime, int delay) 
{ 


    if (*startTime == 0) 
    { 
        *startTime = time(NULL); 
        digitalWrite(pin, state); 
    } 
    else 
    { 
        time_t curTime = time(NULL); 
        if ((curTime - *startTime) > delay) 
            { 
                if (state == LOW) 
                    digitalWrite(pin, HIGH); 
                else 
                    digitalWrite(pin, LOW); 
            } 
    } 
} 


bool debunceLightsInput(int input) 
{ 
    if (digitalRead(input) == 0)
    { 
		if  (lightsState == 0)
		{
			lightsState = 1;
			return true; 
		}
		else
		{
			return false;
		}
    } 
	else
	{
		lightsState = 0;
        return false; 
	}
} 
                             
/* 
 * Function :   CheckControls 
 * Description: Verify all game inputs and bypass 
 *  
 * Parameters:  None 
 * 
 * Return       No return value 
 *  
 */ 
void CheckControls() 
{ 
    // Gun management 
    // Check if the Gun switch is pressed (set to ground) 
    // If pressed then close the corresponding light and open the door if all lights are off 
    if (debunceLightsInput(GUN_SWITCH) == true) 
    { 
		switch (gunLights) 
		{ 
			case 3: 
				digitalWrite(GUN_LIGHT_3, HIGH); 
				gunLights--; 
				break; 
			case 2: 
				digitalWrite(GUN_LIGHT_2, HIGH); 
				gunLights--; 
				break; 
			case 1: 
				// TimedActivate(GUN_DOOR, HIGH, &gunDoorTimer, DOOR_GACHE_DELAY); 
				digitalWrite(GUN_LIGHT_1, HIGH); 
				digitalWrite(GUN_DOOR, HIGH); 
				gunLights--; 
				break; 
		} 
    } 
     
    // Laser Key management 
    // Check if Laser key was turned (set to ground) 
    // If turnes then enable Laser 
    if ((digitalRead(LASER_KEY) == 0) && (laserActivated == 0)) 
    { 
        laserActivated = 1; 
        //S2D_PlaySound(laser); 
        for (int i = 0; i< 8; i++)
            phoneNumber[i] = ' ';
        S2D_PlayMusic(mus, true);  // play on a loop 
        digitalWrite(LASER_POWER, LOW); 
         
    } 
}

/* 
 * Function :   CheckAllBypasss 
 * Description: Verify all game inputs and bypass 
 *  
 * Parameters:  None 
 * 
 * Return       No return value 
 *  
 */ 
void CheckAllBypass() 
{ 
    // Bypass management     
    // Check if any of the bypass file exist 
    checkBypass(laserKeyBypass, BYPASS_LASER_KEY, LOW, &noTimer, 0); 
    checkBypass(gunBypass, BYPASS_GUN, HIGH, &noTimer, 0); 
    checkBypass(tvLiftUpBypass, BYPASS_TV_LIFT_UP, LOW, &tvLiftUpTimer, TV_LIFT_UP_DELAY); 
    checkBypass(tvLiftDownBypass, BYPASS_TV_LIFT_DOWN, LOW, &tvLiftDownTimer, TV_LIFT_DOWN_DELAY); 
    checkBypass(handSensorBypass, BYPASS_HAND_SENSOR, HIGH, &noTimer, 0); 
} 


/* 
 * Function :   RenderVerticalMessage 
 * Description: Verify all game inputs and bypass 
 *  
 * Parameters:  x           x coordinate of the string to display 
 *              y           x coordinate of the string to display 
 *              *message    String to display 
 * 
 * Return       No return value 
 *  
 */ 
void RenderVerticalMessage(int x, int y, char *message) 
{ 
    int i=0; 
     
    txt->x = x; 
    txt->y = y; 
         
    while (message[i] != '\0') 
    { 
        letter[0] = message[i++]; 
        S2D_SetText(txt, letter); 
        S2D_DrawText(txt); 
        txt->y = txt->y + 35; 


    } 
     
} 


/* 
 * Function :   CalcRandColor 
 * Description: Calculate random color sequence to display in the radar 
 *  
 * Parameters:  None 
 * 
 * Return       No return value 
 *  
 */ 
void CalcRandColor() 
{ 
	randColor[0] = rand() % MAX_COLOR; 
	while ((randColor[1] = (rand() % MAX_COLOR)) == randColor[0]); 
    while ((randColor[2] = (rand() % MAX_COLOR)) == randColor[1]); 
    while ((randColor[3] = (rand() % MAX_COLOR)) == randColor[2]); 
	while ((randColor[4] = (rand() % MAX_COLOR)) == randColor[3]); 
	while ((randColor[5] = (rand() % MAX_COLOR)) == randColor[4]); 
    while ((randColor[6] = (rand() % MAX_COLOR)) == randColor[5]); 
	while ((randColor[7] = (rand() % MAX_COLOR)) == randColor[6]); 
	while ((randColor[8] = (rand() % MAX_COLOR)) == randColor[7]); 
	while ((randColor[9] = (rand() % MAX_COLOR)) == randColor[8]); 
} 


/* 
 * Function :   ResetStage 
 * Description: Bring the game back to the first level 
 *  
 * Parameters:  None 
 * 
 * Return       No return value 
 *  
 */ 
void ResetStage() 
{ 
    sequenceElapsedTime = 0; 
    pressed = 0; 
    lastColorPressed = -1; 
    MaxStage = MIN_STAGE; 
    sprintf(message, "Stage : %d", MaxStage); 
    //S2D_SetText(txtTop, message); 
    //S2D_SetText(txtTop2, message); 
    //S2D_SetText(txtBot, message); 
    //S2D_SetText(txtBot2, message); 
    //S2D_PlaySound(failSnd); 
    //i = 0; 
    //angle = 0; 
    //j = -640;   
    txtTop->x = i++; 
    txtTop2->x = j++; 


    txtBot->x = i++; 
    txtBot2->x = j++; 
} 


/* 
 * Function :   CompareSequence 
 * Description: Check if the input color is the proper one 
 *  
 * Parameters:  aColor      The color of the button pressed 
 * 
 * Return       No return value 
 *  
 */ 
void CompareSequence(int aColor) 
{ 
    lastColorPressed = aColor; 
    if (color[randColor[pressed]][3] == aColor) 
    { 
        if (pressed == 0) 
        { 
            sequenceElapsedTime = 0; 
        } 
        pressed++; 
             
        if (pressed == MaxStage) 
        { 
            srand(rand()); 
            CalcRandColor(); 
             
            clearScreenDelay = 0; 
            MaxStage++; 
            pressed = 0; 
            nbTime = 0; 
            lastColorPressed = -1; 
            sprintf(message, "Stage : %d", MaxStage); 


            //S2D_SetText(txtTop, message); 
            //S2D_SetText(txtTop2, message); 
            //S2D_SetText(txtBot, message); 
            //S2D_SetText(txtBot2, message); 
            S2D_PlaySound(succesSnd); 
            //i = 0; 
            //angle = 0; 
            //j = -640;   
            txtTop->x = i++; 
            txtTop2->x = j++; 


            txtBot->x = i++; 
            txtBot2->x = j++; 
         
            if (MaxStage == MAX_STAGES) 
            { 
                missionCompleted = true; 
                S2D_SetText(txtTop, "7 4 1 9 5"); 
                S2D_SetText(txtTop2, "Mission Complete!"); 
                S2D_SetText(txtBot, "Mission Completed!"); 
                S2D_SetText(txtBot2, "Mission Completed!"); 
                //S2D_PlaySound(endSnd); 
            }             
        } 
    } 
    else 
    { 
        ResetStage(); 
    } 
} 


/* 
 * Function :   InputButtonPressed 
 * Description: Debouncing the botton entry 
 *  
 * Parameters:  button      The button pressed 
 * 
 * Return       bool        one if a button was pressed 
 *  
 */ 
bool InputButtonPressed(int button) 
{ 

    if (digitalRead(button) == 0)
    { 
		if  (colorState[button] == 0)
		{
			colorState[button] = 1;
			return true; 
		}
		else
		{
			return false;
		}
    } 
	else
	{
		colorState[button] = 0;
        return false; 
	} 
} 


/* 
 * Function :   CheckColorButtons 
 * Description: Cicle between button colors to see if one was pressed 
 *  
 * Parameters:  None 
 * 
 * Return       No return value 
 *  
 */ 
void CheckColorButtons() 
 { 
    if (pressed != 0) 
        sequenceElapsedTime++; 
         
    if (sequenceElapsedTime > MAX_ELAPSED_TIME) 
        ResetStage(); 
         
    if (lastColorPressed != RED) 
    { 
        if (InputButtonPressed(RED) == true) 
        { 
            CompareSequence(RED); 
        } 
    } 
    if (lastColorPressed != BLUE) 
    { 
        if (InputButtonPressed(BLUE) == true) 
        { 
            CompareSequence(BLUE); 
        } 
    } 
    if (lastColorPressed != YELLOW) 
    { 
        if (InputButtonPressed(YELLOW) == true) 
        { 
            CompareSequence(YELLOW); 
        } 
    } 
    if (lastColorPressed != GREEN) 
    { 
        if (InputButtonPressed(GREEN) == true) 
        { 
            CompareSequence(GREEN); 
        } 
    } 
    if (lastColorPressed != WHITE) 
    { 
        if (InputButtonPressed(WHITE) == true) 
        { 
            CompareSequence(WHITE); 
        } 
    }   
} 


/* 
 * Function :   RenderBar 
 * Description: Render a bar on the screen 
 *  
 * Parameters:  *bar        Which bar to draw 
 *               
 * 
 * Return       No return value 
 *  
 */ 
void RenderBar(struct BAR *bar) 
{ 


   int length = (int)((float) bar->actual * ((float) MaxStage / (float) MAX_STAGES)); 


   if (bar->orientation == 0) 
    { 
         
        S2D_DrawTriangle( 
            bar->x,          bar->y,               0, 0.7, 0, 1, 
            bar->x + length, bar->y,               0, 0.7, 0, 1, 
            bar->x + length, bar->y + bar->width,  0, 0.7, 0, 1); 
        S2D_DrawTriangle( 
            bar->x,          bar->y,               0, 0.7, 0, 1, 
            bar->x + length, bar->y + bar->width,  0, 0.7, 0, 1, 
            bar->x,          bar->y + bar->width,  0, 0.7, 0, 1);   
    } 
    else 
    {     
        S2D_DrawTriangle( 
            bar->x,               bar->y,          0, 0.7, 0, 1, 
            bar->x + bar->width,  bar->y,          0, 0.7, 0, 1, 
            bar->x + bar->width,  bar->y - length, 0, 0.7, 0, 1); 
        S2D_DrawTriangle( 
            bar->x,               bar->y,          0, 0.7, 0, 1, 
            bar->x + bar->width,  bar->y - length, 0, 0.7, 0, 1, 
            bar->x,               bar->y - length, 0, 0.7, 0, 1); 
    }           
     
   if (bar->actual >= bar->height) 
        bar->increment = -1; 
   if (bar->actual <= 0) 
   { 
        bar->increment = 1; 
        bar->height = rand() % 50; 
   } 
   bar->actual = bar->actual + bar->increment; 
} 


/* 
 * Function :   RenderVoice 
 * Description: Render the voice waveform 
 *  
 * Parameters:  *offet      Start position of rendering 
 *               
 * 
 * Return       No return value 
 *  
 */ 
void RenderVoice(int *offset) 
{ 
    voice->x = *offset; 
    S2D_DrawImage(voice); 
    voice->x = *offset +100; 
    S2D_DrawImage(voice); 
    voice->x = *offset +200; 
    S2D_DrawImage(voice); 
    if ((*offset)++ == 400) 
        *offset = 300; 
} 


/* 
 * Function :   ClearScreen 
 * Description: Set the screen to black 
 *  
 * Parameters:  None 
 *               
 * 
 * Return       No return value 
 *  
 */ 
void ClearScreen() 
{ 
    S2D_DrawTriangle( 
        320,  90, 0, 0, 0, 1, 
        150, 165, 0, 0, 0, 1, 
        150, 315, 0, 0, 0, 1); 


    S2D_DrawTriangle( 
        320,  90, 0, 0, 0, 1, 
        150, 315, 0, 0, 0, 1, 
        320, 390, 0, 0, 0, 1); 


    S2D_DrawTriangle( 
        320,  90, 0, 0, 0, 1, 
        320, 390, 0, 0, 0, 1, 
        490, 315, 0, 0, 0, 1); 


    S2D_DrawTriangle( 
        320,  90, 0, 0, 0, 1, 
        490, 315, 0, 0, 0, 1, 
        490, 165, 0, 0, 0, 1); 
} 


/* 
 * Function :   Render 
 * Description: Draw the radar 
 *  
 * Parameters:  None 
 *               
 * 
 * Return       No return value 
 *  
 */ 
void Render()  
{ 
    if (!validPhoneNumber) 
    { 
        TimedActivate(TV_LIFT_DOWN, LOW, &tvLiftDownTimer, TV_LIFT_DOWN_DELAY); 
        CheckControls(); 
        CheckAllBypass();
        ClearScreen(); 
        return; 
    } 
    else 
    { 
        // Bring TV UP 
        TimedActivate(TV_LIFT_UP, LOW, &tvLiftUpTimer, TV_LIFT_UP_DELAY); 
        // Shutdown Laser 
        digitalWrite(LASER_POWER, HIGH); 
        S2D_FadeOutMusic(2000); 

        CheckAllBypass(); 
    } 


    if (currentStage != MaxStage) 
    { 
        currentStage = MaxStage; 
        for (int wait = 0; wait < 10000000; wait++) 
            digitalRead(0); 
    } 
    CheckColorButtons(); 
	    
    if (clearScreenDelay++ < CLEAR_SCREEN_DELAY) 
    { 
        ClearScreen(); 


        k = 0; 
    } 
	else 
	{ 
        if (missionCompleted == false) 
		{ 
            if ((++k % DISPLAY_TIME) == 0) 
            { 
                nbTime++; 
            }; 
				 
            if (nbTime == MaxStage) 
            { 
                clearScreenDelay = 0; 
                nbTime = nbTime % MaxStage; 
            } 
            else 
            { 
                nbTime = nbTime % MaxStage; 


                S2D_DrawTriangle( 
                    320,  90, color[randColor[nbTime]][0], color[randColor[nbTime]][1], color[randColor[nbTime]][2], 1, 
                    150, 165, color[randColor[nbTime]][0], color[randColor[nbTime]][1], color[randColor[nbTime]][2], 1, 
                    150, 315, color[randColor[nbTime]][0], color[randColor[nbTime]][1], color[randColor[nbTime]][2], 1); 


                S2D_DrawTriangle( 
                    320,  90, color[randColor[nbTime]][0], color[randColor[nbTime]][1], color[randColor[nbTime]][2], 1, 
                    150, 315, color[randColor[nbTime]][0], color[randColor[nbTime]][1], color[randColor[nbTime]][2], 1, 
                    320, 390, color[randColor[nbTime]][0], color[randColor[nbTime]][1], color[randColor[nbTime]][2], 1); 
	   
                S2D_DrawTriangle( 
                    320,  90, color[randColor[nbTime]][0], color[randColor[nbTime]][1], color[randColor[nbTime]][2], 1, 
                    320, 390, color[randColor[nbTime]][0], color[randColor[nbTime]][1], color[randColor[nbTime]][2], 1, 
                    490, 315, color[randColor[nbTime]][0], color[randColor[nbTime]][1], color[randColor[nbTime]][2], 1); 


                S2D_DrawTriangle( 
                    320,  90, color[randColor[nbTime]][0], color[randColor[nbTime]][1], color[randColor[nbTime]][2], 1, 
                    490, 315, color[randColor[nbTime]][0], color[randColor[nbTime]][1], color[randColor[nbTime]][2], 1, 
                    490, 165, color[randColor[nbTime]][0], color[randColor[nbTime]][1], color[randColor[nbTime]][2], 1); 
            } 
        } 
    } 


    if (missionCompleted == false) 
    { 
        S2D_DrawTriangle( 
            220, 239, 0, 0.7, 0, 1, 
            420, 239, 0, 0.7, 0, 1, 
            420, 241, 0, 0.7, 0, 1); 
        S2D_DrawTriangle( 
            220, 239, 0, 0.7, 0, 1, 
            420, 241, 0, 0.7, 0, 1, 
            220, 241, 0, 0.7, 0, 1); 


        S2D_DrawTriangle( 
            319, 140, 0, 0.7, 0, 1, 
            319, 340, 0, 0.7, 0, 1, 
            321, 340, 0, 0.7, 0, 1); 
        S2D_DrawTriangle( 
            319, 140, 0, 0.7, 0, 1, 
            321, 340, 0, 0.7, 0, 1, 
            321, 140, 0, 0.7, 0, 1); 
    }                


    S2D_RotateImage(scan, (angle++%360), S2D_TOP_RIGHT); 
    S2D_DrawImage(scan); 
     
    RenderVoice(&voiceOffset); 


    S2D_DrawTriangle( 
        550, 0, 0, 0, 0, 1, 
        640, 0, 0, 0, 0, 1, 
        640, 100, 0, 0, 0, 1); 
    S2D_DrawTriangle( 
        550, 0, 0, 0, 0, 1, 
        640, 100, 0, 0, 0, 1, 
        550, 100, 0, 0, 0, 1);    


    S2D_DrawImage(img); 
   
    RenderBar(&h0); 
    RenderBar(&h1); 
    RenderBar(&h2); 
    RenderBar(&h3); 
    RenderBar(&h4); 
    RenderBar(&h5); 
     
    RenderBar(&v0); 
    RenderBar(&v1); 
    RenderBar(&v2); 
    RenderBar(&v3); 
    RenderBar(&v4); 
    RenderBar(&v5); 
    RenderBar(&v6); 
    RenderBar(&v7); 
    RenderBar(&v8); 
    RenderBar(&v9); 
    RenderBar(&v10); 
   
    S2D_DrawLine(78 + MaxStage * 7, 430, 186, 446, 1, 0, 0.7, 0, 1, 0, 0.7, 0, 1, 0, 0.7, 0, 1, 0, 0.7, 0, 1); 
    S2D_DrawTriangle(75 + MaxStage * 7, 428, 0, 0.7, 0, 1, 
                     81 + MaxStage * 7, 428, 0, 0.7, 0, 1, 
                     78 + MaxStage * 7, 434, 0, 0.7, 0, 1); 
    S2D_DrawTriangle(78 + MaxStage * 7, 426, 0, 0.7, 0, 1, 
                     81 + MaxStage * 7, 432, 0, 0.7, 0, 1, 
                     75 + MaxStage * 7, 432, 0, 0.7, 0, 1); 


    Missile->x = 25; 
    Missile->y = i; 
    S2D_DrawImage(Missile); 
   
    Lance->y = i + 270; 
    S2D_DrawImage(Lance); 


    Missile->x = 580; 
    Missile->y = i; 
    S2D_DrawImage(Missile); 
  
    Launched->y = i + 270; 
    S2D_DrawImage(Launched); 


    Missile->x = 25; 
    Missile->y = j; 
    S2D_DrawImage(Missile); 
   
    Lance->y = j + 270; 
    S2D_DrawImage(Lance); 


    Missile->x = 580; 
    Missile->y = j; 
    S2D_DrawImage(Missile); 
  
    Launched->y = j + 270; 
    S2D_DrawImage(Launched); 
   
    if (missionCompleted == false) 
    { 
     	if (i==640) 
        { 
            i = 0; 
            j = -640; 
        } 
	   
        txtTop->x = i++; 
        txtTop2->x = j++; 


        txtBot->x = i++; 
        txtBot2->x = j++; 
    } 
    else 
    { 
       if ((k++ % DISPLAY_TIME) == 0) 
        { 
            nbTime++; 
        }; 
             
        if (nbTime % 2) 
        { 
            txtTop->x = 240; 
            txtTop->y = 220; 


            S2D_DrawText(txtTop); 
        } 
    } 
} 




/* 
 * Function :   Monitor keyboard 
 *  
 * Parameters:  S2D_Event       Keyboard event 
 *               
 * 
 * Return       No return value 
 *  
 */ 
void on_key(S2D_Event e)  
{ 
    char myKey;

    switch (e.type)  
    { 
    case S2D_KEY_DOWN:

        
        myKey = e.key[0];
        //printf("Key down: %s, %c\n", e.key, e.key[0]); 
        //if (strcmp(e.key, "Escape") == 0) S2D_Close(window); 
        //if (strcmp(e.key, "Return") == 0)  
        //{ 
        //    MaxStage++; 
        //    S2D_PlaySound(succesSnd); 
        //    if (MaxStage == MAX_STAGES) 
        //    { 
        //        missionCompleted = true; 
        //        S2D_SetText(txtTop, "7 4 1 9 5"); 
        //        S2D_SetText(txtTop2, "Mission Complete!"); 
        //        S2D_SetText(txtBot, "Mission Completed!"); 
        //        S2D_SetText(txtBot2, "Mission Completed!"); 
        //        S2D_PlaySound(endSnd); 
        //    } 


        //} 
        if (strcmp(e.key, "Keypad 0") == 0) myKey = '0';
        if (strcmp(e.key, "Keypad 1") == 0) myKey = '1';
        if (strcmp(e.key, "Keypad 2") == 0) myKey = '2';
        if (strcmp(e.key, "Keypad 3") == 0) myKey = '3';
        if (strcmp(e.key, "Keypad 4") == 0) myKey = '4';
        if (strcmp(e.key, "Keypad 5") == 0) myKey = '5';
        if (strcmp(e.key, "Keypad 6") == 0) myKey = '6';
        if (strcmp(e.key, "Keypad 7") == 0) myKey = '7';
        if (strcmp(e.key, "Keypad 8") == 0) myKey = '8';
        if (strcmp(e.key, "Keypad 9") == 0) myKey = '9';
        
        InsertPhoneDidgit(myKey); 
        if (CheckPhoneNumber() == 1) 
        { 
            TimedActivate(TV_LIFT_UP, LOW, &tvLiftUpTimer, TV_LIFT_UP_DELAY); 
        }         
        break; 


    case S2D_KEY_HELD: 
         break; 


    case S2D_KEY_UP: 
         break; 
    } 
} 


void Update() 
{ 
    if (checkEndRequest(stopBunker) == 1) 
        { 
            S2D_FreeWindow(window); 
        } 
} 


void cleanUpFiles() 
{ 
    remove("BYPASS_GUN"); 
    remove("BYPASS_LASER_KEY"); 
    remove("BYPASS_TV_LIFT_UP"); 
    remove("BYPASS_TV_LIFT_DOWN"); 
    remove("BYPASS_HAND_SENSOR");     
    remove("STOP_BUNKER"); 
} 


/* 
 * Function :   main 
 *  
 * Parameters:  None       Keyboard event 
 *               
 * 
 * Return       int         1 
 *  
 */ 
int main()  
{     
    cleanUpFiles(); 
     
    wiringPiSetup() ; 
    laserActivated = 0; 
	gunLights = 3; 
	lightsState = 0;
	colorState[RED]    = 0;
	colorState[BLUE]   = 0;
	colorState[YELLOW] = 0;
	colorState[GREEN]  = 0;
	colorState[WHITE]  = 0;

    pinMode(SIMON_RED_BUTTON,   INPUT); 
    pinMode(SIMON_BLUE_BUTTON,  INPUT); 
    pinMode(SIMON_YELLOW_BUTTON,INPUT); 
    pinMode(SIMON_GREEN_BUTTON, INPUT); 
    pinMode(SIMON_WHITE_BUTTON, INPUT); 
     
    pinMode(GUN_SWITCH,         INPUT); 
    pinMode(GUN_DOOR,           OUTPUT); 
    pinMode(GUN_LIGHT_1,        OUTPUT); 
    pinMode(GUN_LIGHT_2,        OUTPUT); 
    pinMode(GUN_LIGHT_3,        OUTPUT); 
    pinMode(LASER_KEY,          INPUT); 
    pinMode(LASER_POWER,        OUTPUT); 
    pinMode(TV_LIFT_UP,         OUTPUT); 
    pinMode(TV_LIFT_DOWN,       OUTPUT); 
    pinMode(BYPASS_HAND_SENSOR, OUTPUT); 


    pullUpDnControl(SIMON_RED_BUTTON,    PUD_UP); 
    pullUpDnControl(SIMON_BLUE_BUTTON,   PUD_UP); 
    pullUpDnControl(SIMON_YELLOW_BUTTON, PUD_UP); 
    pullUpDnControl(SIMON_GREEN_BUTTON,  PUD_UP); 
    pullUpDnControl(SIMON_WHITE_BUTTON,  PUD_UP); 
         
    pullUpDnControl(GUN_SWITCH,         PUD_UP); 
    pullUpDnControl(GUN_DOOR,           PUD_UP); 
    digitalWrite(GUN_DOOR,              LOW); 
    pullUpDnControl(GUN_LIGHT_1,        PUD_UP); 
    digitalWrite(GUN_LIGHT_1,           LOW); 
    pullUpDnControl(GUN_LIGHT_2,        PUD_UP); 
    digitalWrite(GUN_LIGHT_2,           LOW); 
    pullUpDnControl(GUN_LIGHT_3,        PUD_UP); 
    digitalWrite(GUN_LIGHT_3,           LOW); 
    pullUpDnControl(LASER_KEY,          PUD_UP); 
    pullUpDnControl(LASER_POWER,        PUD_UP); 
    digitalWrite(LASER_POWER,           HIGH); 
    pullUpDnControl(TV_LIFT_UP,         PUD_UP); 
    digitalWrite(TV_LIFT_UP,            HIGH); 
    pullUpDnControl(TV_LIFT_DOWN,       PUD_UP); 
    digitalWrite(TV_LIFT_DOWN,          HIGH);     
    pullUpDnControl(BYPASS_HAND_SENSOR, PUD_UP); 
    digitalWrite(BYPASS_HAND_SENSOR,    LOW); 
     
    srand(rand()); 
    CalcRandColor(); 


    message = (char*) malloc(100 * sizeof(char)); 


    letter = (char*) malloc(2 * sizeof(char)); 
    sprintf(letter, " "); 
     
    txt = S2D_CreateText("Alien-Encounters-Regular.ttf", "", 40); 


    window = S2D_CreateWindow("Hello Triangle", 640, 480, Update, Render, 0); 


    img = S2D_CreateImage("Radar2TransVide.png"); 
    scan = S2D_CreateImage("aiguillePPSuperSmall.png");     
    Missile = S2D_CreateImage("Missile.png");     
    Launched = S2D_CreateImage("Launched.png");     
    Lance = S2D_CreateImage("Lance.png");     
    Deactivated = S2D_CreateImage("Deactivated.png");     
    Desactive = S2D_CreateImage("Desactive.png");     
    voice = S2D_CreateImage("Voice.png");     


    img->x = 70; 
    img->y = 0; 
    img->width  = 500; 
    img->height = 480; 


    Missile->x = 25; 
    Missile->y = 0; 
    Missile->width  = 30; 
    Missile->height = 250; 
     
    Lance->x = 25; 
    Lance->y = 0; 
    Lance->width  = 30; 
    Lance->height = 200;     


    Launched->x = 580; 
    Launched->y = 0; 
    Launched->width  = 30; 
    Launched->height = 300;     


    Desactive->x = 580; 
    Desactive->y = 0; 
    Desactive->width  = 30; 
    Desactive->height = 300;     


    Deactivated->x = 580; 
    Deactivated->y = 0; 
    Deactivated->width  = 30; 
    Deactivated->height = 300;     
     
    scan->x = 220; 
    scan->y = 240; 
    scan->width  = 100; 
    scan->height = 125; 
     
    voice->width  = 100; 
    voice->height = 30; 
    voice->x = 500; 
    voice->y = 23; 




    txtTop = S2D_CreateText("Alien-Encounters-Regular.ttf", "Missile Lance", 40); 
    txtTop2 = S2D_CreateText("Alien-Encounters-Regular.ttf", "Missile Lance", 40); 
    txtBot = S2D_CreateText("Alien-Encounters-Regular.ttf", "Missile Launched", 40); 
    txtBot2 = S2D_CreateText("Alien-Encounters-Regular.ttf", "Missile Launched", 40); 


    txtTop->y = 0; 
    txtTop2->y = 0; 
    txtBot->y = 430; 
    txtBot2->y = 430; 
	   
    txtTop->color.r = 0.0; 
    txtTop->color.g = 0.8; 
    txtTop->color.b = 0.0; 
    txtTop->color.a = 1.0; 
	 
    txtTop2->color.r = 0.0; 
    txtTop2->color.g = 0.8; 
    txtTop2->color.b = 0.0; 
    txtTop2->color.a = 1.0;     


    txtBot->color.r = 0.0; 
    txtBot->color.g = 0.8; 
    txtBot->color.b = 0.0; 
    txtBot->color.a = 1.0; 
		 
    txtBot2->color.r = 0.0; 
    txtBot2->color.g = 0.8; 
    txtBot2->color.b = 0.0; 
    txtBot2->color.a = 1.0;     


    txt->color.r = 0.0; 
    txt->color.g = 0.8; 
    txt->color.b = 0.0; 
    txt->color.a = 1.0; 
    mus = S2D_CreateMusic("laserSound.mp3"); 
    succesSnd = S2D_CreateSound("radarSoundEffect32.wav"); 
    failSnd = S2D_CreateSound("NFF-success.wav"); 
    endSnd = S2D_CreateSound("tada.wav"); 


    window->on_key = on_key; 


    S2D_ShowCursor(false); 
    S2D_Show(window); 
    S2D_FreeImage(scan);   
    S2D_FreeImage(img);   
    S2D_FreeImage(Missile); 
    S2D_FreeImage(Lance); 
    S2D_FreeImage(Launched); 
    S2D_FreeImage(Deactivated); 
    S2D_FreeImage(Desactive); 
    S2D_FreeImage(voice); 


    free(message); 
       
    S2D_FreeText(txtTop); 
    S2D_FreeText(txtTop2); 
    S2D_FreeText(txtBot); 
    S2D_FreeText(txtBot2);   


    free(letter); 
    S2D_FreeText(txt); 


    S2D_FreeSound(endSnd);   
    S2D_FreeSound(succesSnd);   
    S2D_FreeSound(failSnd); 
    S2D_FreeMusic(mus); 
     
    //S2D_FreeWindow(window); 


    S2D_Close(window); 


    digitalWrite(GUN_DOOR,              HIGH); 
    digitalWrite(LASER_POWER,           HIGH); 
    digitalWrite(TV_LIFT_UP,            HIGH); 
    digitalWrite(TV_LIFT_DOWN,          HIGH);     
    digitalWrite(GUN_LIGHT_1,           HIGH); 
    digitalWrite(GUN_LIGHT_2,           HIGH); 
    digitalWrite(GUN_LIGHT_3,           HIGH);	 
    digitalWrite(BYPASS_HAND_SENSOR,    HIGH);	 


    return 0; 
} 

#include<iostream>//Lighting implemented
#include<GL/gl.h>
#include<GL/glut.h>
#include<GL/glu.h>
#include <stdlib.h>
#include<string.h>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<cmath>
#include<ctime>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

#define UINTSIZE sizeof(unsigned int)
#define FLOATSIZE sizeof(float)
#define INF 1e8
using namespace std;

GLfloat UpwardsScrollVelocity = -40.0;
float view=20.0;

int pos=1;
char quote[6][80];

int numberOfQuotes=0,i;

/*sound functionality*/
///////////////
// Buffers to hold sound data.
ALuint Buffer[3];

// Sources are points of emitting sound.
ALuint Source[3];

// Position of the source sound.
ALfloat SourcePos[] = { 0.0, 0.0, 0.0 };

// Velocity of the source sound.
ALfloat SourceVel[] = { 0.0, 0.0, 0.0 };


// Position of the Listener.
ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };

// Velocity of the Listener.
ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };

// Orientation of the Listener. (first 3 elements are "at", second 3 are "up")
// Also note that these should be units of '1'.
ALfloat ListenerOri[] = { 0.0, 0.0, -1.0,  0.0, 1.0, 0.0 };



/*
 * ALboolean LoadALData()
 *
 *	This function will load our sample data from the disk using the Alut
 *	utility and send the data into OpenAL as a buffer. A source is then
 *	also created to play that buffer.
 */
ALboolean LoadALData()
{
	// Variables to load into.

	ALenum format;
	ALsizei size;
	ALvoid* data;
	ALsizei freq;
	ALboolean loop;

	// Load wav data into a buffer.

	alGenBuffers(3, &Buffer[0]);

	if(alGetError() != AL_NO_ERROR)
		return AL_FALSE;

	char *ch="intro.wav";//intro music
	ALbyte * filename=(ALbyte*)ch;
	alutLoadWAVFile(filename, &format, &data, &size, &freq, &loop);
	alBufferData(Buffer[0], format, data, size, freq);
	alutUnloadWAV(format, data, size, freq);
	
	char *ch2="select.wav";// menu select music
	ALbyte * filename2=(ALbyte*)ch2;
	alutLoadWAVFile(filename2, &format, &data, &size, &freq, &loop);
	alBufferData(Buffer[1], format, data, size, freq);
	alutUnloadWAV(format, data, size, freq);
	
	char *ch3="start.wav";// menu select music
	ALbyte * filename3=(ALbyte*)ch3;
	alutLoadWAVFile(filename3, &format, &data, &size, &freq, &loop);
	alBufferData(Buffer[2], format, data, size, freq);
	alutUnloadWAV(format, data, size, freq);

	// Bind the buffer with the source.

	alGenSources(3, &Source[0]);

	if(alGetError() != AL_NO_ERROR)
		return AL_FALSE;

	alSourcei (Source[0], AL_BUFFER,   Buffer[0]   );
	alSourcef (Source[0], AL_PITCH,    1.0      );
	alSourcef (Source[0], AL_GAIN,     0.3f     );
	alSourcefv(Source[0], AL_POSITION, SourcePos);
	alSourcefv(Source[0], AL_VELOCITY, SourceVel);
	alSourcei (Source[0], AL_LOOPING,  AL_TRUE   );
	
	alSourcei (Source[1], AL_BUFFER,   Buffer[1]   );
	alSourcef (Source[1], AL_PITCH,    1.0      );
	alSourcef (Source[1], AL_GAIN,     1.0      );
	alSourcefv(Source[1], AL_POSITION, SourcePos);
	alSourcefv(Source[1], AL_VELOCITY, SourceVel);
	alSourcei (Source[1], AL_LOOPING,  loop     );
	
	alSourcei (Source[2], AL_BUFFER,   Buffer[2]   );
	alSourcef (Source[2], AL_PITCH,    1.0      );
	alSourcef (Source[2], AL_GAIN,     1.0      );
	alSourcefv(Source[2], AL_POSITION, SourcePos);
	alSourcefv(Source[2], AL_VELOCITY, SourceVel);
	alSourcei (Source[2], AL_LOOPING,  loop     );
	// Do another error check and return.

	if(alGetError() == AL_NO_ERROR)
		return AL_TRUE;

	return AL_FALSE;
}



/*
 * void SetListenerValues()
 *
 *	We already defined certain values for the Listener, but we need
 *	to tell OpenAL to use that data. This function does just that.
 */
void SetListenerValues()
{
	alListenerfv(AL_POSITION,    ListenerPos);
	alListenerfv(AL_VELOCITY,    ListenerVel);
	alListenerfv(AL_ORIENTATION, ListenerOri);
}



/*
 * void KillALData()
 *
 *	We have allocated memory for our buffers and sources which needs
 *	to be returned to the system. This function frees that memory.
 */
void KillALData()
{
	alDeleteBuffers(3, &Buffer[0]);
	alDeleteSources(3, &Source[0]);
	alutExit();
}

//*********************************************
//*  glutIdleFunc(timeTick);                  *
//*********************************************
void play()
{

pid_t pid;
	char *temp;
    char **arg;
    arg=(char**)malloc(100*sizeof(char*));
    char c;
    int i,no,start,out,current_out,fd,check=0;
    	//char str[100];
    	char str[]="./viewer F16.ifs 3dstarscape.ifs";
	
	
		i=0;no=0,out=0,check=0;
		while((c=str[i])!='\0')
		   {
		       arg[no]=(char*)malloc(100*sizeof(char));	
		      
		       while((c=str[i])==' ')
			  i++;
			start=0;
			while((c=str[i])!=' ' && (c=str[i])!='\0')
			  {
			      arg[no][start]=c;
			      start++;
			      i++;
			  }
			  arg[no][start]='\0';
		  	  
		  	  no++;
		  }
		  	
		  arg[no]=(char*)NULL;

	
	pid=fork();
		if(pid==0)   
		   {
		   alSourceStop(Source[0]);
		   KillALData();
		   check=execvp(arg[0],arg);
		   if(check==-1)
		   printf("Command not found\n");
		   _exit(1);
		   }
		   
			
}
void timeTick(void)
{
    if (UpwardsScrollVelocity< -600)
        view-=0.1;
    if(view < 0) {view=20; UpwardsScrollVelocity = -10.0;}
    //  exit(0);
    UpwardsScrollVelocity -= 0.1;
  glutPostRedisplay();

}


//*********************************************
//* printToConsoleWindow()                *
//*********************************************

void printToConsoleWindow()
{
    int l,lenghOfQuote, i;

    for(  l=0;l<numberOfQuotes;l++)
    {
        lenghOfQuote = (int)strlen(quote[l]);

        for (i = 0; i < lenghOfQuote; i++)
        {
          cout<<quote[l][i];
        }
          cout<<endl;
    }

}

//*********************************************
//* RenderToDisplay()                       *
//*********************************************

void RenderToDisplay()
{
    int l,lenghOfQuote, i;

    //glTranslatef(0.0, -100, UpwardsScrollVelocity);
    glRotatef(-20, 1.0, 0.0, 0.0);
    glScalef(0.05, 0.05, 0.05);
    for(  l=0;l<numberOfQuotes;l++)
    {
      glColor3f(1,1,0);
      lenghOfQuote = (int)strlen(quote[l]);
      glPushMatrix();
      if(l==0)
      {
      glTranslatef(-0.0*(lenghOfQuote*37), -((l-4)*200), 0.0);
      glScalef(1.2, 1.2, 1.2);
      }
      if(l==3)
      {
      glTranslatef(-0.6*(lenghOfQuote*37), -((l+2)*200), 0.0);
      glScalef(0.7, 0.7, 0.7);
      }
      else
      {
      glTranslatef(-(lenghOfQuote*37), -((l)*200), 0.0);
      }
      if(l==pos)
           {
           //glutStrokeCharacter(GLUT_STROKE_ROMAN, '-');
               glColor3f(0,1,1);	
            glutStrokeCharacter(GLUT_STROKE_ROMAN, '>');	
            }   
              
        for (i = 0; i < lenghOfQuote; i++)
        {
            //glColor3f((UpwardsScrollVelocity/10)+300+(l*10),(UpwardsScrollVelocity/10)+300+(l*10),0.0);
            //glColor3f(1,1,0);
            glutStrokeCharacter(GLUT_STROKE_ROMAN, quote[l][i]);
        }
       glPopMatrix();
      }
	 
   
}
//*********************************************
//* glutDisplayFunc(myDisplayFunction);       *
//*********************************************

void myDisplayFunction(void)
{
  glClear(GL_COLOR_BUFFER_BIT);
  glLoadIdentity();
  gluLookAt(0.0, 30.0, 100.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  RenderToDisplay();
  glutSwapBuffers();
}
//*********************************************
//* glutReshapeFunc(reshape);               *
//*********************************************

void reshape(int w, int h)
{
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, 1.0, 1.0, 3200);
  glMatrixMode(GL_MODELVIEW);
}


void special_keys(int key, int x, int y)
{
	
	switch(key)
	{
	  case GLUT_KEY_UP:
	  	alSourcePlay(Source[1]);
	 	if(pos==1)pos=2;
		else if(pos==2)pos=1;
		break;
	 case GLUT_KEY_DOWN:
	  	alSourcePlay(Source[1]);
		if(pos==1)pos=2;
		else if(pos==2)pos=1;
		break;
	 }
	glutPostRedisplay();//calls redisplay immediately
}
void process_keys(unsigned char key,int x,int y)
{
	if(key==' ')
	{
		alSourcePlay(Source[2]);
		if(pos==1)
		play();	
		else if(pos==2)
		_exit(1);
		   
				
	}	
}
//*********************************************
//* int main()                                *
//*********************************************
int main(int argc,char** argv)
{
    strcpy(quote[0],"Devil On Wings");
    
    strcpy(quote[1],"Start Game");
    strcpy(quote[2],"Exit");
    strcpy(quote[3],"Press Space to Select");
    
    numberOfQuotes=4;
    glutInit(&argc, argv);
    
    alutInit(NULL, 0);
	alGetError();

	// Load the wav data.

	if(LoadALData() == AL_FALSE)
	{
	    printf("Error loading data.");
	}

	SetListenerValues();
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 400);
    glutCreateWindow("DevilOnWings");
    glClearColor(0.0, 0.0, 0.0, 1.0);
    alSourcePlay(Source[0]);
    glLineWidth(3);
    glutKeyboardFunc(process_keys);
  	glutSpecialFunc(special_keys);
      	
    glutDisplayFunc(myDisplayFunction);
    glutReshapeFunc(reshape);
    glutIdleFunc(timeTick);
    glutMainLoop();
    return 0;
}

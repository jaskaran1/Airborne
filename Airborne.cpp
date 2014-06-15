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
#include <cmath>
#include <ctime>
#include "ifs.h"
#define UINTSIZE sizeof(unsigned int)
#define FLOATSIZE sizeof(float)
#define INF 1e8
using namespace std;
IFS_DATA* MODEL,*MODEL2;//global.Make changes to this struct
double planex=0.,planey=0.,spherez=-20.,universez=2.,asteroidthetaz=0.;//spin of the ball
double planeminx=INF,planemaxx=-INF,planeminy=INF,planemaxy=-INF,planeminz=INF,planemaxz=-INF,planethetaz=0.0,planethetax=0.0;
//double speed=0.;
int s=0;//score

//*****************************//
//*********GAME CODE***********//
//*****************************//
ssize_t read_uint32(int infd, unsigned int* uint_star) {
    if (read(infd, uint_star, UINTSIZE) == UINTSIZE) {
	return UINTSIZE;
    } else {
	fprintf(stderr, "Error reading size of a uint32 field\n");
	exit(-1);
    }
}

ssize_t read_float32(int infd, float* float_star) {
    if (read(infd, float_star, FLOATSIZE) == FLOATSIZE) {
        return FLOATSIZE;
    } else {
        fprintf(stderr, "Error reading size of a float32 field\n");
        exit(-1);
    }
}

ssize_t read_string32 (int infd, char** buf) {
    unsigned int mem_len = 0;
    read_uint32(infd, &mem_len);
    void *ptr;
    ptr= realloc(*buf, mem_len);
    *buf=(char*)ptr;
    if (mem_len == read(infd, *buf, mem_len)) {
	return mem_len;
    }
    else {
	fprintf(stderr, "Error reading a string32 field\n");
	exit(-1);
    }
}

IFS_DATA* load_ifs_file (const char* filename) {
    int infd;
    IFS_DATA* ifs_data = NULL;
    float version;
    char* ifstag = NULL;
    unsigned int i;
    unsigned int nVertices = 0;
    unsigned int nTriangles = 0;
    unsigned int tmp_Index = 0;
    
    if ((infd = open(filename, O_RDONLY)) < 2) {
	fprintf(stderr, "Error opening an input IFS file\n");
	exit(-1);
    }

    ifs_data = (IFS_DATA*) malloc(sizeof(IFS_DATA));
    ifs_data->modelName = NULL;
    ifs_data->numVertices = 0;
    ifs_data->vertices = NULL;
    ifs_data->numTriangles = 0;
    ifs_data->triangles = NULL;
    
    read_string32(infd, &ifstag);
    if (strcmp(ifstag, "IFS") != 0) {
	fprintf(stderr, "Not IFS filetype\n");
	exit(-1);
    }
    free(ifstag); ifstag = NULL;

    read_float32(infd, &version);
    if (version != 1.0) {
	fprintf(stderr, "Invalid version number: %f\n", version);
	exit(-1);
    }

    read_string32(infd, &(ifs_data->modelName));
    
    read_string32(infd, &ifstag);
    if (strcmp(ifstag, "VERTICES") != 0) {
	fprintf(stderr, "Not IFS filetype\n");
	exit(-1);
    }
    free(ifstag); ifstag = NULL;

    read_uint32(infd, &nVertices);
    ifs_data->numVertices = nVertices;

    ifs_data->vertices = (VERTEX*) malloc(nVertices * sizeof(VERTEX));
    for (i =0; i < ifs_data->numVertices; ++i) {
	ifs_data->vertices[i].id = i;
	read_float32(infd, &((ifs_data->vertices)[i].x));
	read_float32(infd, &((ifs_data->vertices)[i].y));
	read_float32(infd, &((ifs_data->vertices)[i].z));
    }
    
    read_string32(infd, &ifstag);
    if (strcmp(ifstag, "TRIANGLES") != 0) {
	fprintf(stderr, "Not IFS filetype\n");
	exit(-1);
    }
    free(ifstag); ifstag = NULL;
	
    read_uint32(infd, &nTriangles);
    ifs_data->numTriangles = nTriangles;
    
    ifs_data->triangles = (TRIANGLE*) malloc(nTriangles * sizeof(TRIANGLE));
    for (i =0; i < ifs_data->numTriangles; ++i) {
	read_uint32(infd, &tmp_Index);
	if (tmp_Index >= nVertices) {
	    fprintf(stderr, "Invalid Vertex index\n");
	    exit(-1);
	}
	ifs_data->triangles[i].a = &((ifs_data->vertices)[tmp_Index]);
	read_uint32(infd, &tmp_Index);
	if (tmp_Index >= nVertices) {
	    fprintf(stderr, "Invalid Vertex index\n");
	    exit(-1);
	}
	ifs_data->triangles[i].b = &((ifs_data->vertices)[tmp_Index]);
	read_uint32(infd, &tmp_Index);
	if (tmp_Index >= nVertices) {
	    fprintf(stderr, "Invalid Vertex index\n");
	    exit(-1);
	}
	ifs_data->triangles[i].c = &((ifs_data->vertices)[tmp_Index]);
    }

    if (close(infd) == -1) {
	fprintf(stderr, "Error closing an input IFS file\n");
	exit(-1);
    }
	
    return ifs_data;
}

void free_ifs_data(IFS_DATA** ifs_data) {
    if (ifs_data) {
	free((*ifs_data)->modelName);
	free((*ifs_data)->vertices);
	free((*ifs_data)->triangles);
    }
    free(*ifs_data);
    *ifs_data = NULL;
}

void print_ifs_summary(FILE* target, IFS_DATA* ifs_data) {
    unsigned int i;
    fprintf(target, "=====  IFS  SUMMARY  =====\n");
    fprintf(target, " Model name          : %s\n", ifs_data->modelName);
    fprintf(target, " Number of vertices  : %d\n", ifs_data->numVertices);
    for (i=0; i<ifs_data->numVertices; ++i) {
	fprintf(target, " v_%06d : (%8f, %8f, %8f)\n",
		ifs_data->vertices[i].id,
		ifs_data->vertices[i].x, 
		ifs_data->vertices[i].y,
		ifs_data->vertices[i].z);
    }
    fprintf(target, " Number of triangles : %d\n", ifs_data->numTriangles);
    for (i=0; i<ifs_data->numTriangles; ++i) {
	fprintf(target, " t_%06d : (v_%06d, v_%06d, v_%06d)\n", i,
		(ifs_data->triangles[i].a)->id, 
		(ifs_data->triangles[i].b)->id,
		(ifs_data->triangles[i].c)->id);
    }
    fprintf(target, "===== END OF SUMMARY =====\n");
}
//-------------------------------------------------Vector class-----------------
class Vector4{
public:
double dim[4];

Vector4(){
for(int i=0;i<4;i++)dim[i]=0;
}

Vector4(float a,float b,float c)
{dim[0]=a;
 dim[1]=b;
 dim[2]=c;
 dim[3]=1;}

void homogenize(){
 if(dim[3]!=1)dim[0]/=dim[3],dim[1]/=dim[3],dim[2]/=dim[3],dim[3]=1;
 }
 
Vector4& operator= (const Vector4& param)
{ for(int i=0;i<4;i++)
 	dim[i]=param.dim[i];
  return *this;
}
void print()
{cout<<dim[0]<<" "<<dim[1]<<" "<<dim[2]<<" "<<dim[3]<<endl;}

};
//---------------------------------------------------------Matrix Class-------------------------------
class Matrix4{
public:
float a[4][4];
Matrix4(){//initialized as identity matrix
for(int i=0;i<4;i++)
	{for(int j=0;j<4;j++)
		{	a[i][j]=0;	
		}
	}
	}
void LoadIdentity()
{for(int i=0;i<4;i++)
	{for(int j=0;j<4;j++)
		{if(i==j)
			a[i][j]=1;
		 else
		 	a[i][j]=0;	
		}
	}
}
void transpose()
{
 for(int i=0;i<4;i++)
 	{for(int j=i+1;j<4;j++)
 		{float temp=a[i][j];
 		 a[i][j]=a[j][i];
 		 a[j][i]=temp;}
 	}
}

Matrix4 operator *(double& scalar)
{Matrix4 res;
 for(int i=0;i<4;i++)
 	{for(int j=0;j<4;j++)
 		res.a[i][j]=scalar*a[i][j];
 	}
 return res;
}

Vector4 operator *(Vector4& vec)//Matrix4*Vector4
{Vector4 res;
 for(int i=0;i<4;i++)
 	{res.dim[i]=0;
 	 for(int j=0;j<4;j++)
 		res.dim[i]+=a[i][j]*vec.dim[j];
 	}
 res.homogenize();
 return res;}

Matrix4 operator *(Matrix4& param)//Matrix*Matrix
{Matrix4 res;
 for(int i=0;i<4;i++)//rows
 {for(int j=0;j<4;j++)
 	{
 	 for(int k=0;k<4;k++)
 	 	res.a[i][j]+=a[i][k]*param.a[k][j];
 	}
 }
return res;}

Matrix4& operator =(const Matrix4& param)   
{for(int i=0;i<4;i++)
 	{for(int j=0;j<4;j++)
 		a[i][j]=param.a[i][j];
 		}
return *this;
}

void print()
{for(int i=0;i<4;i++)
	{for(int j=0;j<4;j++)
		printf("%f ",a[i][j]);
 	 printf("\n");}
}

};//----------------------------------------------------------------------------------------------------
void init (void) 
{   glClearColor (0.0, 0.0, 0.0, 0.0);
    //glEnable(GL_DEPTH_TEST);
    }

Vector4 GetEyeSpaceCoord(double modelview[],double x,double y,double z)
{Vector4 WorldCoord(x,y,z);
 Vector4 EyeCoord;
 Matrix4 Transform;
 for(int j=0;j<4;j++)
 {for(int i=0;i<4;i++)
 	Transform.a[j][i]=modelview[i*4+j];//(i,j)
 	//printf("%lf ",ptr[i*4+j]);
 // printf("\n");	
 }
 EyeCoord=Transform*WorldCoord;
 return EyeCoord;
}
void output(float x,float y,char text[])
{glPushMatrix();
 glTranslatef(x,y,0);
 glScalef(1/152.38,1/152.38,1/152.38);
 for(char* p=text;*p!='\0';p++)
 {
 	glutStrokeCharacter(GLUT_STROKE_ROMAN,*p);
 }
 glPopMatrix();
 //glFlush();
} 

bool ComputeCollision(Vector4 obstaclemin,Vector4 obstaclemax,Vector4 playermin,Vector4 playermax)
{//printf("Hello\n");
static int i=0;
if(!(obstaclemax.dim[0]<playermin.dim[0]||obstaclemin.dim[0]>playermax.dim[0]
  ||obstaclemax.dim[1]<playermin.dim[1]||obstaclemin.dim[1]>playermax.dim[1]
  ||obstaclemax.dim[2]<playermin.dim[2]||obstaclemin.dim[2]>playermax.dim[2]))
  {
  printf("Collision detected %d\n",i);
  
  /*pid_t pid;
  pid=fork();
  if(pid==0)//Child process is execution of explosion sound track
  	{int check=execl("/usr/bin/vlc","vlc","explosion.wav",NULL);
  	 if(check==-1)
  	 printf("Command not found\n");
  	 sleep(15);
  	}
  if(pid>0)//parent process	
   {wait();
    exit(EXIT_SUCCESS);
   }*///Uncomment it to see the fork effect.
   return true;
   //sleep(30);
   //exit(EXIT_SUCCESS);
   i++;
   
  }
  return false;
 
}
bool draw_scene()//returns if collision was there or not
{glColor3f(1.0,0.0,0.0);//For making the game tough we can rotate the incoming stones.
 double transformmatrix[16];//used to obtain screen coordinates from world coordinates
 //-------Central Sphere--------------
 glPushMatrix();
 glTranslated(0,0,spherez);
 glGetDoublev(GL_MODELVIEW_MATRIX,transformmatrix);//Multiplying transformmatrix with the world coordinates gives me the camera coordinates 
 glutSolidSphere(.2, 10, 8);
 Vector4 asteroidmin=GetEyeSpaceCoord(transformmatrix,-.2,-.2,-.2);
 Vector4 asteroidmax=GetEyeSpaceCoord(transformmatrix,.2,.2,.2);
 glPopMatrix();
 //-------Rest of the spheres---------
 glColor3f(0,0,1);//blue colored
 glPushMatrix();
 glRotated(asteroidthetaz,0,0,1);
 glTranslated(0,1,spherez);
 glGetDoublev(GL_MODELVIEW_MATRIX,transformmatrix);//Multiplying transformmatrix with the world coordinates gives me the camera coordinates 
 glutSolidSphere(0.2, 10, 8);
 Vector4 asteroidmin1=GetEyeSpaceCoord(transformmatrix,-.2,-.2,-.2);
 Vector4 asteroidmax1=GetEyeSpaceCoord(transformmatrix,.2,.2,.2);
 glPopMatrix();
 
 glPushMatrix();
 glRotated(asteroidthetaz,0,0,1);
 glTranslated(1,0,spherez);
 glGetDoublev(GL_MODELVIEW_MATRIX,transformmatrix);//Multiplying transformmatrix with the world coordinates gives me the camera coordinates 
 glutSolidSphere(0.2, 10, 8);
 Vector4 asteroidmin2=GetEyeSpaceCoord(transformmatrix,-.2,-.2,-.2);
 Vector4 asteroidmax2=GetEyeSpaceCoord(transformmatrix,.2,.2,.2);
 glPopMatrix();
 
 glPushMatrix();
 glRotated(asteroidthetaz,0,0,1);
 glTranslated(-1,0,spherez);
 glGetDoublev(GL_MODELVIEW_MATRIX,transformmatrix);//Multiplying transformmatrix with the world coordinates gives me the camera coordinates 
 glutSolidSphere(0.2, 10, 8);
 Vector4 asteroidmin3=GetEyeSpaceCoord(transformmatrix,-.2,-.2,-.2);
 Vector4 asteroidmax3=GetEyeSpaceCoord(transformmatrix,.2,.2,.2);
 glPopMatrix();
 
 glPushMatrix();
 glRotated(asteroidthetaz,0,0,1);
 glTranslated(0,-1,spherez);
 glGetDoublev(GL_MODELVIEW_MATRIX,transformmatrix);//Multiplying transformmatrix with the world coordinates gives me the camera coordinates 
 glutSolidSphere(0.2, 10, 8);
 Vector4 asteroidmin4=GetEyeSpaceCoord(transformmatrix,-.2,-.2,-.2);
 Vector4 asteroidmax4=GetEyeSpaceCoord(transformmatrix,.2,.2,.2); 
 glPopMatrix();
 //---------Universe-------------------
 glPushMatrix();//For universe
 glColor3f(1,1,1);
 glTranslated(0,.5,universez);
 //glRotated(universethetaz,0,0,1);
 glBegin(GL_TRIANGLES);
 for(int i=0;i<MODEL2->numTriangles;i++){
 TRIANGLE tri=MODEL2->triangles[i];
 	Vector4 A(tri.a->x,tri.a->y,tri.a->z);
 	Vector4 B(tri.b->x,tri.b->y,tri.b->z);
 	Vector4 C(tri.c->x,tri.c->y,tri.c->z);
 	glVertex3f(A.dim[0],A.dim[1],A.dim[2]);
 	glVertex3f(B.dim[0],B.dim[1],B.dim[2]);
 	glVertex3f(C.dim[0],C.dim[1],C.dim[2]);
        }
 glEnd();
 glPopMatrix();
  /*float light_position[]={0,0,5,0};//place the light at eye position.Camera is stationary here.
 glLightfv(GL_LIGHT0, GL_POSITION, light_position);
 glEnable(GL_LIGHTING);
 glEnable(GL_LIGHT0);*/
 //---------Airplane----------------------
 glPushMatrix();
 glTranslated(planex,planey,0);
 glRotated(planethetaz,0,0,1);
 glRotated(planethetax,1,0,0);
 glRotated(180.,0,1,0);
 glGetDoublev(GL_MODELVIEW_MATRIX,transformmatrix); 
 glColor3f(1.0,1.0,0.0);  
 glBegin(GL_TRIANGLES);
 for(int i=0;i<MODEL->numTriangles;i++)
 	{ 	TRIANGLE tri=MODEL->triangles[i];
 		Vector4 A(tri.a->x,tri.a->y,tri.a->z);
 		Vector4 B(tri.b->x,tri.b->y,tri.b->z);
 		Vector4 C(tri.c->x,tri.c->y,tri.c->z);
 		glVertex3f(A.dim[0],A.dim[1],A.dim[2]);
 		glVertex3f(B.dim[0],B.dim[1],B.dim[2]);
 		glVertex3f(C.dim[0],C.dim[1],C.dim[2]);
 	}
 //double minplanex,minplaney,minplanez,maxplanex,maxplaney,maxplanez;
 planeminx=INF,planemaxx=-INF,planeminy=INF,planemaxy=-INF,planeminz=INF,planemaxz=-INF;
 
 for(int i=0;i<MODEL->numVertices;i++)
 	{VERTEX point=MODEL->vertices[i];
 	 Vector4 pointeyespace=GetEyeSpaceCoord(transformmatrix,point.x,point.y,point.z);
 	 if(planeminx>pointeyespace.dim[0])
 	 	planeminx=pointeyespace.dim[0];
 	 if(planeminy>pointeyespace.dim[1])
 	 	planeminy=pointeyespace.dim[1];
 	 if(planeminz>pointeyespace.dim[2])
 	 	planeminz=pointeyespace.dim[2];
 	 if(planemaxx<pointeyespace.dim[0])
 	 	planemaxx=pointeyespace.dim[0];	
 	 if(planemaxy<pointeyespace.dim[1])
 	 	planemaxy=pointeyespace.dim[1];
 	 if(planemaxz<pointeyespace.dim[2])
 	 	planemaxz=pointeyespace.dim[2];		
 	
 	}
 glEnd();
 glPopMatrix();
 //----Compute Collision between central asteroid and F16
 Vector4 planemin(planeminx,planeminy,planeminz);//For more accuracy since most of the airplane is last half
 Vector4 planemax(planemaxx,planemaxy,planemaxz);
 bool collide=ComputeCollision(asteroidmin,asteroidmax,planemin,planemax);
 bool collide1=ComputeCollision(asteroidmin1,asteroidmax1,planemin,planemax);
 bool collide2=ComputeCollision(asteroidmin2,asteroidmax2,planemin,planemax);
 bool collide3=ComputeCollision(asteroidmin3,asteroidmax3,planemin,planemax);
 bool collide4=ComputeCollision(asteroidmin4,asteroidmax4,planemin,planemax);
 //glutSwapBuffers();
 
 return collide||collide1||collide2||collide3||collide4;
 glFlush();
 }
void draw_final(char score[])
{  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   char mess1[]="GAME OVER!!";
   char mess2[]="Your Score-";
   output(-3,0,mess1);
   output(-3,-2,mess2);
   output(6,-2,score);
   glFlush();
   sleep(5);
   exit(EXIT_SUCCESS);
}
void display(void)
{ //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); 
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   gluPerspective(40,4./3.,.5,15);//PerspectiveProjection matrix.Never set znear to zero.Defines the view volume
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(0,1,4,0,0,0,0,1,0);//setup camera 
   bool collide=draw_scene();
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(-10*4/3.,10*4/3.,-10.,10.,-1,1);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   char score[20];
   sprintf(score,"%d",s);
   //glColor3f(1,1,0);
   char player[]="Jaskaran";
   char message[]="Score-";
   char credits[]="Created by Jaskaran,Parmeet and Navneet";
  output(7,9,player);
  output(-13,9,message);
  output(-10,9,score);
  output(-13,-9.5,credits);
  if(collide==1)
  {//draw_final();
   //glutIdleFunc(draw_final);
   draw_final(score);
   }
   
   glFlush();
   s++;
 }

void reshape(int w,int h)
{ glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
 
}
void surroundings(void)
{
   spherez = spherez + .01;
   universez=universez+.00125;
   asteroidthetaz+=.099;//angle by which universe rotates
   if (spherez > 5.0)
      spherez = -20.0;
   if(universez>3.0)
   	universez=2.0;  
   if(asteroidthetaz>360.)
   	asteroidthetaz-=360;	 
   glutPostRedisplay();
}
//----------Defined the Airplane Motion-------------------------
void neutraliseleft(void)//bringback to mean position uniformly
{if(planethetaz>0)
 	{planethetaz-=.5;
 	 surroundings();}
 else
 glutIdleFunc(surroundings);
 
}
void rotateleft(void)
{if(planethetax!=0)
	planethetax=0;
 if(planethetaz<30)
 	{planethetaz+=.5; 
 	surroundings(); 
	 }
 if(planethetaz>=30)//-30 angle achieved.Start translation
 	{planex-=.2;
 	 glutIdleFunc(neutraliseleft);
 	 }
 	
}
void neutraliseright(void)
{if(planethetaz<0)
	{planethetaz+=.5;
	 surroundings();
	}
 else
 	glutIdleFunc(surroundings);	
}
void rotateright(void)
{if(planethetax!=0)
	planethetax=0;
 if(planethetaz>-30)
	{planethetaz-=.5;
	 surroundings();}
 if(planethetaz<=-30)
 	{planex+=.2;
 	 glutIdleFunc(neutraliseright);
 	}	
}
void neutraliseup(void)
{if(planethetax>0)
	{planethetax-=.5;
	 surroundings();
	}
 else
 	glutIdleFunc(surroundings);	
	}
void rotateup(void)
{if(planethetaz!=0)
	planethetaz=0;
 if(planethetax<20)
	{planethetax+=.5;
	 surroundings();}
 if(planethetax>=20)
 	{planey+=.2;
 	glutIdleFunc(neutraliseup);
 	}	 
}
void neutralisedown()
{if(planethetax<0)
	{planethetax+=.5;
	surroundings();}
 else
 	glutIdleFunc(surroundings);	
}
void rotatedown(void)
{if(planethetaz!=0)
	planethetaz=0;
 if(planethetax>-20)
	{planethetax-=.5;
	 surroundings();
	}
 if(planethetax<=-20)
 	{planey-=.2;
 	 glutIdleFunc(neutralisedown);
 	}	
}
//-------------------------------------------------
void specialkeys(int key,int x,int y)
{switch(key)
{case GLUT_KEY_LEFT:
 glutIdleFunc(rotateleft);
 break;
 case GLUT_KEY_RIGHT:
 glutIdleFunc(rotateright);
 break;
 case GLUT_KEY_UP:
 //planey+=.2;
 glutIdleFunc(rotateup);
 break;
 case GLUT_KEY_DOWN:
 //planey-=.2;
 glutIdleFunc(rotatedown);
 break;
 }
 glutPostRedisplay();//calls redisplay immediately
} 
   

int main(int argc,char** argv)
{

MODEL=load_ifs_file(argv[1]);//Load Plane Model
 //MODEL3=load_ifs_file(argv[3]);//The object model which will behave as  obstacles.Right now spheres
 MODEL2=load_ifs_file(argv[2]);//Load Universe
    //FILE* f1;
    //f1=fopen("F16summary.txt","w");
    //print_ifs_summary(f1, MODEL);
    glutInit(&argc, argv);
    
    glutInitDisplayMode (GLUT_DEPTH|GLUT_SINGLE | GLUT_RGB);
    
    glutInitWindowSize (800,600); 
    
    glutInitWindowPosition (100,100);
    
    glutCreateWindow (argv[0]);
    
    init ();
    glutDisplayFunc(display);
    //glutReshapeFunc(reshape);
    glutIdleFunc(surroundings);
    //glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialkeys);
    glutMainLoop(); 
 return 0;
}

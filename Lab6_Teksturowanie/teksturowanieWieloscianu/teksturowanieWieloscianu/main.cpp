#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <GLUT/glut.h>
#include <OpenGL/OpenGL.h>

#define N 100
#define NF ((float)N)
#define M_POINTS 1
#define M_MESH   2
#define M_FILLED 3

float abs_f(float);

float abs_f(float x){
   return  x > 0 ? x : -x;
}

using namespace std;

typedef float p3[3];

float tab[N][N][3];
float color[N][N][3];
float normal[N][N][3];
float light_pos[2][4] = {
    {0.0, 0.0, 0.0, 0.0},
    {0.0, 0.0, 0.0, 0.0}
};
bool spin = false;
bool show[5] = {true, true, true, true, true};
bool zad1 = true;
bool tri = true;

int model = M_FILLED;
static GLfloat theta[] = {0.0, 0.0, 0.0};
static GLfloat beta[2][3] = {{7, 0.0, 0.0}, {-3.5, 0.0, 0.0}};
static GLfloat r=0.01;
static GLfloat pix2angle;
static GLfloat status = 0;
static int x_pos_old = 0;
static int delta_x = 0;
static int y_pos_old = 0;
static int delta_y = 0;
static GLfloat zoom_pos_old = 0.0;
static GLfloat delta_zoom = 0.0;
static GLfloat viewer[] = {0.0, 0.0, 10.0};
static int yp = 1;

void draw();
void reshape(GLsizei, GLsizei);
void keys(unsigned char, int, int);
void mouse(int btn, int state, int x, int y);
void motion(GLsizei x, GLsizei y);
void init();
void axes();
void calc();
void egg();
void fillRandomColors();
void spinEgg();
GLbyte *LoadTGAImage(const char *FileName, GLint *ImWidth, GLint *ImHeight, GLint *ImComponents, GLenum *ImFormat);
void piramid();

int main (int argc, char ** argv){
    calc();
    fillRandomColors();
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Laboratorium nr 6");
    
    glutDisplayFunc(draw);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keys);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutIdleFunc(spinEgg);
    
    //fillRandomColors();
    init();
    //calc();
    
    glutMainLoop();
    
    return 0;
}

void axes(){
    p3 x_min = {-5.0, 0.0, 0.0};
    p3 x_max = { 5.0, 0.0, 0.0};
    
    p3 y_min = {0.0, -5.0, 0.0};
    p3 y_max = {0.0,  5.0, 0.0};
    
    p3 z_min = {0.0, 0.0, -5.0};
    p3 z_max = {0.0, 0.0,  5.0};
    
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex3fv(x_min);
    glVertex3fv(x_max);
    glEnd();
    
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_LINES);
    glVertex3fv(y_min);
    glVertex3fv(y_max);
    glEnd();
    
    glColor3f(0.0, 0.0, 1.0);
    glBegin(GL_LINES);
    glVertex3fv(z_min);
    glVertex3fv(z_max);
    glEnd();
    
    

}

void fillRandomColors(){
    for(int i=0; i<N; i++){
        for(int j=0; j<N; j++){
            for(int k=0; k<3; k++){
                color[i][j][k] = (rand() % 101)/100.0f;
            }
        }
    }
    
    for(int i=0; i<N; i++){
        for(int k=0; k<3; k++){
            color[i][N-1][k] = color[N-i-1][0][k];
        }
    }
}

void spinEgg(){
    if(spin){
        theta[0] += 0.05;
        if(theta[0] > 360.0) theta[0] -= 360.0;
        
        theta[1] += 0.15;
        if(theta[1] > 360.0) theta[1] -= 360.0;
        
        theta[2] += 0.05;
        if(theta[2] > 360.0) theta[2] -= 360.0;
        
        glutPostRedisplay();
    }
}

void calc(){
    float u = 0.0, v=0.0;
    
	for(int i=0; i< N; i++){
        u = ((float)i)/(N-1);
		for(int j=0; j<N; j++){
            v = ((float)j)/(N-1);
			tab[i][j][0] = (-90*pow(u, 5.0) + 225*pow(u, 4.0) - 270*pow(u, 3.0) + 180*pow(u, 2.0) - 45*u) * cos(3.14 * v) ;
			tab[i][j][1] =  160*pow(u, 4.0) - 320*pow(u, 3.0) + 160*pow(u, 2.0) - 5.0;
			tab[i][j][2] = (-90*pow(u, 5.0) + 225*pow(u, 4.0) - 270*pow(u, 3.0) + 180*pow(u, 2.0) - 45*u) * sin(3.14 * v);
            
            GLfloat xu = (-450*pow(u,4) + 900*pow(u,3) - 810*pow(u,2) + 360*u - 45)*cos(3.14 * v);
            GLfloat yu = 640*pow(u,3) - 960*pow(u,2) + 320*u;
            GLfloat zu = (-450*pow(u,4) + 900*pow(u,3) - 810*pow(u,2) + 360*u - 45)*sin(3.14 * v);
            
            GLfloat xv = 3.14 * (90*pow(u,5) - 225*pow(u,4) + 270*pow(u,3) - 180*pow(u,2) + 45*u)*sin(3.14 * v);
            GLfloat yv = 0;
            GLfloat zv = -3.14 * (90*pow(u,5) - 225*pow(u,4) + 270*pow(u,3) - 180*pow(u,2) + 45*u)*cos(3.14 * v);
            
            normal[i][j][0] = yu*zv - zu*yv;
            normal[i][j][1] = zu*xv - xu*zv;
            normal[i][j][2] = xu*yv - yu*xv;
            
            GLfloat len = sqrt(pow(normal[i][j][0],2) + pow(normal[i][j][1],2) + pow(normal[i][j][2],2));	
            normal[i][j][0] /= len;
            normal[i][j][1] /= len;
            normal[i][j][2] /= len;
            
            if(i >= N/2) {
                normal[i][j][0] *= -1;
                normal[i][j][1] *= -1;
                normal[i][j][2] *= -1;
            }
        }
        
	}
    

}

void egg(){
    // draw
    switch(model){
        case M_POINTS:
            glColor3f(1.0, 1.0, 1.0);
            glBegin(GL_POINTS);
            
            for(int i=0; i<N; i++){
                for(int j=0; j<N; j++){
                    glVertex3fv(tab[i][j]);
                }
            }
            
            glEnd();
            break;
            
        case M_MESH:
            glColor3f(1.0, 1.0, 1.0);
            glBegin(GL_LINES);
                        
            for(int i=0; i<N-1; i++){
                for(int j=0; j<N-1; j++){
                    glVertex3fv(tab[i][j]);
                    glVertex3fv(tab[i+1][j]);
                    
                    glVertex3fv(tab[i][j]);
                    glVertex3fv(tab[i][j+1]);
                }
            }
    
            glEnd();
            
            
            break;
            
        case M_FILLED:
            glColor3f(1.0, 1.0, 1.0);
            glBegin(GL_TRIANGLES);
            
            for(int i=0; i<N/2; i++){
                for(int j=0; j<N-1; j++){
//                    glColor3fv(color[i][j]);
                    glNormal3fv(normal[i][j]);
                    glTexCoord2f(i/NF, j/NF);
                    glVertex3fv(tab[i][j]);
                    
//                    glColor3fv(color[i+1][j]);
                    glNormal3fv(normal[i+1][j]);
                    glTexCoord2f((i+1)/NF, j/NF);
                    glVertex3fv(tab[i+1][j]);
                    
//                    glColor3fv(color[i][j+1]);
                    glNormal3fv(normal[i][j+1]);
                    glTexCoord2f(i/NF, (j+1)/NF);
                    glVertex3fv(tab[i][j+1]);
                    
//                    glColor3fv(color[i][j+1]);
                    glNormal3fv(normal[i][j+1]);
                    glTexCoord2f(i/NF, (j+1)/NF);
                    glVertex3fv(tab[i][j+1]);
                    
//                    glColor3fv(color[i+1][j+1]);
                    glNormal3fv(normal[i+1][j+1]);
                    glTexCoord2f((i+1)/NF, (j+1)/NF);
                    glVertex3fv(tab[i+1][j+1]);
                    
//                    glColor3fv(color[i+1][j]);
                    glNormal3fv(normal[i+1][j]);
                    glTexCoord2f((i+1)/NF, j/NF);
                    glVertex3fv(tab[i+1][j]);
                }
            }
            
            for(int i=N-1; i>N/2; i--){
                for(int j=0; j<N-1; j++){
//                    glColor3fv(color[i][j]);
                    glNormal3fv(normal[i][j]);
                    glTexCoord2f((N-i-1)/NF, (N-j)/NF);
                    glVertex3fv(tab[i][j]);
                    
//                    glColor3fv(color[i-1][j]);
                    glNormal3fv(normal[i-1][j]);
                    glTexCoord2f((N-i+1-1)/NF, (N-j)/NF);
                    glVertex3fv(tab[i-1][j]);
                    
//                    glColor3fv(color[i][j+1]);
                    glNormal3fv(normal[i][j+1]);
                    glTexCoord2f((N-i-1)/NF, (N-j-1)/NF);
                    glVertex3fv(tab[i][j+1]);
                    
//                    glColor3fv(color[i][j+1]);
                    glNormal3fv(normal[i][j+1]);
                    glTexCoord2f((N-i-1)/NF, (N-j-1)/NF);
                    glVertex3fv(tab[i][j+1]);
                    
//                    glColor3fv(color[i-1][j+1]);
                    glNormal3fv(normal[i-1][j+1]);
                    glTexCoord2f((N-i+1-1)/NF, (N-j-1)/NF);
                    glVertex3fv(tab[i-1][j+1]);
                    
//                    glColor3fv(color[i-1][j]);
                    glNormal3fv(normal[i-1][j]);
                    glTexCoord2f((N-i+1-1)/NF, (N-j)/NF);
                    glVertex3fv(tab[i-1][j]);
                }
            }
            
            glEnd();
            break;
    }
    
}

void init(){
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//    
//    GLfloat mat_ambient[]  = {1.0, 1.0, 1.0, 1.0};        
//    GLfloat mat_diffuse[]  = {0.8, 0.8, 0.8, 1.0};    
//    GLfloat mat_specular[] = {0.8, 0.8, 0.8, 1.0};    
//    GLfloat mat_shininess  = {50.0};
//      
//    GLfloat light0_ambient[] = {0.0, 0.0, 0.0, 1.0};
//    GLfloat light0_diffuse[] = {1.0, 0.5, 0.2, 1.0};        
//    GLfloat light0_specular[]= {1.0, 0.5, 0.2, 1.0};
//
//    GLfloat light1_ambient[] = {0.0, 0.0, 0.0, 1.0};
//    GLfloat light1_diffuse[] = {1.0, 1.0, 0.0, 1.0};        
//    GLfloat light1_specular[]= {1.0, 1.0, 0.0, 1.0};
    
    
    GLfloat mat_ambient[]  = {1.0, 1.0, 1.0, 1.0};        
    GLfloat mat_diffuse[]  = {1.0, 1.0, 1.0, 1.0};    
    GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};    
    GLfloat mat_shininess  = {50.0};
    
    GLfloat light0_ambient[] = {0.0, 0.0, 0.0, 1.0};
    GLfloat light0_diffuse[] = {1.0, 1.0, 1.0, 1.0};        
    GLfloat light0_specular[]= {1.0, 1.0, 1.0, 1.0};
    
    GLfloat light1_ambient[] = {0.0, 0.0, 0.0, 1.0};
    GLfloat light1_diffuse[] = {1.0, 1.0, 1.0, 1.0};        
    GLfloat light1_specular[]= {1.0, 1.0, 1.0, 1.0};

    GLfloat att_constant  = {1.0};
    GLfloat att_linear    = {0.05}; 
    GLfloat att_quadratic  = {0.001};
    
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos[0]);
    
    glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
    glLightfv(GL_LIGHT1, GL_POSITION, light_pos[1]);
    
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);
    
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, att_constant);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, att_linear);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, att_quadratic);
    
    
    glShadeModel(GL_SMOOTH); // właczenie łagodnego cieniowania
//    glEnable(GL_LIGHTING);   // właczenie systemu oświetlenia sceny 
    glEnable(GL_LIGHT0);     // włączenie źródła o numerze 0
    glEnable(GL_LIGHT1);     // włączenie źródła o numerze 1
    glEnable(GL_DEPTH_TEST); // włączenie mechanizmu z-bufora     

    
    GLbyte *pBytes;
    GLint ImWidth, ImHeight, ImComponents;
    GLenum ImFormat;
    
//    glEnable(GL_CULL_FACE);
   
    
    //    pBytes = LoadTGAImage("/Users/teamon/Downloads/tekstury/P4_t.tga", &ImWidth, &ImHeight, &ImComponents, &ImFormat);
    pBytes = LoadTGAImage("/Users/teamon/Downloads/t_1024.tga", &ImWidth, &ImHeight, &ImComponents, &ImFormat);
    glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);
    free(pBytes);
    
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void mouse(int btn, int state, int x, int y){
    if(btn == GLUT_LEFT_BUTTON){
        if(state == GLUT_DOWN){
            x_pos_old = x;
            y_pos_old = y;
            status = 1;
        } else {
            status = 0;
        }
    } else if(btn == GLUT_RIGHT_BUTTON){
        if(state == GLUT_DOWN){
            zoom_pos_old = y;
            status = 2;
        } else {
            status = 0;
        }
    }
}

void motion(GLsizei x, GLsizei y){
    delta_x = x - x_pos_old;
    x_pos_old = x;
    
    delta_y = y - y_pos_old;
    y_pos_old = y;
    
    delta_zoom = y - zoom_pos_old;
    zoom_pos_old = y;
    
    glutPostRedisplay();
}

void reshape(GLsizei horizontal, GLsizei vertical){
    pix2angle = 360.0/(float)horizontal;
    
    glViewport(0, 0, horizontal, vertical);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
        
    gluPerspective(70, 1.0, 1.0, 30.0);
    
    if(horizontal <= vertical) {
        glViewport(0, (vertical-horizontal)/2, horizontal, horizontal);
    } else {
        glViewport((horizontal-vertical)/2, 0, vertical, vertical);
    }
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void keys(unsigned char key, int x, int y){
    if(key >= '0' && key <= '5'){
        show[key - '1'] = !show[key - '1'];
    } else {
        switch(key){
            case 'p':
                model = M_POINTS;
                break;
                
            case 'm':
                model = M_MESH;
                break;
                
            case 'f':
                model = M_FILLED;
                break;
                
            case 's':
                spin = !spin;
                break;
                
            case 'z':
                zad1 = !zad1;
                break;
                
            case 't':
                tri = !tri;
                break;
        }
    }
    
    draw();
}

void draw(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    
    
    if(zad1){
        if(status == 1){
            theta[1] += delta_x*pix2angle;
            theta[0] += delta_y*pix2angle;
        } else if(status == 2){
            viewer[2] += delta_zoom/10.0;
            if(viewer[2] < 0) viewer[2] = 0;
        }
    } else {
        if(status == 1){
            beta[0][0] += delta_x/40.0;
            beta[0][1] += delta_y/40.0;
        } else if(status == 2){
            beta[1][0] += delta_x/40.0;
            beta[1][1] += delta_y/40.0;
        }
        
        for(int b=0; b<2; b++){
            if(beta[b][1] >= M_PI) beta[b][1] -= 2*M_PI;
            else if(beta[b][1] <= -M_PI) beta[b][1] += 2*M_PI;
            
            if(beta[b][1] > M_PI/2 || beta[b][1] < -M_PI/2) yp = -1;
            else yp = 1;
            
            light_pos[b][0] = r*cos(beta[b][0])*cos(beta[b][1]);
            light_pos[b][1] = r*sin(beta[b][1]);
            light_pos[b][2] = r*sin(beta[b][0])*cos(beta[b][1]);
            //cout << "b=" << b << " => " << beta[b][0] << "," << beta[b][1] << "," << beta[b][2] << endl;

        }
        
        glLightfv(GL_LIGHT0, GL_POSITION, light_pos[0]);
        glLightfv(GL_LIGHT1, GL_POSITION, light_pos[1]);

    }
    
    gluLookAt(viewer[0], viewer[1], viewer[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    axes();
    
    //if(zad1){
        glRotatef(theta[0], 1.0, 0.0, 0.0);
        glRotatef(theta[1], 0.0, 1.0, 0.0);
        glRotatef(theta[2], 0.0, 0.0, 1.0);
    //}
    
    glColor3f(1.0, 1.0, 1.0);
    
//    if(!zad1){
//        egg();
//        //glutWireTeapot(3.0);
//        //glutSolidTeapot(3.0);
//    } else {
//        egg();
//        //glutWireTeapot(3.0);
//
//    }
    
//    piramid();
    egg();
    
    
    glFlush();
    glutSwapBuffers();
}

void piramid(){
    if(show[0]){
        glBegin(GL_QUADS);
        glTexCoord2f(0.0, 1.0);
        glVertex3f(-3.0,  3.0, 0.0); // D
        glTexCoord2f(1.0, 1.0);
        glVertex3f( 3.0,  3.0, 0.0); // C
        glTexCoord2f(1.0, 0.0);
        glVertex3f( 3.0, -3.0, 0.0); // B
        glTexCoord2f(0.0, 0.0);
        glVertex3f(-3.0, -3.0, 0.0); // A
        glEnd();
    }
    
    glBegin(GL_TRIANGLES);
    
    if(show[1]){
        glTexCoord2f(0.0, 0.0);
        glVertex3f(-3.0, -3.0, 0.0); // A
        glTexCoord2f(1.0, 0.0);
        glVertex3f( 3.0, -3.0, 0.0); // B
        glTexCoord2f(0.5, 0.5);
        glVertex3f( 0.0,  0.0, 5.0);
    }
    
    if(show[2]){
        glTexCoord2f(1.0, 0.0);
        glVertex3f( 3.0, -3.0, 0.0); // B
        glTexCoord2f(1.0, 1.0);
        glVertex3f( 3.0,  3.0, 0.0); // C
        glTexCoord2f(0.5, 0.5);
        glVertex3f( 0.0,  0.0, 5.0);
    }
    
    if(show[3]){
        glTexCoord2f(1.0, 1.0);
        glVertex3f( 3.0,  3.0, 0.0); // C
        glTexCoord2f(0.0, 1.0);
        glVertex3f(-3.0,  3.0, 0.0); // D
        glTexCoord2f(0.5, 0.5);
        glVertex3f( 0.0,  0.0, 5.0);
    }
    
    if(show[4]){
        glTexCoord2f(0.0, 1.0);
        glVertex3f(-3.0,  3.0, 0.0); // D
        glTexCoord2f(0.0, 0.0);
        glVertex3f(-3.0, -3.0, 0.0); // A
        glTexCoord2f(0.5, 0.5);
        glVertex3f( 0.0,  0.0, 5.0);
    }
    
    glEnd();

}


GLbyte *LoadTGAImage(const char *FileName, GLint *ImWidth, GLint *ImHeight, GLint *ImComponents, GLenum *ImFormat){
    typedef struct {
        GLbyte    idlength;              
        GLbyte    colormaptype;           
        GLbyte    datatypecode;             
        unsigned short    colormapstart;  
        unsigned short    colormaplength; 
        //unsigned char     colormapdepth;   
        unsigned short    x_orgin;         
        unsigned short    y_orgin;         
        unsigned short    width;          
        unsigned short    height;         
        GLbyte    bitsperpixel;                   
        GLbyte    descriptor;             
    } TGAHEADER;
    
    
    FILE *pFile;                    
    TGAHEADER tgaHeader;            
    unsigned long lImageSize;        
    short sDepth;                    
    GLbyte *pbitsperpixel = NULL; 
    
    *ImWidth = 0;                
    *ImHeight = 0;
    *ImFormat = GL_BGR_EXT;
    *ImComponents = GL_RGB8;
    
    pFile = fopen(FileName, "rb");
    if(pFile == NULL)
        return NULL;
        
    fread(&tgaHeader, sizeof(TGAHEADER), 1, pFile);
    
    *ImWidth = tgaHeader.width;
    *ImHeight = tgaHeader.height;
    sDepth = tgaHeader.bitsperpixel / 8;

    
    if(tgaHeader.bitsperpixel != 8 && tgaHeader.bitsperpixel != 24 && tgaHeader.bitsperpixel != 32)
        return NULL;
    
    lImageSize = tgaHeader.width * tgaHeader.height * sDepth;
    pbitsperpixel = (GLbyte*)malloc(lImageSize * sizeof(GLbyte));
    
    if(pbitsperpixel == NULL)
        return NULL;
    
    if(fread(pbitsperpixel, lImageSize, 1, pFile) != 1){
        free(pbitsperpixel);
        return NULL;
    }
    
    switch(sDepth)
    {
        case 3:    
            *ImFormat = GL_BGR_EXT;
            *ImComponents = GL_RGB8;
            break;
        case 4:
            *ImFormat = GL_BGRA_EXT;
            *ImComponents = GL_RGBA8;
            break;
        case 1:
            *ImFormat = GL_LUMINANCE;
            *ImComponents = GL_LUMINANCE8;
            break;
    };
    
    fclose(pFile);
    
    return pbitsperpixel;
}


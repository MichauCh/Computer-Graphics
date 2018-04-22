#define _CRT_SECURE_NO_DEPRECATE 
/* Jajko 3-D + obrót + œwiat³o + tekstura */
#include <windows.h> 
#include <gl/gl.h> 
#include <gl/glut.h> 
#include <math.h> 
#include <time.h>
#include <cstdio> 
static GLfloat viewer[] = { 0.0, 0.0, 10.0 };
static GLfloat fi = 0.0, theta = 0.0;// k¹ty obrotu, elewacja i azymut 
static GLfloat pix2angle_x = 0.0, pix2angle_y = 0.0; // przelicznik pikseli na stopnie 
static GLint status = 0; // stan klawiszy myszy 
static int x_pos_old = 0, y_pos_old = 0; // poprzednia pozycja kursora myszy 
static int delta_x = 0, delta_y = 0;
float verLength = 1.0; //Dlugosc boku kwadratu 
float viewerR = 10.0; //Promien sfery obserwatora 
static GLfloat angle[] = { 0.0, 0.0, 0.0 };
bool sciany[5] = { true, true, true, true, true };
typedef float point3[3];
const float PI = 3.14159265;
typedef float point2[2]; //Tablica na wspolrzedne tekstury 

GLbyte *LoadTGAImage(const char *FileName, GLint *ImWidth, GLint *ImHeight, GLint *ImComponents, GLenum *ImFormat) {
	// Struktura dla nag³ówka pliku TGA 
#pragma pack(1) 
	typedef struct {
		GLbyte idlength;
		GLbyte colormaptype;
		GLbyte datatypecode;
		unsigned short colormapstart;
		unsigned short colormaplength;
		unsigned char colormapdepth;
		unsigned short x_orgin;
		unsigned short y_orgin;
		unsigned short width;
		unsigned short height;
		GLbyte bitsperpixel;
		GLbyte descriptor;
	}
	TGAHEADER;
#pragma pack(8) 
	FILE *pFile;
	TGAHEADER tgaHeader;
	unsigned long lImageSize;
	short sDepth;
	GLbyte *pbitsperpixel = NULL; // Wartoœci domyœlne zwracane w przypadku b³êdu 
	*ImWidth = 0;
	*ImHeight = 0;
	*ImFormat = GL_BGR_EXT;
	*ImComponents = GL_RGB8;
	pFile = fopen(FileName, "rb");
	if (pFile == NULL)
		return NULL; // Przeczytanie nag³ówka pliku 
	fread(&tgaHeader, sizeof(TGAHEADER), 1, pFile); // Odczytanie szerokoœci, wysokoœci i g³êbi obrazu 
	*ImWidth = tgaHeader.width;
	*ImHeight = tgaHeader.height;
	sDepth = tgaHeader.bitsperpixel / 8; // Sprawdzenie, czy g³êbia spe³nia za³o¿one warunki (8, 24, lub 32 bity) 
	if (tgaHeader.bitsperpixel != 8 && tgaHeader.bitsperpixel != 24 && tgaHeader.bitsperpixel != 32)
		return NULL;
	// Obliczenie rozmiaru bufora w pamiêci
	lImageSize = tgaHeader.width * tgaHeader.height * sDepth; // Alokacja pamiêci dla danych obrazu 
	pbitsperpixel = (GLbyte*)malloc(lImageSize * sizeof(GLbyte));
	if (pbitsperpixel == NULL)
		return NULL;
	if (fread(pbitsperpixel, lImageSize, 1, pFile) != 1) {
		free(pbitsperpixel);
		return NULL;
	}
	// Ustawienie formatu OpenGL 
	switch (sDepth) {
	case 3: *ImFormat = GL_BGR_EXT;
		*ImComponents = GL_RGB8;
		break;
	case 4: *ImFormat = GL_BGRA_EXT;
		*ImComponents = GL_RGBA8;
		break;
	case 1: *ImFormat = GL_LUMINANCE;
		*ImComponents = GL_LUMINANCE8;
		break;
	};
	fclose(pFile);
	return pbitsperpixel;
}

void Pyramid() {
	glColor3f(1.0, 1.0, 1.0);
	if (sciany[0]) {
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 1.0);
		glVertex3f(-3.0, 3.0, 0.0); // D
		glTexCoord2f(1.0, 1.0);
		glVertex3f(3.0, 3.0, 0.0); // C
		glTexCoord2f(1.0, 0.0);
		glVertex3f(3.0, -3.0, 0.0); // B
		glTexCoord2f(0.0, 0.0);
		glVertex3f(-3.0, -3.0, 0.0); // A
		glEnd();
	}
	glBegin(GL_TRIANGLES);
	if (sciany[1]) {
		glTexCoord2f(0.0, 0.0);
		glVertex3f(-3.0, -3.0, 0.0); // A
		glTexCoord2f(1.0, 0.0);
		glVertex3f(3.0, -3.0, 0.0); // B
		glTexCoord2f(0.5, 0.5);
		glVertex3f(0.0, 0.0, 5.0);
	}
	if (sciany[2]) {
		glTexCoord2f(1.0, 0.0);
		glVertex3f(3.0, -3.0, 0.0); // B
		glTexCoord2f(1.0, 1.0);
		glVertex3f(3.0, 3.0, 0.0); // C
		glTexCoord2f(0.5, 0.5);
		glVertex3f(0.0, 0.0, 5.0);
	}
	if (sciany[3]) {
		glTexCoord2f(1.0, 1.0);
		glVertex3f(3.0, 3.0, 0.0); // C
		glTexCoord2f(0.0, 1.0);
		glVertex3f(-3.0, 3.0, 0.0); // D
		glTexCoord2f(0.5, 0.5);
		glVertex3f(0.0, 0.0, 5.0);
	}
	if (sciany[4]) {
		glTexCoord2f(0.0, 1.0);
		glVertex3f(-3.0, 3.0, 0.0); // D
		glTexCoord2f(0.0, 0.0);
		glVertex3f(-3.0, -3.0, 0.0); // A
		glTexCoord2f(0.5, 0.5);
		glVertex3f(0.0, 0.0, 5.0);
	}
	glEnd();
}
//Funkcja callback dla obrotu 
void spinPiramid() {
	angle[0] -= 0.5;
	if (angle[0] > 360.0)
		angle[0] -= 360.0;
	angle[1] -= 0.5;
	if (angle[1] > 360.0)
		angle[1] -= 360.0;
	angle[2] -= 0.5;
	if (angle[2] > 360.0)
		angle[2] -= 360.0;
	glutPostRedisplay(); //odœwie¿enie zawartoœci aktualnego okna
}
// Funkcja rysuj¹ca osie uk³adu wspó³rzêdnych 
void Axes(void) {
	point3 x_min = { -2.0, 0.0, 0.0 };
	point3 x_max = { 2.0, 0.0, 0.0 };
	// pocz¹tek i koniec obrazu osi x 
	point3 y_min = { 0.0, -2.0, 0.0 };
	point3 y_max = { 0.0, 2.0, 0.0 };
	// pocz¹tek i koniec obrazu osi y 
	point3 z_min = { 0.0, 0.0, -2.0 };
	point3 z_max = { 0.0, 0.0, 2.0 }; // pocz¹tek i koniec obrazu osi y 
	glColor3f(1.0f, 0.0f, 0.0f); // kolor rysowania osi - czerwony 
	glBegin(GL_LINES); // rysowanie osi x 
	glVertex3fv(x_min);
	glVertex3fv(x_max);
	glEnd();
	glColor3f(0.0f, 1.0f, 0.0f); // kolor rysowania - zielony 
	glBegin(GL_LINES); // rysowanie osi y
	glVertex3fv(y_min);
	glVertex3fv(y_max);
	glEnd();
	glColor3f(0.0f, 0.0f, 1.0f); // kolor rysowania - niebieski 
	glBegin(GL_LINES); // rysowanie osi z 
	glVertex3fv(z_min);
	glVertex3fv(z_max);
	glEnd();
}
// Funkcja okreœlaj¹ca co ma byæ rysowane (zawsze wywo³ywana gdy trzeba przerysowaæ scenê) 
void RenderScene(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Czyszczenie okna aktualnym kolorem czyszcz¹cym 
	glLoadIdentity(); // Czyszczenie macierzy bie¿¹cej 
	if (status == 2) {
		// jeœli prawy klawisz myszy wciœniêty 
		viewerR += 0.1* delta_y; // modyfikacja polozenia obserwatora(zoom)
		if (viewerR <= 6.0) // ograniczenie zblizenia 
			viewerR = 6.0; if (viewerR >= 25.0) // ograniczenie oddalenia 
			viewerR = 25.0;
	}
	//Wspolrzedne obserwatora - wzorki z ZSK 
	viewer[0] = viewerR * cos(theta) * cos(fi);
	viewer[1] = viewerR * sin(fi);
	viewer[2] = viewerR * sin(theta) * cos(fi);
	gluLookAt(viewer[0], viewer[1], viewer[2], 0.0, 0.0, 0.0, 0.0, cos(fi), 0.0);
	// Zdefiniowanie po³o¿enia obserwatora 
	Axes();
	// Narysowanie osi przy pomocy funkcji zdefiniowanej powy¿ej 
	//Rotacje 
	glRotatef(angle[0], 1.0, 0.0, 0.0);
	glRotatef(angle[1], 0.0, 1.0, 0.0);
	glRotatef(angle[2], 0.0, 0.0, 1.0);
	//Renderowanie piramidy 
	Pyramid();
	glFlush(); // Przekazanie poleceñ rysuj¹cych do wykonania 
	glutSwapBuffers();
}
// Funkcja ustalaj¹ca stan renderowania
void MyInit(void) {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Kolor czyszc¹cy (wype³nienia okna) ustawiono na czarny
	// Zmienne dla obrazu tekstury 
	GLbyte *pBytes;
	GLint ImWidth, ImHeight, ImComponents;
	GLenum ImFormat;
	// Definicja materia³u z jakiego zrobiony jest przedmiot
	//------------------------------------------------------- 
	GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
	// wspó³czynniki ka =[kar,kag,kab] dla œwiat³a otoczenia 
	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	// wspó³czynniki kd =[kdr,kdg,kdb] œwiat³a rozproszonego
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	// wspó³czynniki ks =[ksr,ksg,ksb] dla œwiat³a odbitego 
	GLfloat mat_shininess = { 100.0 };
	// wspó³czynnik n opisuj¹cy po³ysk powierzchni 
	// Definicja Ÿród³a œwiat³a
	//-------------------------------------------------------
	GLfloat light_position[]{ 5.0, 5.0, 10.0, 1.0 };
	// po³o¿enie Ÿród³a 
	GLfloat light_ambient[]{ 0.1, 0.1, 0.1, 1.0 };
	// sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a otoczenia 
	// Ia = [Iar,Iag,Iab] 
	GLfloat light_diffuse[]{ 1.0, 1.0, 1.0, 1.0 };
	// sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a powoduj¹cego 
	// odbicie dyfuzyjne Id = [Idr,Idg,Idb] 
	GLfloat light_specular[]{ 1.0, 1.0, 1.0, 1.0 };
	// sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a powoduj¹cego 
	// odbicie kierunkowe Is = [Isr,Isg,Isb] 
	GLfloat att_constant = 1.0;
	// sk³adowa sta³a ds dla modelu zmian oœwietlenia w funkcji 
	// odleg³oœci od Ÿród³a 
	GLfloat att_linear = 0.05;
	// sk³adowa liniowa dl dla modelu zmian oœwietlenia w funkcji 
	// odleg³oœci od Ÿród³a
	GLfloat att_quadratic = 0.001;
	// sk³adowa kwadratowa dq dla modelu zmian oœwietlenia w funkcji 
	// odleg³oœci od Ÿród³a 
	// Ustawienie patrametrów materia³u 
	//------------------------------------------------------- 
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);
	// Ustawienie parametrów Ÿród³a œwiat³a
	//------------------------------------------------------- 
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);
	// Ustawienie opcji systemu oœwietlania sceny 
	//------------------------------------------------------- 
	glShadeModel(GL_SMOOTH);
	// w³aczenie ³agodnego cieniowania 
	glEnable(GL_LIGHTING);
	// w³aczenie systemu oœwietlenia sceny
	glEnable(GL_LIGHT0);
	// w³¹czenie Ÿród³a o numerze 0 
	glEnable(GL_DEPTH_TEST);
	// w³¹czenie mechanizmu z-bufora
	glEnable(GL_CULL_FACE);

	pBytes = LoadTGAImage("tekstura1.tga", &ImWidth, &ImHeight, &ImComponents, &ImFormat);
	// Zdefiniowanie tekstury 2-D
	glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);
	// Zwolnienie pamiêci 
	free(pBytes);
	// W³¹czenie mechanizmu teksturowania
	glEnable(GL_TEXTURE_2D);
	// Ustalenie trybu teksturowania 
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	// Okreœlenie sposobu nak³adania tekstur 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
// Funkcja ma za zadanie utrzymanie sta³ych proporcji rysowanych 
// w przypadku zmiany rozmiarów okna. 
// Parametry vertical i horizontal (wysokoœæ i szerokoœæ okna) s¹ 
// przekazywane do funkcji za ka¿dym razem gdy zmieni siê rozmiar okna. 
void ChangeSize(GLsizei horizontal, GLsizei vertical) {
	pix2angle_x = 360.0*0.1 / (float)horizontal;
	// przeliczenie pikseli na stopnie 
	pix2angle_y = 360.0*0.1 / (float)vertical;
	glMatrixMode(GL_PROJECTION);
	// Prze³¹czenie macierzy bie¿¹cej na macierz projekcji 
	glLoadIdentity();
	// Czyszcznie macierzy bie¿¹cej 
	GLfloat AspectRatio = static_cast<float> (horizontal) / vertical;
	gluPerspective(70.0, AspectRatio, 1.0, 30.0);
	// Ustawienie parametrów dla rzutu perspektywicznego 
	if (horizontal <= vertical)
		glViewport(0, 0, horizontal, vertical);
	else
		glViewport(0, 0, horizontal, vertical);
	// Ustawienie wielkoœci okna okna widoku (viewport) w zale¿noœci
	// relacji pomiêdzy wysokoœci¹ i szerokoœci¹ okna 
	glMatrixMode(GL_MODELVIEW);
	// Prze³¹czenie macierzy bie¿¹cej na macierz widoku modelu 
	glLoadIdentity(); // Czyszczenie macierzy bie¿¹cej 
}
// Funkcja "bada" stan myszy i ustawia wartosci odpowiednich zmiennych globalnych 
void Mouse(int btn, int state, int x, int y) {
	if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		y_pos_old = y;
		// przypisanie aktualnie odczytanej pozycji kursora 
		// jako pozycji poprzedniej 
		status = 2; //wciœniêty zosta³ prawy klawisz myszy 
	}
	else status = 0; // nie zosta³ wciœniêty ¿aden klawisz
}
// Funkcja "monitoruje" polozenie kursora myszy i ustawia wartosci odpowiednich 
// zmiennych globalnych 
void Motion(GLsizei x, GLsizei y) {
	delta_x = x - x_pos_old;
	// obliczenie ró¿nicy po³o¿enia kursora myszy 
	x_pos_old = x;
	// podstawienie bie¿acego po³o¿enia jako poprzednie 
	delta_y = y - y_pos_old;
	// obliczenie ró¿nicy po³o¿enia kursora myszy 
	y_pos_old = y;
	// podstawienie bie¿acego po³o¿enia jako poprzednie
	glutPostRedisplay();
	// przerysowanie obrazu sceny 
}
void Keys(unsigned char key, int x, int y)
{
	if (key == '1') sciany[0] = !sciany[0];
	if (key == '2') sciany[1] = !sciany[1];
	if (key == '3') sciany[2] = !sciany[2];
	if (key == '4') sciany[3] = !sciany[3];
	if (key == '5') sciany[4] = !sciany[4];
}
// G³ówny punkt wejœcia programu. Program dzia³a w trybie konsoli 
void main(void) {
	int foo = 1; char * bar[1] = { " " };
	glutInit(&foo, bar);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(300, 300);
	glutCreateWindow("Micha³ Ostros³up");
	glutDisplayFunc(RenderScene);
	// Okreœlenie, ¿e funkcja RenderScene bêdzie funkcj¹ zwrotn¹ // (callback function). Bedzie ona wywo³ywana za ka¿dym razem
	// gdy zajdzie potrzba przeryswania okna 
	// Dla aktualnego okna ustala funkcjê zwrotn¹ odpowiedzialn¹ 
	// zazmiany rozmiaru okna 
	glutReshapeFunc(ChangeSize);
	MyInit();
	// Funkcja MyInit() (zdefiniowana powy¿ej) wykonuje wszelkie 
	// inicjalizacje konieczne przed przyst¹pieniem do renderowania
	// W³¹czenie mechanizmu usuwania powierzchni niewidocznych 
	glutMouseFunc(Mouse);
	// Ustala funkcjê zwrotn¹ odpowiedzialn¹ za badanie stanu myszy 
	glutMotionFunc(Motion);
	// Ustala funkcjê zwrotn¹ odpowiedzialn¹ za badanie ruchu myszy 
	//Rejestracja funkcji zwrotnej (obrot)
	glutKeyboardFunc(Keys);
	//Rejestracja funkcji zwrotnej (obrot) 
	glutIdleFunc(spinPiramid);
	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT
} 
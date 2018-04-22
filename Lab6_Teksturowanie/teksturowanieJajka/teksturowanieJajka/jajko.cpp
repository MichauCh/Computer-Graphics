#define _CRT_SECURE_NO_DEPRECATE 
/* Jajko 3-D + obr�t + �wiat�o + tekstura */ 
#include <windows.h> 
#include <gl/gl.h> 
#include <gl/glut.h> 
#include <math.h> 
#include <time.h>
#include <cstdio> 
static GLfloat viewer[] = { 0.0, 0.0, 10.0 }; 
static GLfloat fi = 0.0, theta = 0.0;// k�ty obrotu, elewacja i azymut 
static GLfloat pix2angle_x = 0.0, pix2angle_y = 0.0; // przelicznik pikseli na stopnie 
static GLint status = 0; // stan klawiszy myszy 
static int x_pos_old = 0, y_pos_old = 0; // poprzednia pozycja kursora myszy 
static int delta_x = 0, delta_y = 0; 
const int N = 50; //Liczba punktow na jaka dzielimy kwadrat jednostkowy 
float verLength = 1.0; //Dlugosc boku kwadratu 
float viewerR = 10.0; //Promien sfery obserwatora 
static GLfloat angle[] = { 0.0, 0.0, 0.0 }; 
typedef float point3[3]; 
const float PI = 3.14159265; 
point3 **pointsTab; 
point3 **pointsNorms; 
typedef float point2[2]; //Tablica na wspolrzedne tekstury 
point2 **pointsTex; 

GLbyte *LoadTGAImage(const char *FileName, GLint *ImWidth, GLint *ImHeight, GLint *ImComponents, GLenum *ImFormat){ 
// Struktura dla nag��wka pliku TGA 
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
	GLbyte *pbitsperpixel = NULL; // Warto�ci domy�lne zwracane w przypadku b��du 
	*ImWidth = 0; 
	*ImHeight = 0; 
	*ImFormat = GL_BGR_EXT; 
	*ImComponents = GL_RGB8; 
	pFile = fopen(FileName, "rb"); 
	if (pFile == NULL) 
		return NULL; // Przeczytanie nag��wka pliku 
	fread(&tgaHeader, sizeof(TGAHEADER), 1, pFile); // Odczytanie szeroko�ci, wysoko�ci i g��bi obrazu 
	*ImWidth = tgaHeader.width; 
	*ImHeight = tgaHeader.height; 
	sDepth = tgaHeader.bitsperpixel / 8; // Sprawdzenie, czy g��bia spe�nia za�o�one warunki (8, 24, lub 32 bity) 
	if (tgaHeader.bitsperpixel != 8 && tgaHeader.bitsperpixel != 24 && tgaHeader.bitsperpixel != 32)
		return NULL; 
// Obliczenie rozmiaru bufora w pami�ci
	lImageSize = tgaHeader.width * tgaHeader.height * sDepth; // Alokacja pami�ci dla danych obrazu 
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
//Funkcja wyliczajaca wspolrzedna X punktu (u,v) w przestrzeni 3D 
float calc3Dx(float u, float v) {
	float x, a = v*PI;
	x = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * cos(a); 
	return x; 
}
//Funkcja wyliczajaca wspolrzedna Y punktu (u,v) w przestrzeni 3D 
float calc3Dy(float u, float v) { 
	float y; y = 160 * pow(u, 4) - 320 * pow(u, 3) + 160 * pow(u, 2); 
	return y - 5; 
} 
//Funkcja wyliczajaca wspolrzedna Z punktu (u,v) w przestrzeni 3D 
float calc3Dz(float u, float v) { 
	float z, a = v*PI; 
	z = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * sin(a); 
	return z; 
}
//Obliczenie wspolrzednej X wektora normalnego do powierzchni w punkcie 
float calcNormX(float u, float v) { 
	float x, a = v*PI; 
	float yu = 640 * pow(u, 3) - 960 * pow(u, 2) + 320 * u; 
	float yv = 0; 
	float zu = (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45)*sin(a); 
	float zv = -PI*(90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u)*cos(a); 
	x = (GLfloat)(yu*zv - zu*yv); 
	return x;
}
//Obliczenie wspolrzednej Y wektora normalnego do powierzchni w punkcie 
float calcNormY(float u, float v) {
	float y, a = v*PI; 
	float xu = (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45)*cos(a);
	float xv = PI*(90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u)*sin(a);
	float zu = (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45)*sin(a); 
	float zv = -PI*(90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u)*cos(a);
	y = (GLfloat)(zu*xv - xu*zv);
	return y;
}
//Obliczenie wspolrzednej Z wektora normalnego do powierzchni w punkcie 
float calcNormZ(float u, float v) {
	float z, a = v*PI; 
	float xu = (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45)*cos(a);
	float xv = PI*(90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u)*sin(a); 
	float yu = 640 * pow(u, 3) - 960 * pow(u, 2) + 320 * u; float yv = 0; 
	z = (GLfloat)(xu*yv - yu*xv); 
	return z; 
} 
//Funkcja generujaca siatke puntow, najpierw w 2D, potem w 3D 
void genPointsMesh() { 
	float stepXY = verLength / N; //Przypisanie punktom wspolrzednych 
	for (int i = 0; i<N + 1; i++) { 
		for (int j = 0; j<N + 1; j++) { 
			pointsTab[i][j][0] = j*stepXY; 
			pointsTab[i][j][1] = i*stepXY; } } 
//Przeksztalcenie wspolrzednych z dziedziny parametrycznej w przestrzen 3D 
	float u, v; 
	for (int i = 0; i<N + 1; i++) { 
		for (int j = 0; j<N + 1; j++) { 
			v = pointsTab[i][j][0];
			u = pointsTab[i][j][1]; //Zapisanie wspolrzedych tekstury 
			pointsTex[i][j][0] = v; 
			pointsTex[i][j][1] = u; 
			pointsTab[i][j][0] = calc3Dx(u, v);
			pointsTab[i][j][1] = calc3Dy(u, v);
			pointsTab[i][j][2] = calc3Dz(u, v); //Wyliczenie wspolrzednych wektorow normalnych do powierzchni jajka 
			float x = calcNormX(u, v); 
			float y = calcNormY(u, v); 
			float z = calcNormZ(u, v);
			//Normalizacja wektorow normalnych do powierzchni jajka 
			//Wektory na bokach jajka 
			if (i < N / 2) { 
				pointsNorms[i][j][0] = x / (float)sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2)); 
				pointsNorms[i][j][1] = y / (float)sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2)); 
				pointsNorms[i][j][2] = z / (float)sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
			} 
			if (i > N / 2) { 
				pointsNorms[i][j][0] = -1.0*x / (float)sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
				pointsNorms[i][j][1] = -1.0*y / (float)sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
				pointsNorms[i][j][2] = -1.0*z / (float)sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2)); 
			} 
			//Wektory na "szczycie" jajka
			if (i == N / 2) { 
				pointsNorms[i][j][0] = 0; 
				pointsNorms[i][j][1] = 1; 
				pointsNorms[i][j][2] = 0;
			} 
			//Wektory na "dnie" jajka 
			if (i == 0 || i == N) { 
				pointsNorms[i][j][0] = 0;
				pointsNorms[i][j][1] = -1; 
				pointsNorms[i][j][2] = 0; 
			}
		} 
	} 
} 
//Funkcja renderujaca okreslony model jajka
	void Egg(void) { 
		//Wygenerowanie siatki 3D punktow 
		genPointsMesh(); 
		//Parametry rysowania 
		glColor3f(1.0, 1.0, 1.0); 
		//Rysowanie jajka - trojkaty 
		for (int i = 0; i < N; i++) { 
			for (int j = 0; j < N; j++) {
				//W jedna strone 
				glBegin(GL_TRIANGLES);
				glNormal3fv(pointsNorms[i][j + 1]);
				glTexCoord2fv(pointsTex[i][j + 1]);
				glVertex3fv(pointsTab[i][j + 1]); 
				glNormal3fv(pointsNorms[i + 1][j]); 
				glTexCoord2fv(pointsTex[i + 1][j]); 
				glVertex3fv(pointsTab[i + 1][j]);
				glNormal3fv(pointsNorms[i + 1][j + 1]); 
				glTexCoord2fv(pointsTex[i + 1][j + 1]); 
				glVertex3fv(pointsTab[i + 1][j + 1]); 
				glEnd(); //W druga strone 
				glBegin(GL_TRIANGLES); 
				glNormal3fv(pointsNorms[i][j]); 
				glTexCoord2fv(pointsTex[i][j]);
				glVertex3fv(pointsTab[i][j]);
				glNormal3fv(pointsNorms[i + 1][j]);
				glTexCoord2fv(pointsTex[i + 1][j]); 
				glVertex3fv(pointsTab[i + 1][j]);
				glNormal3fv(pointsNorms[i][j + 1]);
				glTexCoord2fv(pointsTex[i][j + 1]); 
				glVertex3fv(pointsTab[i][j + 1]);
				glEnd(); 
			} 
		} 
	} 
// Funkcja rysuj�ca osie uk�adu wsp�rz�dnych 
	void Axes(void) { 
		point3 x_min = { -2.0, 0.0, 0.0 }; 
		point3 x_max = { 2.0, 0.0, 0.0 }; 
		// pocz�tek i koniec obrazu osi x 
		point3 y_min = { 0.0, -2.0, 0.0 };
		point3 y_max = { 0.0, 2.0, 0.0 }; 
		// pocz�tek i koniec obrazu osi y 
		point3 z_min = { 0.0, 0.0, -2.0 }; 
		point3 z_max = { 0.0, 0.0, 2.0 }; // pocz�tek i koniec obrazu osi y 
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
// Funkcja okre�laj�ca co ma by� rysowane (zawsze wywo�ywana gdy trzeba przerysowa� scen�) 
	void RenderScene(void) { 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
		// Czyszczenie okna aktualnym kolorem czyszcz�cym 
		glLoadIdentity(); // Czyszczenie macierzy bie��cej 
		if (status == 2) { 
			// je�li prawy klawisz myszy wci�ni�ty 
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
		// Zdefiniowanie po�o�enia obserwatora 
		Axes();
		// Narysowanie osi przy pomocy funkcji zdefiniowanej powy�ej 
		//Rotacje 
		glRotatef(angle[0], 1.0, 0.0, 0.0); 
		glRotatef(angle[1], 0.0, 1.0, 0.0); 
		glRotatef(angle[2], 0.0, 0.0, 1.0); 
		//Renderowanie jajka 
		Egg();
		glFlush(); // Przekazanie polece� rysuj�cych do wykonania 
		glutSwapBuffers();
	} 
//Funkcja callback dla obrotu 
	void spinEgg() { 
		angle[0] -= 0.5; 
		if (angle[0] > 360.0) 
			angle[0] -= 360.0; 
		angle[1] -= 0.5; 
		if (angle[1] > 360.0) 
			angle[1] -= 360.0; 
		angle[2] -= 0.5; 
		if (angle[2] > 360.0) 
			angle[2] -= 360.0; 
		glutPostRedisplay(); //od�wie�enie zawarto�ci aktualnego okna
	} 
// Funkcja ustalaj�ca stan renderowania
	void MyInit(void) { 
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
		// Kolor czyszc�cy (wype�nienia okna) ustawiono na czarny
		// Zmienne dla obrazu tekstury 
		GLbyte *pBytes; 
		GLint ImWidth, ImHeight, ImComponents;
		GLenum ImFormat; 
	// Definicja materia�u z jakiego zrobiony jest przedmiot
	//------------------------------------------------------- 
		GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
		// wsp�czynniki ka =[kar,kag,kab] dla �wiat�a otoczenia 
		GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 }; 
		// wsp�czynniki kd =[kdr,kdg,kdb] �wiat�a rozproszonego
		GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 }; 
		// wsp�czynniki ks =[ksr,ksg,ksb] dla �wiat�a odbitego 
		GLfloat mat_shininess = { 100.0 }; 
		// wsp�czynnik n opisuj�cy po�ysk powierzchni 
		// Definicja �r�d�a �wiat�a
		//-------------------------------------------------------
		GLfloat light_position[]{ 5.0, 5.0, 10.0, 1.0 }; 
		// po�o�enie �r�d�a 
		GLfloat light_ambient[]{ 0.1, 0.1, 0.1, 1.0 };
		// sk�adowe intensywno�ci �wiecenia �r�d�a �wiat�a otoczenia 
		// Ia = [Iar,Iag,Iab] 
		GLfloat light_diffuse[]{ 1.0, 1.0, 1.0, 1.0 }; 
		// sk�adowe intensywno�ci �wiecenia �r�d�a �wiat�a powoduj�cego 
		// odbicie dyfuzyjne Id = [Idr,Idg,Idb] 
		GLfloat light_specular[]{ 1.0, 1.0, 1.0, 1.0 };
		// sk�adowe intensywno�ci �wiecenia �r�d�a �wiat�a powoduj�cego 
		// odbicie kierunkowe Is = [Isr,Isg,Isb] 
		GLfloat att_constant = 1.0; 
		// sk�adowa sta�a ds dla modelu zmian o�wietlenia w funkcji 
		// odleg�o�ci od �r�d�a 
		GLfloat att_linear = 0.05; 
		// sk�adowa liniowa dl dla modelu zmian o�wietlenia w funkcji 
		// odleg�o�ci od �r�d�a
		GLfloat att_quadratic = 0.001; 
		// sk�adowa kwadratowa dq dla modelu zmian o�wietlenia w funkcji 
		// odleg�o�ci od �r�d�a 
		// Ustawienie patrametr�w materia�u 
		//------------------------------------------------------- 
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular); 
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient); 
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse); 
		glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess); 
		// Ustawienie parametr�w �r�d�a �wiat�a
		//------------------------------------------------------- 
		glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient); 
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse); 
		glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular); 
		glLightfv(GL_LIGHT0, GL_POSITION, light_position); 
		glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
		glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic); 
		// Ustawienie opcji systemu o�wietlania sceny 
		//------------------------------------------------------- 
		glShadeModel(GL_SMOOTH); 
		// w�aczenie �agodnego cieniowania 
		glEnable(GL_LIGHTING); 
		// w�aczenie systemu o�wietlenia sceny
		glEnable(GL_LIGHT0); 
		// w��czenie �r�d�a o numerze 0 
		glEnable(GL_DEPTH_TEST); 
		// w��czenie mechanizmu z-bufora
		pBytes = LoadTGAImage("tekstura1.tga", &ImWidth, &ImHeight, &ImComponents, &ImFormat); 
		// Zdefiniowanie tekstury 2-D
		glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes); 
		// Zwolnienie pami�ci 
		free(pBytes); 
		// W��czenie mechanizmu teksturowania
		glEnable(GL_TEXTURE_2D);
		// Ustalenie trybu teksturowania 
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); 
		// Okre�lenie sposobu nak�adania tekstur 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	} 
// Funkcja ma za zadanie utrzymanie sta�ych proporcji rysowanych 
// w przypadku zmiany rozmiar�w okna. 
// Parametry vertical i horizontal (wysoko�� i szeroko�� okna) s� 
// przekazywane do funkcji za ka�dym razem gdy zmieni si� rozmiar okna. 
	void ChangeSize(GLsizei horizontal, GLsizei vertical) { 
		pix2angle_x = 360.0*0.1 / (float)horizontal; 
		// przeliczenie pikseli na stopnie 
		pix2angle_y = 360.0*0.1 / (float)vertical; 
		glMatrixMode(GL_PROJECTION); 
		// Prze��czenie macierzy bie��cej na macierz projekcji 
		glLoadIdentity();
		// Czyszcznie macierzy bie��cej 
		GLfloat AspectRatio = static_cast<float> (horizontal) / vertical;
		gluPerspective(70.0, AspectRatio, 1.0, 30.0); 
		// Ustawienie parametr�w dla rzutu perspektywicznego 
		if (horizontal <= vertical)
			glViewport(0, 0, horizontal, vertical);
		else
			glViewport(0, 0, horizontal, vertical);
		// Ustawienie wielko�ci okna okna widoku (viewport) w zale�no�ci
		// relacji pomi�dzy wysoko�ci� i szeroko�ci� okna 
		glMatrixMode(GL_MODELVIEW); 
		// Prze��czenie macierzy bie��cej na macierz widoku modelu 
		glLoadIdentity(); // Czyszczenie macierzy bie��cej 
	} 
	// Funkcja "bada" stan myszy i ustawia wartosci odpowiednich zmiennych globalnych 
	void Mouse(int btn, int state, int x, int y) { 
		if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
			y_pos_old = y; 
			// przypisanie aktualnie odczytanej pozycji kursora 
			// jako pozycji poprzedniej 
			status = 2; //wci�ni�ty zosta� prawy klawisz myszy 
		} else status = 0; // nie zosta� wci�ni�ty �aden klawisz
	} 
// Funkcja "monitoruje" polozenie kursora myszy i ustawia wartosci odpowiednich 
// zmiennych globalnych 
	void Motion(GLsizei x, GLsizei y) {
		delta_x = x - x_pos_old;
		// obliczenie r�nicy po�o�enia kursora myszy 
		x_pos_old = x; 
		// podstawienie bie�acego po�o�enia jako poprzednie 
		delta_y = y - y_pos_old; 
		// obliczenie r�nicy po�o�enia kursora myszy 
		y_pos_old = y;
		// podstawienie bie�acego po�o�enia jako poprzednie
		glutPostRedisplay(); 
		// przerysowanie obrazu sceny 
	} 
// G��wny punkt wej�cia programu. Program dzia�a w trybie konsoli 
	void main(void) {
		//Ziarno losowosci 
		srand((unsigned)time(NULL)); 
		//Dynamiczna alokacja tablicy punktow 
		pointsTab = new point3*[N + 1]; 
		for (int i = 0; i<N + 1; i++) { 
			pointsTab[i] = new point3[N + 1];
		}
		//Dynamiczna alokacja tablicy i wygenerowanie kolorow losowych dla punktow 
		pointsNorms = new point3*[N + 1]; 
		for (int i = 0; i < N + 1; i++) { 
			pointsNorms[i] = new point3[N + 1]; 
		}
	//Dynamiczna alokacja tablicy wsplorzednych tesktur 
		pointsTex = new point2*[N + 1]; 
		for (int i = 0; i < N + 1; i++) { 
			pointsTex[i] = new point2[N + 1]; 
		} 
		int foo = 1; char * bar[1] = { " " }; 
		glutInit(&foo, bar);
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
		glutInitWindowSize(300, 300);
		glutCreateWindow("Micha� Jajko 3D");
		glutDisplayFunc(RenderScene);
		// Okre�lenie, �e funkcja RenderScene b�dzie funkcj� zwrotn� // (callback function). Bedzie ona wywo�ywana za ka�dym razem
		// gdy zajdzie potrzba przeryswania okna 
		// Dla aktualnego okna ustala funkcj� zwrotn� odpowiedzialn� 
		// zazmiany rozmiaru okna 
		glutReshapeFunc(ChangeSize);
		MyInit();
		// Funkcja MyInit() (zdefiniowana powy�ej) wykonuje wszelkie 
		// inicjalizacje konieczne przed przyst�pieniem do renderowania
		// W��czenie mechanizmu usuwania powierzchni niewidocznych 
		glutMouseFunc(Mouse); 
		// Ustala funkcj� zwrotn� odpowiedzialn� za badanie stanu myszy 
		glutMotionFunc(Motion); 
		// Ustala funkcj� zwrotn� odpowiedzialn� za badanie ruchu myszy 
		//Rejestracja funkcji zwrotnej (obrot) 
		glutIdleFunc(spinEgg); 
		glutMainLoop(); 
		// Funkcja uruchamia szkielet biblioteki GLUT
		//Zwolnienie pami�ci
		for (int i = 0; i < N + 1; i++) { 
			delete[] pointsTab[i];
			delete[] pointsNorms[i];
			pointsTab[i] = 0;
			pointsNorms[i] = 0; 
		} 
		delete[] pointsTab;
		delete[] pointsNorms;
		pointsTab = 0; 
		pointsNorms = 0; 
	}
/* Jajko 3d z dwoma ruchomymi zrodlami swiatla */ 
#include <windows.h> 
#include <gl/gl.h> 
#include <gl/glut.h> 
#include <math.h> 
#include <time.h> 
static GLfloat fi[2] = { 5.5, 1.0 }, theta[2] = { 4.5, 4.5 };// k¹ty obrotu, elewacja i azymut 
static GLfloat pix2angle_x = 0.0, pix2angle_y = 0.0; //zmienne do przeliczania pikseli na stopnie 
static GLint status = 0; // stan myszy 
static int x_pos_old = 0, y_pos_old = 0; //poprzednie polozenie myszy 
static int delta_x = 0, delta_y = 0; // ró¿nica polozenia myszy 
const int N = 50; //Liczba punktow na jaka dzielimy kwadrat jednostkowy 
float verLength = 1.0; //Dlugosc boku kwadratu 
float R = 10.0; //Promien sfery swiatel 
typedef float point3[3]; 
const float PI = 3.14159265; 
point3 **pointsTab; 
point3 **pointsNorms; //Funkcja wyliczajaca wspolrzedna X punktu (u,v) w przestrzeni 3D 
float licz3Dx(float u, float v) { 
	float x, a = v*PI; 
	x = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * cos(a); 
	return x; 
} 
//Funkcja wyliczajaca wspolrzedna Y punktu (u,v) w przestrzeni 3D 
float licz3Dy(float u, float v) { 
	float y; y = 160 * pow(u, 4) - 320 * pow(u, 3) + 160 * pow(u, 2); 
	return y - 5; 
} 
//Funkcja wyliczajaca wspolrzedna Z punktu (u,v) w przestrzeni 3D 
float licz3Dz(float u, float v) { 
	float z, a = v*PI;
	z = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * sin(a); 
	return z;
} 
//Obliczenie wspolrzednej X wektora normalnego do powierzchni w punkcie 
float normaX(float u, float v) { 
	float x, a = v*PI; 
	float yu = 640 * pow(u, 3) - 960 * pow(u, 2) + 320 * u; 
	float yv = 0; 
	float zu = (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45)*sin(a); 
	float zv = -PI*(90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u)*cos(a); 
	x = (GLfloat)(yu*zv - zu*yv); 
	return x; 
} 
//Obliczenie wspolrzednej Y wektora normalnego do powierzchni w punkcie 
float normaY(float u, float v) { 
	float y, a = v*PI; 
	float xu = (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45)*cos(a); 
	float xv = PI*(90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u)*sin(a); 
	float zu = (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45)*sin(a); 
	float zv = -PI*(90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u)*cos(a); 
	y = (GLfloat)(zu*xv - xu*zv); 
	return y; 
} 
//Obliczenie wspolrzednej Z wektora normalnego do powierzchni w punkcie 
float normaZ(float u, float v) { 
	float z, a = v*PI; 
	float xu = (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45)*cos(a); 
	float xv = PI*(90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u)*sin(a);
	float yu = 640 * pow(u, 3) - 960 * pow(u, 2) + 320 * u; float yv = 0; 
	z = (GLfloat)(xu*yv - yu*xv); 
	return z; 
} 
//Funkcja generujaca siatke puntow, najpierw w 2D, potem w 3D 
void genPointsMesh() { 
	float stepXY = verLength / N;
	//Przypisanie punktom wspolrzednych 
	for (int i = 0; i<N + 1; i++) { 
		for (int j = 0; j<N + 1; j++) { 
			pointsTab[i][j][0] = j*stepXY; 
			pointsTab[i][j][1] = i*stepXY; 
		} 
	} 
	//Przeksztalcenie wspolrzednych z dziedziny parametrycznej w przestrzen 3D 
	float u, v; 
	for (int i = 0; i<N + 1; i++) { 
		for (int j = 0; j<N + 1; j++) { 
			v = pointsTab[i][j][0]; 
			u = pointsTab[i][j][1]; 
			pointsTab[i][j][0] = licz3Dx(u, v); 
			pointsTab[i][j][1] = licz3Dy(u, v); 
			pointsTab[i][j][2] = licz3Dz(u, v); 
	//Wyliczenie wspolrzednych wektorow normalnych do powierzchni jajka 
			float x = normaX(u, v); 
			float y = normaY(u, v); 
			float z = normaZ(u, v); 
		//Normalizacja wektorow normalnych do powierzchni jajka wektory na bokach jajka 
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
		//Wektory na gorze jajka 
			if (i == N / 2) { 
				pointsNorms[i][j][0] = 0; 
				pointsNorms[i][j][1] = 1; 
				pointsNorms[i][j][2] = 0; 
			} 
		//Wektory na dole jajka 
			if (i == 0 || i == N) { 
				pointsNorms[i][j][0] = 0;
				pointsNorms[i][j][1] = -1; pointsNorms[i][j][2] = 0;
			}
		}
	}
} 
//Funkcja renderujaca okreslony model jajka 
void Egg(void) { 
	//Wygenerowanie siatki 3D punktow 
	genPointsMesh(); //Parametry rysowania 
	glColor3f(1.0, 1.0, 1.0); //Rysowanie jajka - trojkaty 
	for (int i = 0; i < N; i++) { 
		for (int j = 0; j < N; j++) { 
			//W jedna strone 
			glBegin(GL_TRIANGLES); 
			glNormal3fv(pointsNorms[i][j + 1]); 
			glVertex3fv(pointsTab[i][j + 1]); 
			glNormal3fv(pointsNorms[i + 1][j]); 
			glVertex3fv(pointsTab[i + 1][j]); 
			glNormal3fv(pointsNorms[i + 1][j + 1]); 
			glVertex3fv(pointsTab[i + 1][j + 1]); 
			glEnd(); //W druga strone 
			glBegin(GL_TRIANGLES); 
			glNormal3fv(pointsNorms[i][j]); 
			glVertex3fv(pointsTab[i][j]); 
			glNormal3fv(pointsNorms[i + 1][j]); 
			glVertex3fv(pointsTab[i + 1][j]); 
			glNormal3fv(pointsNorms[i][j + 1]); 
			glVertex3fv(pointsTab[i][j + 1]); 
			glEnd(); 
		} 
	} 
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
	point3 z_max = { 0.0, 0.0, 2.0 };
	// pocz¹tek i koniec obrazu osi y 
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
	glBegin(GL_LINES);
	// rysowanie osi z 
	glVertex3fv(z_min);
	glVertex3fv(z_max);
	glEnd();
}
// Funkcja okreœlaj¹ca co ma byæ rysowane (zawsze wywo³ywana gdy trzeba przerysowaæ scenê) 
	void RenderScene(void) { 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
		// Czyszczenie okna aktualnym kolorem czyszcz¹cym 
		glLoadIdentity(); // Czyszczenie macierzy bie¿¹cej 
		if (status == 1) { // jeœli lewy klawisz myszy wciœniêty 
			theta[0] -= delta_x*pix2angle_x; //Ograniczenie dla azymutu 
			if (theta[0] <= 0) 
				theta[0] += 2 * PI; 
			if (theta[0] >= 2 * PI) 
				theta[0] -= 2 * PI; 
			fi[0] -= delta_y*pix2angle_y; //Ograniczenie dla elewacji 
			if (fi[0] <= 0) 
				fi[0] += 2 * PI; 
			if (fi[0] >= 2 * PI) 
				fi[0] -= 2 * PI; 
		} 
		else if (status == 2) { // jeœli prawy klawisz myszy wciœniêty
			theta[1] -= delta_x*pix2angle_x; //Ograniczenie dla azymutu 
			if (theta[1] <= 0) 
				theta[1] += 2 * PI;
			if (theta[1] >= 2 * PI) 
				theta[1] -= 2 * PI; 
			fi[1] -= delta_y*pix2angle_y; //Ograniczenie dla elewacji 
			if (fi[1] <= 0) 
				fi[1] += 2 * PI; 
			if (fi[1] >= 2 * PI) 
				fi[1] -= 2 * PI; 
		} 
		gluLookAt(0.0, 0.0, -10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); 
		// Zdefiniowanie po³o¿enia obserwatora 
		GLfloat lights_positions[4] = { 0 }; 
		lights_positions[0] = R * cos(theta[0]) * cos(fi[0]); 
		lights_positions[1] = R * sin(fi[0]); 
		lights_positions[2] = R * sin(theta[0]) * cos(fi[0]); 
		lights_positions[3] = 1.0; 
		glLightfv(GL_LIGHT0, GL_POSITION, lights_positions); 
		//Aktualizacja pozycji swiatla 0 
		lights_positions[0] = R * cos(theta[1]) * cos(fi[1]); 
		lights_positions[1] = R * sin(fi[1]); 
		lights_positions[2] = R * sin(theta[1]) * cos(fi[1]); 
		lights_positions[3] = 1.0; 
		glLightfv(GL_LIGHT1, GL_POSITION, lights_positions); 
		//Aktualizacja pozycji swiatla 1 
		Axes(); 
		// Narysowanie osi przy pomocy funkcji zdefiniowanej powy¿ej 
		//Renderowanie jajka 
		Egg(); 
		glFlush(); // Przekazanie poleceñ rysuj¹cych do wykonania 
		glutSwapBuffers(); 
	} 
// Funkcja ustalaj¹ca stan renderowania 
	void MyInit(void) { 
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Kolor czyszc¹cy (wype³nienia okna) ustawiono na czarny
		// Definicja materia³u z jakiego zrobiony jest przedmiot 
//------------------------------------------------------- 
		GLfloat mat_ambient[] = { 0.3, 0.3, 0.3, 1.0 }; 
		// wspó³czynniki ka =[kar,kag,kab] dla œwiat³a otoczenia 
		GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 }; 
		// wspó³czynniki kd =[kdr,kdg,kdb] œwiat³a rozproszonego 
		GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 }; 
		// wspó³czynniki ks =[ksr,ksg,ksb] dla œwiat³a odbitego 
		GLfloat mat_shininess = { 100.0 }; 
		// wspó³czynnik n opisuj¹cy po³ysk powierzchni 
		// Definicja Ÿród³a œwiat³a 
//------------------------------------------------------- 
		GLfloat light_position[2][4] = { { -10.0, -10.0, -10.0, 1.0 },
										 { -10.0, -10.0, -10.0, 1.0 } }; // po³o¿enie Ÿród³a 
		GLfloat light_ambient[] = { 0.2, 0.2, 0.2, 1.0 }; 
		// sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a otoczenia Ia = [Iar,Iag,Iab] 
		GLfloat light_diffuse[2][4] = { { 1.0, 0.0, 0.0, 0.0 },
										{ 0.0, 0.0, 1.0, 1.0 } }; 
		// sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a powoduj¹cego odbicie dyfuzyjne Id = [Idr,Idg,Idb]
		GLfloat light_specular[2][4] = { { 1.0, 1.0, 0.0, 1.0 },
										 { 0.7, 0.7, 1.0, 1.0 } }; // sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a powoduj¹cego // odbicie kierunkowe Is = [Isr,Isg,Isb] 
		GLfloat att_constant = { 1.0 }; // sk³adowa sta³a ds dla modelu zmian oœwietlenia w funkcji odleg³oœci od Ÿród³a 
		GLfloat att_linear = 0.001; // sk³adowa liniowa dl dla modelu zmian oœwietlenia w funkcji odleg³oœci od Ÿród³a 
		GLfloat att_quadratic = 0.001; // sk³adowa kwadratowa dq dla modelu zmian oœwietlenia w funkcji odleg³oœci od Ÿród³a 
		// Ustawienie patrametrów materia³u 
//------------------------------------------------------- 
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular); 
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient); 
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess); 
		// Ustawienie parametrów Ÿród³a œwiat³a 
//------------------------------------------------------- 
		glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient); 
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse[0]); 
		glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular[0]); 
		glLightfv(GL_LIGHT0, GL_POSITION, light_position[0]); 
		glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant); 
		glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear); 
		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic); 
		glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient); 
		glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse[1]); 
		glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular[1]); 
		glLightfv(GL_LIGHT1, GL_POSITION, light_position[1]); 
		glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, att_constant); 
		glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, att_linear); 
		glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, att_quadratic); 
		// Ustawienie opcji systemu oœwietlania sceny 
//------------------------------------------------------- 
		glShadeModel(GL_SMOOTH); // w³aczenie ³agodnego cieniowania 
		glEnable(GL_LIGHTING); // w³aczenie systemu oœwietlenia sceny 
		glEnable(GL_LIGHT0); // w³¹czenie Ÿród³a o numerze 0 
		glEnable(GL_LIGHT1); // w³¹czenie Ÿród³a o numerze 1 
		glEnable(GL_DEPTH_TEST); // w³¹czenie mechanizmu z-bufora 
	} 
// Funkcja ma za zadanie utrzymanie sta³ych proporcji rysowanych w przypadku zmiany rozmiarów okna. 
//Parametry vertical i horizontal (wysokoœæ i szerokoœæ okna) s¹ przekazywane do funkcji za ka¿dym razem gdy zmieni siê rozmiar okna. 
	void ChangeSize(GLsizei horizontal, GLsizei vertical) { 
		pix2angle_x = 360.0*0.0125 / (float)horizontal; 
		// przeliczenie pikseli na stopnie 
		pix2angle_y = 360.0*0.0125 / (float)vertical; 
		glMatrixMode(GL_PROJECTION); // Prze³¹czenie macierzy bie¿¹cej na macierz projekcji 
		glLoadIdentity(); // Czyszcznie macierzy bie¿¹cej 
		gluPerspective(70.0, 1.0, 1.0, 30.0); // Ustawienie parametrów dla rzutu perspektywicznego 
		if (horizontal <= vertical)
			glViewport(0, (vertical - horizontal) / 2, horizontal, horizontal);
		else glViewport((horizontal - vertical) / 2, 0, vertical, vertical); 
		// Ustawienie wielkoœci okna okna widoku (viewport) w zale¿noœci relacji pomiêdzy wysokoœci¹ i szerokoœci¹ okna 
		glMatrixMode(GL_MODELVIEW); // Prze³¹czenie macierzy bie¿¹cej na macierz widoku modelu 
		glLoadIdentity(); // Czyszczenie macierzy bie¿¹cej 
	} 
// Funkcja "bada" stan myszy i ustawia wartosci odpowiednich zmiennych globalnych 
	void Mouse(int btn, int state, int x, int y) { 
		if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			x_pos_old = x; // przypisanie aktualnie odczytanej pozycji kursora 
			y_pos_old = y; // jako pozycji poprzedniej 
			status = 1; 
	// wciœniêty zosta³ lewy klawisz myszy 
		} else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) { 
			x_pos_old = x; // przypisanie aktualnie odczytanej pozycji kursora 
			y_pos_old = y; // jako pozycji poprzedniej 
			status = 2; //wciœniêty zosta³ prawy klawisz myszy 
		} else status = 0; // nie zosta³ wciœniêty ¿aden klawisz
	} 
// Funkcja "monitoruje" polozenie kursora myszy i ustawia wartosci odpowiednich zmiennych globalnych 
	void Motion(GLsizei x, GLsizei y) { 
		delta_x = x - x_pos_old; // obliczenie ró¿nicy po³o¿enia kursora myszy 
		x_pos_old = x; // podstawienie bie¿acego po³o¿enia jako poprzednie 
		delta_y = y - y_pos_old; // obliczenie ró¿nicy po³o¿enia kursora myszy
		y_pos_old = y; // podstawienie bie¿acego po³o¿enia jako poprzednie 
		glutPostRedisplay(); // przerysowanie obrazu sceny 
	} // G³ówny punkt wejœcia programu. Program dzia³a w trybie konsoli 
	void main(void) { 
		//Dynamiczna alokacja tablicy punktow 
		pointsTab = new point3*[N + 1]; 
		for (int i = 0; i<N + 1; i++) {
			pointsTab[i] = new point3[N + 1];
		} 
	//Dynamiczna alokacja tablicy wspolrzednych wektorow normalnych
		pointsNorms = new point3*[N + 1]; 
		for (int i = 0; i < N + 1; i++) { 
			pointsNorms[i] = new point3[N + 1]; 
		} 
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); 
		glutInitWindowSize(300, 300); 
		glutCreateWindow("Jajko 3D z dwoma ruchomymi zrodlami swiatla"); 
		glutDisplayFunc(RenderScene); // Okreœlenie, ¿e funkcja RenderScene bêdzie funkcj¹ zwrotn¹ (callback function). 
		//Bedzie ona wywo³ywana za ka¿dym razem gdy zajdzie potrzba przeryswania okna. Dla aktualnego okna ustala funkcjê zwrotn¹ odpowiedzialn¹ zazmiany rozmiaru okna 
		glutReshapeFunc(ChangeSize); 
		MyInit(); // Funkcja MyInit() (zdefiniowana powy¿ej) wykonuje wszelkie inicjalizacje konieczne przed przyst¹pieniem do renderowania 
		// W³¹czenie mechanizmu usuwania powierzchni niewidocznych 
		glutMouseFunc(Mouse); // Ustala funkcjê zwrotn¹ odpowiedzialn¹ za badanie stanu myszy 
		glutMotionFunc(Motion); // Ustala funkcjê zwrotn¹ odpowiedzialn¹ za badanie ruchu myszy 
		glEnable(GL_DEPTH_TEST); 
		glutMainLoop(); // Funkcja uruchamia szkielet biblioteki GLUT 
		//Zwolnienie pamiêci 
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
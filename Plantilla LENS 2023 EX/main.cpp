#include <windows.h>
#include <iostream> //Libreria para entrada y salida de datos
#include <fstream> //Libreria para manejar archivos
#include <string> //Libreria para utilizar tipos de datos string
#include <cmath>
#include "Librerias/Dibujar bitmaps/gdipload.h"
#include "Librerias/Musica/libzplay.h"
#include "Librerias/Gamepad/GamePadRR.h"
GamePadRR* Gapad;


using namespace libZPlay;//Se declara que se usara la libreria de libZPlay para reproductor de musica
using namespace std; //Utiliza el espacio de nombres de la libreria STD de c++

struct Musica
{
	string Dir;
	string Nombre;
};
Musica Cancion[3];

struct FrameArray {
	int x, y;
	int ancho, alto;
};//Similar a la estructura rect de winapi

struct Input
{
	enum Keys
	{
		Backspace = 0x08, Tab,
		Clear = 0x0C, Enter,
		Shift = 0x10, Control, Alt,
		Escape = 0x1B,
		Space = 0x20, PageUp, PageDown, End, Home, Left, Up, Right, Down,
		Zero = 0x30, One, Two, Three, Four, Five, Six, Seven, Eight, Nine,
		A = 0x41, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		NumPad0 = 0x60, NumPad1, NumPad2, NumPad3, NumPad4, NumPad5, NumPad6, NumPad7, NumPad8, NumPad9,
		F1 = 0x70, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12
	};
}input;
bool KEYS[256];

enum Animaciones
{
	Idle,
	Running,
	Jump,
	fall,
	Dash,
};
enum Stages
{
	Inicio,
	FirstStage,
	GameOver
};
enum Frames {
	Frame0,
	Frame1,
	Frame2,
	Frame3,
	Frame4,
	Frame5,
	Frame6,
	Frame7,
	Frame8,
	Frame9,
	Frame10,
	Frame11,
	Frame12,
	Frame13,
	Frame14,
	Frame15,
	Frame16,
	Frame17,
	Frame18,
	Frame19,
	Frame20
};

enum ScoreName
{
	A = L'A', B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z
};

struct DatosPersonaje {
	//Ruta donde esta nuestra imagen
	WCHAR BmpW[MAX_PATH] = L"Recursos/Olas.bmp";
	WCHAR BmpW2[MAX_PATH] = L"Recursos/RINGS.bmp";
	WCHAR BmpW3[MAX_PATH] = L"Recursos/Enemigo.bmp";
	miBitmap HojaSprite, HojaSpriteR, HojaSpriteE;

	//Posicion relativa de nuestro Sprite en la ventana
	int XCurrentCoordDraw;
	int YCurrentCoordDraw;

	//Dimensiones en pixeles que se requieren para dibujarlo en la ventana
	int HeightDraw;
	int WeightDraw;

	int Animaciones = 5;
	int idAnimacionIdle = 0;
	int FramesAnimacionIdle = 10;

	int idAnimacionCorriendo = 1;
	int FramesAniCorriendo = 11;

	int idAnimacionSaltando = 2;
	int FramesAnimacionSaltando = 6;
	float velocityY;
	bool AnimacionSaltando;
	RECT groundHitbox; // Hitbox del suelo
	RECT objectHitbox; // Hitbox del objeto

	int idAnimacioncallendo = 3;
	int FramesAnimacioncallendo = 5;

	int idAnimacionCaminando = 4;
	int FramesAnimacionCaminando = 11;


	int idAnimacionGameOver = 5;
	int FramesAnimacionGameOver = 0;

	FrameArray** FrameSpriteArray;

	int HitboxAncho;
	int HitboxAlto;
	int HitboxX = XCurrentCoordDraw;
	int HitboxY = YCurrentCoordDraw;
	int HitboxScaleX;
	int HitboxScaleY;

	
}miPersonaje, miRings, Enemigo;

struct Puntuaciones
{
	int puntajesArr = 0;
	char nombresArr[3] = { 'A','A', 'A' };

}punts[3];
int cont;


struct DatosStage {
	int Escenarios = 4;
	miBitmap ImagenEscenario1;
	WCHAR Bmp1[MAX_PATH] = L"Recursos/Layout3.bmp";
	miBitmap ImagenEscenario2;
	WCHAR Bmp2[MAX_PATH] = L"Recursos/Layout5.bmp";
	miBitmap ImagenEscenario3;
	WCHAR Bmp3[MAX_PATH] = L"Recursos/Fin11.bmp";
	miBitmap ImagenEscenario4;
	WCHAR Bmp4[MAX_PATH] = L"Recursos/Victoria1.bmp";
		
}miStage;

struct Colisiones
{
	int cantidad = 1;
	int ScaleX;
	int ScaleY;
	miBitmap Colision1;
	WCHAR Bmp1[MAX_PATH] = L"";
	int HitboxAncho = 64;
	int HitboxAlto = 19;
	int HitboxX = 480;
	int HitboxY = 380;
	int HitboxScaleX;
	int HitboxScaleY;
}MisColisiones;

//Variables Globales

long puntos = 2500;
int RINGS = 0;
const int ANCHO_VENTANA = 1800;
const int ALTO_VENTANA = 900;
const int BPP = 4;
const int TICK = 99;//99
const unsigned int TRANSPARENCY = 0xFF00FF00;
ZPlay* player = CreateZPlay();//Generamos un objeto puntero para nuestro reproductor
ZPlay* playerEfecto = CreateZPlay();
TStreamStatus status;
bool pausa = false;
int* ptrBufferPixelsWindow;
int AnimacionActual;
int FrameActual = 0;
int FrameActualR = 0;
int FrameActualE = 0;
int DelayFrameAnimation = 0;
bool PANTALLAINICIAL = true;
bool RECORERPANTALLA = false;
bool PANTALLADERROTA = false;
bool PANTALLACOMOJUGAR = false;
bool POSICIONPERSONAJE = true;
bool PANTALLAVICTORIA = false; 
bool haAlcanzadoMitad = false;
bool PUNTOINICIAL = false;
bool colisionmoneda = false;
bool coliionpinchos = false;
const int TIMER_ID = 1;
const int TIMER_INTERVAL = 16;  // Aproximadamente 60 FPS
bool Tomarcoins = false;


const float JUMP_VELOCITY = -14.0f;  // Velocidad inicial del salto
const float GRAVITY = 0.4f;          // Aceleración de la gravedad
const float MOVEMENT_SPEED = 5.0f;   // Velocidad de movimiento horizontal

HWND hWnd;
int countdownSeconds = 120;
bool running = false;

int startX = 50; // Coordenada x de inicio de la porción de imagen a mostrar
int startY = 550; // Coordenada y de inicio de la porción de imagen a mostrar

int largoXmapa = 16200;
clock_t timeC_t = 0;

int Letraint = A;
//registro nombre
bool RegName = false;
bool Pletra = true;
bool Sletra = false;
bool Tletra = false;

wchar_t LetraChar[1];
wchar_t PLetraAux[1];
wchar_t SLetraAux[1];
wchar_t TLetraAux[1];
wchar_t Auxnombre[3];

wstring SecondsToTimeString(int seconds)
{
	int minutes = seconds / 60;
	int remainingSeconds = seconds % 60;

	std::wstring timeString;
	timeString += std::to_wstring(minutes);
	timeString += L":";

	if (remainingSeconds < 10)
	{
		timeString += L"0";
	}

	timeString += std::to_wstring(remainingSeconds);

	return timeString;
}




//Definicion de funciones
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void LimpiarFondo(int* ptrBuffer, unsigned int color, int area);
void TranScaleblt(int* punteroDestino, int* punteroOrigen, int inicioXDestino, int inicioYDestino, int inicioXOrigen, int inicioYOrigen, int ancho, int alto, int anchodefondo, int anchodeorigen, int escalaX, int escalaY);
void mirrorxblt(int* punteroDestino,//Buffer donde comenzara a pasar los datos
	int* punteroOrigen, // Buffer que copiara los datos del bmp
	int inicioXDestino,//ventana
	int inicioYDestino,
	int inicioXOrigen,//bmp
	int inicioYOrigen,
	int ancho,//ancho del sprite
	int alto,//alto del sprite
	int anchodefondo,//dimensiones
	int anchodeorigen,
	int escalaX,//tamaño de escala X bmp
	int escalaY,//tamaño de escala Y bmp
	const unsigned int TRANSPARENCY);
void TranScalebltRings(int* punteroDestino, int* punteroOrigen, int inicioXDestino, int inicioYDestino, int inicioXOrigen, int inicioYOrigen, int ancho, int alto, int anchodefondo, int anchodeorigen, int escalaX, int escalaY);
void DibujaHitbox(int* ptrBuffer, unsigned int color, int anchoWnd, int altoWnd, int HitboxX, int HitboxY, int HitboxAncho, int HitboxAlto, int escalaX, int escalaY);
void MainRender(HWND hWnd);
void Init();
void KeysEvents();
void ReproductorPausa();
void ReproductorReproduce();
void ReproductorInicializaYReproduce();
void ReproductorCambiarCancionYReproduce(int);
void ReproductorEfecto(string dir);
void CargaFramesSprite();
void dibujarcontador(HDC hdc);
void marcador(HDC hdc);
void UpdateObject();
void UpdateTimer();
void CrearMono();
void CargaFramesSpriteRings(); 
void CrearRings();
void DrawHitbox(HDC hdc, RECT hitbox);
void CrearEnemigo();
bool DetectarColision(int x1, int y1, int ancho1, int alto1, int x2, int y2, int ancho2, int alto2);
void CargaFramesSpriteEnemigo();
void dibujar_Anillos(HDC hdc);
void guardarPuntaje(int newRecord);
void LeerPuntaje();
bool compararPuntaje(int newRecord);
void ReinicioMemoria();

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	WNDCLASSEX wc = {};									// Windows Class Structure
	HWND hWnd;
	MSG msg;

	TCHAR szAppName[] = TEXT("MyWinAPIApp");
	TCHAR szAppTitle[] = TEXT("FARFIX");

	hInstance = GetModuleHandle(NULL);				// Grab An Instance For Our Window

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc = (WNDPROC)WndProc;					// WndProc Handles Messages
	wc.cbClsExtra = 0;									// No Extra Window Data
	wc.cbWndExtra = 0;									// No Extra Window Data
	wc.hInstance = hInstance;							// Set The Instance	
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);			// No Background Required For GL
	wc.lpszMenuName = NULL;									// We Don't Want A Menu	
	wc.lpszClassName = szAppName;							// Set The Class Name
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	miPersonaje.groundHitbox = { 0, 650, 800, 700 };
	miPersonaje.objectHitbox = { 100, 450, 150, 500 };

	if (!RegisterClassEx(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,
			"Fallo al registrar clase (Failed To Register The Window Class).",
			"ERROR",
			MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}

	hWnd = CreateWindowEx(
		WS_EX_CLIENTEDGE | WS_EX_APPWINDOW | WS_EX_WINDOWEDGE,	// Extended Style For The Window
		szAppName,							// Class Name
		szAppTitle,							// Window Title
		WS_OVERLAPPEDWINDOW |				// Defined Window Style
		WS_CLIPSIBLINGS |					// Required Window Style
		WS_CLIPCHILDREN,					// Required Window Style
		0, 0,								// Window Position
		ANCHO_VENTANA,						// Calculate Window Width
		ALTO_VENTANA,						// Calculate Window Height
		NULL,								// No Parent Window
		NULL,								// No Menu
		hInstance,							// Instance
		NULL);								// Pass this class To WM_CREATE								

	if (hWnd == NULL) {
		MessageBox(NULL,
			"Error al crear ventana (Window Creation Error).",
			"ERROR",
			MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}

	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	Init();
	ShowWindow(hWnd, nCmdShow);
	SetFocus(hWnd);

	
	running = true;
	
	Gapad = new GamePadRR(1);
	SetTimer(hWnd, TICK, TICK, NULL);
	SetTimer(hWnd, 2, 1000, NULL);
	SetTimer(hWnd, TIMER_ID, TIMER_INTERVAL, nullptr);
	ZeroMemory(&msg, sizeof(MSG));


	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// Limpiar
	KillTimer(hWnd, TIMER_ID);
	UnregisterClass(wc.lpszClassName, hInstance);

	return(int)msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	switch (uMsg)									// Check For Windows Messages
	{
	case WM_CREATE:
		LeerPuntaje();
		break;
	case WM_KEYDOWN:
	{
		KEYS[wParam] = true;
	}
	break;
	case WM_KEYUP:
	{
		KEYS[wParam] = false;
		switch (wParam) {
		case VK_F1://F1
		{
			ReproductorPausa();
		}
		return 0;
		case VK_F2://F1
		{
			ReproductorPausa();
		}
		return 0;
		}
	}
	break;
	case WM_TIMER:
		UpdateObject();
		/*UpdateTimer();*/
	
		if (wParam == TICK)
		{
			MainRender(hWnd);
			player->GetStatus(&status);
			if (!pausa && status.fPlay == 0) {
				ReproductorReproduce();
			}
			if (PANTALLAINICIAL == false)
			{
				puntos--;
			}

			//papu
	
			 // Actualizar la posición del objeto en función de la velocidad vertical
			miPersonaje.velocityY += GRAVITY;
			miPersonaje.objectHitbox.top = miPersonaje.YCurrentCoordDraw;
			miPersonaje.objectHitbox.bottom = miPersonaje.YCurrentCoordDraw + 150; // Ajusta el tamaño de la hitbox del objeto según sea necesario

			// Detectar colisión con el suelo
			RECT intersection;
			if (IntersectRect(&intersection, &miPersonaje.objectHitbox, &miPersonaje.groundHitbox)) {
				// Si hay colisión, reposicionar el objeto en la parte superior del suelo
				miPersonaje.YCurrentCoordDraw = miPersonaje.groundHitbox.top - 150; // Ajusta la posición del objeto según sea necesario
				miPersonaje.objectHitbox.top = miPersonaje.YCurrentCoordDraw;
				miPersonaje.objectHitbox.bottom = miPersonaje.YCurrentCoordDraw + 150; // Ajusta el tamaño de la hitbox del objeto según sea necesario
				miPersonaje.velocityY = 0; // Restablece la velocidad vertical del objeto
				miPersonaje.AnimacionSaltando = false; // Restablece el indicador de salto del objeto
			}
			else {
				// Si no hay colisión, ajustar la velocidad vertical para simular la gravedad
				GRAVITY; // Ajusta el valor de gravedad según sea necesario
			}

			//papu
		}

		/*if (PANTALLAINICIAL == false)
		{
			UpdateTimer();
		}
		InvalidateRect(hWnd, NULL, TRUE);*/
		if (wParam ==  2)
		{
			if (PANTALLAINICIAL == false)
			{
				UpdateTimer();
			}
			
		

			InvalidateRect(hWnd, NULL, TRUE);
		}
		
		break;
	case WM_PAINT:
	{
		

		//Eso tilin
		

			/*HDC hdc;*/
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			HBITMAP h_CMC = CreateBitmap(ANCHO_VENTANA, ALTO_VENTANA, 1, 32, ptrBufferPixelsWindow);
			HDC hdcMem = CreateCompatibleDC(hdc);
			HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, h_CMC);//Creacion de marco para proyecto

			BitBlt(hdc, 0, 0, ANCHO_VENTANA, ALTO_VENTANA, hdcMem, 0, 0, SRCCOPY);

			

		 if (!PANTALLADERROTA && !PANTALLAINICIAL && !PANTALLAVICTORIA)
		{


		TextOut(hdc, 46, 30, (LPCSTR)"Time:", 5);
		TextOut(hdc, 330, 30, "Score:", 6);
		TextOut(hdc, 330, 60, "Rings:", 6);
		marcador(hdc);
		dibujarcontador(hdc);
		dibujar_Anillos(hdc);
		/*DrawHitbox(hdc, miPersonaje.groundHitbox);
		DrawHitbox(hdc, miPersonaje.objectHitbox);*/

		
		 }
		 else if (PANTALLADERROTA && PANTALLAINICIAL && !PANTALLAVICTORIA)
		 {

			 COLORREF bgColor = RGB(255, 255, 255); // Blanco
			 COLORREF textColor = RGB(0, 0, 0); // Negro

			 int Fuente2 = 50;
			 HFONT fuente2 = CreateFont(Fuente2, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
				 OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_DONTCARE, TEXT("BankGothic Lt BT"));
			 SelectObject(hdc, fuente2);
			 SetTextColor(hdc, textColor);
			 SetBkMode(hdc, TRANSPARENT); // Hacer el fondo transparente
			 TextOutW(hdc, 480, 10, L"FINAL SCORE:", 12);

			 wstring puntaje = to_wstring(puntos);
			 TextOutW(hdc, 900, 10, puntaje.c_str(), puntaje.size());

			 string auxpuntos1 = to_string(punts[0].puntajesArr);
			 string auxpuntos2 = to_string(punts[1].puntajesArr);
			 string auxpuntos3 = to_string(punts[2].puntajesArr);
			 TextOut(hdc, 480, 200, (LPCSTR)punts[0].nombresArr, 3);
			 TextOut(hdc, 640, 200, (LPCSTR)auxpuntos1.c_str(), auxpuntos1.size());

			 TextOut(hdc, 480, 260, (LPCSTR)punts[1].nombresArr, 3);
			 TextOut(hdc, 640, 260, (LPCSTR)auxpuntos2.c_str(), auxpuntos2.size());

			 TextOut(hdc, 480, 320, (LPCSTR)punts[2].nombresArr, 3);
			 TextOut(hdc, 640, 320, (LPCSTR)auxpuntos3.c_str(), auxpuntos3.size());

		 }
		 else if (!PANTALLADERROTA && PANTALLAINICIAL && PANTALLAVICTORIA)
		 {

			 COLORREF bgColor = RGB(255, 255, 255); // Blanco
			 COLORREF textColor = RGB(0, 0, 0); // Negro

			 int Fuente2 = 50;
			 HFONT fuente2 = CreateFont(Fuente2, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
				 OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_DONTCARE, TEXT("BankGothic Lt BT"));
			 SelectObject(hdc, fuente2);
			 SetTextColor(hdc, textColor);
			 SetBkMode(hdc, TRANSPARENT); // Hacer el fondo transparente
			 TextOutW(hdc, 480, 10, L"FINAL SCORE:", 12);
			 
			 wstring puntaje = to_wstring(puntos);
			 TextOutW(hdc, 900, 10, puntaje.c_str(), puntaje.size());

			 string auxpuntos1 = to_string(punts[0].puntajesArr);
			 string auxpuntos2 = to_string(punts[1].puntajesArr);
			 string auxpuntos3 = to_string(punts[2].puntajesArr);
			 TextOut(hdc, 480, 200, (LPCSTR)punts[0].nombresArr, 3);
			 TextOut(hdc, 640, 200, (LPCSTR)auxpuntos1.c_str(), auxpuntos1.size());

			 TextOut(hdc, 480, 260, (LPCSTR)punts[1].nombresArr, 3);
			 TextOut(hdc, 640, 260, (LPCSTR)auxpuntos2.c_str(), auxpuntos2.size());

			 TextOut(hdc, 480, 320, (LPCSTR)punts[2].nombresArr, 3);
			 TextOut(hdc, 640, 320, (LPCSTR)auxpuntos3.c_str(), auxpuntos3.size());

			 if (compararPuntaje(puntos)) {
				 LetraChar[0] = static_cast<wchar_t>(Letraint);
				 TextOutW(hdc, 1000, 80, L"NUEVO RECORD!!", 15);
				 if (Pletra)
				 {
					 TextOutW(hdc, 900, 200, L"^", 1);
					 TextOutW(hdc, 900, 220, (LPCWSTR)LetraChar, 1);
					 TextOutW(hdc, 900, 240, L"_", 1);
				 }
				 else if (Sletra)
				 {
					 TextOutW(hdc, 980, 200, L"^", 1);
					 TextOutW(hdc, 900, 220, (LPCWSTR)&PLetraAux, 1);
					 TextOutW(hdc, 980, 220, (LPCWSTR)&LetraChar, 1);
					 TextOutW(hdc, 980, 240, L"_", 1);
				 }
				 else if (Tletra)
				 {
					 TextOutW(hdc, 900, 220, (LPCWSTR)&PLetraAux, 1);
					 TextOutW(hdc, 1060, 200, L"^", 1);
					 TextOutW(hdc, 980, 220, (LPCWSTR)&SLetraAux, 1);
					 TextOutW(hdc, 1060, 220, (LPCWSTR)&LetraChar, 1);
					 TextOutW(hdc, 1060, 240, L"_", 1);
				 }
				 else {
					 TextOutW(hdc, 900, 200, (LPCWSTR)Auxnombre, 3);
					 guardarPuntaje(puntos);
					 ReinicioMemoria();
				 }
			 }
			 DeleteObject(fuente2);

		 }

	
	
		/*dibujarcontador(hdc);*/

		EndPaint(hWnd, &ps);

		DeleteObject(h_CMC);
		//SelectObject(hdcMem, hbmOld);
		DeleteDC(hdcMem);
		//DeleteObject(hbmOld);
	}
	break;
	/*case WM_KEYDOWN:
		{
			KEYS[ wParam ] = true;
		}
		break;*/
	case WM_CLOSE:
	{
		DestroyWindow(hWnd);
	}
	break;
	case WM_DESTROY: //Send A Quit Message
	{
		KillTimer(hWnd, 2);
		KillTimer(hWnd, TICK);
		PostQuitMessage(0);
	}
	break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}




/* Inicializacion de variables y reserva de memoria.
	*/




void Init()
{
	//Empezamos a reproducir musica
	ReproductorInicializaYReproduce();
	if (PANTALLAINICIAL && !PANTALLADERROTA && !PANTALLAVICTORIA)
	{
		ReproductorCambiarCancionYReproduce(1);


	} else if (!PANTALLAINICIAL && !PANTALLADERROTA && !PANTALLAVICTORIA) {
		ReproductorCambiarCancionYReproduce(0);
	}
	
	
	//Cargamos imagen bitmap de nuestro escenario



		CargaFramesSprite();
		CargaFramesSpriteEnemigo();
		CargaFramesSpriteRings();
	
		CargaFramesSpriteRings();
		miStage.ImagenEscenario1 = gdipLoad(miStage.Bmp1);
		miStage.ImagenEscenario2 = gdipLoad(miStage.Bmp2);
		miStage.ImagenEscenario3 = gdipLoad(miStage.Bmp3);
		miStage.ImagenEscenario4 = gdipLoad(miStage.Bmp4);
	//Definimos un puntero del total de pixeles que tiene nuestra ventana
	ptrBufferPixelsWindow = new int[ANCHO_VENTANA * ALTO_VENTANA];

}

void CargaFramesSprite() {
	//Cargamos primero las hojas de sprite
	miPersonaje.HojaSprite = gdipLoad(miPersonaje.BmpW);

	//Definiendo las coordenadas iniciales en pantalla donde iniciaremos
	miPersonaje.XCurrentCoordDraw = 300;
	miPersonaje.YCurrentCoordDraw = 500;

	//Hitbox
	miPersonaje.HitboxAncho = 40;
	miPersonaje.HitboxAlto = 80;
	miPersonaje.HitboxX = miPersonaje.XCurrentCoordDraw;
	miPersonaje.HitboxY = miPersonaje.YCurrentCoordDraw;
	miPersonaje.HitboxScaleX = 2;
	miPersonaje.HitboxScaleY = 2;

	//Definiendo los tamaños de nuestro sprite para renderizarlo en la ventana
	miPersonaje.HeightDraw = 3;
	miPersonaje.WeightDraw = 41;

	//Definiendo las dimenciones en base al # de Animaciones
	miPersonaje.FrameSpriteArray = new FrameArray * [miPersonaje.Animaciones];
	//Definiendo la cantidad de Frames en base a cada Animacion
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle] = new FrameArray[miPersonaje.FramesAnimacionIdle];
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionCorriendo] = new FrameArray[miPersonaje.FramesAniCorriendo];
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionSaltando] = new FrameArray[miPersonaje.idAnimacionSaltando];
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacioncallendo] = new FrameArray[miPersonaje.idAnimacioncallendo];

	//Cargando Frames a nuestro arreglo del sprite
	// ------ - Animacion 1 Idle -------- //
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame0].x = 7; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame0].y = 18;
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame0].ancho = 45; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame0].alto = 85;

	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame1].x = 53; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame1].y = 18;
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame1].ancho = 45; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame1].alto = 85;

	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame2].x = 101; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame2].y = 18;
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame2].ancho = 45; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame2].alto = 85;

	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame3].x = 145; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame3].y = 18;
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame3].ancho = 45; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame3].alto = 85;

	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame4].x = 7; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame4].y = 18;
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame4].ancho = 45; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame4].alto = 85;

	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame5].x = 53; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame5].y = 18;
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame5].ancho = 45; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame5].alto = 90;

	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame6].x = 101; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame6].y = 18;
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame6].ancho = 45; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame6].alto = 90;

	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame7].x = 145; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame7].y = 18;
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame7].ancho = 45; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame7].alto = 90;

	// ------- Animacion 2 Idle -------- //

	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionCorriendo][Frame0].x = 6; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionCorriendo][Frame0].y = 105;
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionCorriendo][Frame0].ancho = 55; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionCorriendo][Frame0].alto = 70;

	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionCorriendo][Frame1].x = 82; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionCorriendo][Frame1].y = 105;
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionCorriendo][Frame1].ancho = 55; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionCorriendo][Frame1].alto = 70;

	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionCorriendo][Frame2].x = 152; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionCorriendo][Frame2].y = 105;
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionCorriendo][Frame2].ancho = 55; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionCorriendo][Frame2].alto = 70;

	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionCorriendo][Frame3].x = 220; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionCorriendo][Frame3].y = 105;
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionCorriendo][Frame3].ancho = 55; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionCorriendo][Frame3].alto = 70;

	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionCorriendo][Frame4].x = 288; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionCorriendo][Frame4].y = 105;
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionCorriendo][Frame4].ancho = 55; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionCorriendo][Frame4].alto = 70;

	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionCorriendo][Frame5].x = 348; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionCorriendo][Frame5].y = 105;
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionCorriendo][Frame5].ancho = 55; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionCorriendo][Frame5].alto = 70;

	// ------- Animacion 3 Saltando -------- //

	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionSaltando][Frame0].x = 7; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionSaltando][Frame0].y = 185;
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionSaltando][Frame0].ancho = 42; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionSaltando][Frame0].alto = 74;

	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionSaltando][Frame1].x = 80; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionSaltando][Frame1].y = 185;
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionSaltando][Frame1].ancho = 42; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionSaltando][Frame1].alto = 74;

	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionSaltando][Frame2].x = 141; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionSaltando][Frame2].y = 185;
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionSaltando][Frame2].ancho = 42; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionSaltando][Frame2].alto = 74;

	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionSaltando][Frame3].x = 202; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionSaltando][Frame3].y = 185;
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionSaltando][Frame3].ancho = 42; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionSaltando][Frame3].alto = 74;

	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionSaltando][Frame4].x = 251; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionSaltando][Frame4].y = 185;
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionSaltando][Frame4].ancho = 42; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionSaltando][Frame4].alto = 74;

	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionSaltando][Frame5].x = 296; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionSaltando][Frame5].y = 185;
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionSaltando][Frame5].ancho = 42; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionSaltando][Frame5].alto = 74;

	// ------- Animacion 3 callendo -------- //

	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacioncallendo][Frame0].x = 346; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionSaltando][Frame0].y = 185;
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacioncallendo][Frame0].ancho = 55; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionSaltando][Frame0].alto = 80;

	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacioncallendo][Frame1].x = 346; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionSaltando][Frame2].y = 185;
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacioncallendo][Frame1].ancho = 55; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionSaltando][Frame2].alto = 80;

	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacioncallendo][Frame2].x = 405; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionSaltando][Frame3].y = 185;
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacioncallendo][Frame2].ancho = 55; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionSaltando][Frame3].alto = 80;


}

void CargaFramesSpriteRings() {
	//Cargamos primero las hojas de sprite
	miRings.HojaSpriteR = gdipLoad(miRings.BmpW2);

	//Definiendo las coordenadas iniciales en pantalla donde iniciaremos
	miRings.XCurrentCoordDraw = 250;
	miRings.YCurrentCoordDraw = 400;

	//Hitbox
	miRings.HitboxAncho = 20;
	miRings.HitboxAlto = 20;
	/*miRings.HitboxX = miRings.XCurrentCoordDraw;
	miRings.HitboxY = miRings.YCurrentCoordDraw;*/
	miRings.HitboxScaleX = 3;
	miRings.HitboxScaleY = 3;

	//Definiendo los tamaños de nuestro sprite para renderizarlo en la ventana
	miRings.HeightDraw = 3;
	miRings.WeightDraw = 41;

	//Definiendo las dimenciones en base al # de Animaciones
	miRings.FrameSpriteArray = new FrameArray * [miRings.Animaciones];
	//Definiendo la cantidad de Frames en base a cada Animacion
	miRings.FrameSpriteArray[miRings.idAnimacionIdle] = new FrameArray[miRings.FramesAnimacionIdle];

	//Cargando Frames a nuestro arreglo del sprite
	// ------ - Animacion 1 Idle -------- //
	miRings.FrameSpriteArray[miRings.idAnimacionIdle][Frame0].x = 6; miRings.FrameSpriteArray[miRings.idAnimacionIdle][Frame0].y = 31;
	miRings.FrameSpriteArray[miRings.idAnimacionIdle][Frame0].ancho = 19; miRings.FrameSpriteArray[miRings.idAnimacionIdle][Frame0].alto = 20;

	miRings.FrameSpriteArray[miRings.idAnimacionIdle][Frame1].x = 30; miRings.FrameSpriteArray[miRings.idAnimacionIdle][Frame1].y = 31;
	miRings.FrameSpriteArray[miRings.idAnimacionIdle][Frame1].ancho = 15; miRings.FrameSpriteArray[miRings.idAnimacionIdle][Frame1].alto = 20;

	miRings.FrameSpriteArray[miRings.idAnimacionIdle][Frame2].x = 50; miRings.FrameSpriteArray[miRings.idAnimacionIdle][Frame2].y = 31;
	miRings.FrameSpriteArray[miRings.idAnimacionIdle][Frame2].ancho = 9; miRings.FrameSpriteArray[miRings.idAnimacionIdle][Frame2].alto = 20;

	miRings.FrameSpriteArray[miRings.idAnimacionIdle][Frame3].x = 64; miRings.FrameSpriteArray[miRings.idAnimacionIdle][Frame3].y = 31;
	miRings.FrameSpriteArray[miRings.idAnimacionIdle][Frame3].ancho = 15; miRings.FrameSpriteArray[miRings.idAnimacionIdle][Frame3].alto = 20;

	miRings.FrameSpriteArray[miRings.idAnimacionIdle][Frame4].x = 6; miRings.FrameSpriteArray[miRings.idAnimacionIdle][Frame4].y = 31;
	miRings.FrameSpriteArray[miRings.idAnimacionIdle][Frame4].ancho = 19; miRings.FrameSpriteArray[miRings.idAnimacionIdle][Frame4].alto = 20;

	miRings.FrameSpriteArray[miRings.idAnimacionIdle][Frame5].x = 30; miRings.FrameSpriteArray[miRings.idAnimacionIdle][Frame5].y = 31;
	miRings.FrameSpriteArray[miRings.idAnimacionIdle][Frame5].ancho = 15; miRings.FrameSpriteArray[miRings.idAnimacionIdle][Frame5].alto = 20;

	miRings.FrameSpriteArray[miRings.idAnimacionIdle][Frame6].x = 50; miRings.FrameSpriteArray[miRings.idAnimacionIdle][Frame6].y = 31;
	miRings.FrameSpriteArray[miRings.idAnimacionIdle][Frame6].ancho = 9; miRings.FrameSpriteArray[miRings.idAnimacionIdle][Frame6].alto = 20;

	miRings.FrameSpriteArray[miRings.idAnimacionIdle][Frame7].x = 64; miRings.FrameSpriteArray[miRings.idAnimacionIdle][Frame7].y = 31;
	miRings.FrameSpriteArray[miRings.idAnimacionIdle][Frame7].ancho = 15; miRings.FrameSpriteArray[miRings.idAnimacionIdle][Frame7].alto = 20;
}

void CargaFramesSpriteEnemigo() {

	//Cargamos primero las hojas de sprite
	Enemigo.HojaSpriteE = gdipLoad(Enemigo.BmpW3);

	//Definiendo las coordenadas iniciales en pantalla donde iniciaremos
	Enemigo.XCurrentCoordDraw = 700;
	Enemigo.YCurrentCoordDraw = 575;

	//Hitbox
	Enemigo.HitboxAncho = 20;
	Enemigo.HitboxAlto = 5;
	//Enemigo.HitboxX = Enemigo.XCurrentCoordDraw;
	//Enemigo.HitboxY = Enemigo.YCurrentCoordDraw;
	Enemigo.HitboxScaleX = 3;
	Enemigo.HitboxScaleY = 3;

	//Definiendo los tamaños de nuestro sprite para renderizarlo en la ventana
	Enemigo.HeightDraw = 3;
	Enemigo.WeightDraw = 41;

	//Definiendo las dimenciones en base al # de Animaciones
	Enemigo.FrameSpriteArray = new FrameArray * [Enemigo.Animaciones];
	//Definiendo la cantidad de Frames en base a cada Animacion
	Enemigo.FrameSpriteArray[Enemigo.idAnimacionIdle] = new FrameArray[Enemigo.FramesAnimacionIdle];

	//Cargando Frames a nuestro arreglo del sprite
	// ------ - Animacion 1 Idle -------- //
	Enemigo.FrameSpriteArray[Enemigo.idAnimacionIdle][Frame0].x = 122; Enemigo.FrameSpriteArray[Enemigo.idAnimacionIdle][Frame0].y = 27;
	Enemigo.FrameSpriteArray[Enemigo.idAnimacionIdle][Frame0].ancho = 43; Enemigo.FrameSpriteArray[Enemigo.idAnimacionIdle][Frame0].alto = 33;

	Enemigo.FrameSpriteArray[Enemigo.idAnimacionIdle][Frame1].x = 164; Enemigo.FrameSpriteArray[Enemigo.idAnimacionIdle][Frame1].y = 27;
	Enemigo.FrameSpriteArray[Enemigo.idAnimacionIdle][Frame1].ancho = 43; Enemigo.FrameSpriteArray[Enemigo.idAnimacionIdle][Frame1].alto = 33;

	Enemigo.FrameSpriteArray[Enemigo.idAnimacionIdle][Frame2].x = 122; Enemigo.FrameSpriteArray[Enemigo.idAnimacionIdle][Frame2].y = 27;
	Enemigo.FrameSpriteArray[Enemigo.idAnimacionIdle][Frame2].ancho = 43; Enemigo.FrameSpriteArray[Enemigo.idAnimacionIdle][Frame2].alto = 33;

	Enemigo.FrameSpriteArray[Enemigo.idAnimacionIdle][Frame3].x = 164; Enemigo.FrameSpriteArray[Enemigo.idAnimacionIdle][Frame3].y = 27;
	Enemigo.FrameSpriteArray[Enemigo.idAnimacionIdle][Frame3].ancho = 43; Enemigo.FrameSpriteArray[Enemigo.idAnimacionIdle][Frame3].alto = 33;

	Enemigo.FrameSpriteArray[Enemigo.idAnimacionIdle][Frame4].x = 122; Enemigo.FrameSpriteArray[Enemigo.idAnimacionIdle][Frame4].y = 27;
	Enemigo.FrameSpriteArray[Enemigo.idAnimacionIdle][Frame4].ancho = 43; Enemigo.FrameSpriteArray[Enemigo.idAnimacionIdle][Frame4].alto = 33;

	Enemigo.FrameSpriteArray[Enemigo.idAnimacionIdle][Frame5].x = 164; Enemigo.FrameSpriteArray[Enemigo.idAnimacionIdle][Frame5].y = 27;
	Enemigo.FrameSpriteArray[Enemigo.idAnimacionIdle][Frame5].ancho = 43; Enemigo.FrameSpriteArray[Enemigo.idAnimacionIdle][Frame5].alto = 33;

	Enemigo.FrameSpriteArray[Enemigo.idAnimacionIdle][Frame6].x = 122; Enemigo.FrameSpriteArray[Enemigo.idAnimacionIdle][Frame6].y = 27;
	Enemigo.FrameSpriteArray[Enemigo.idAnimacionIdle][Frame6].ancho = 43; Enemigo.FrameSpriteArray[Enemigo.idAnimacionIdle][Frame6].alto = 33;

	Enemigo.FrameSpriteArray[Enemigo.idAnimacionIdle][Frame7].x = 124; Enemigo.FrameSpriteArray[Enemigo.idAnimacionIdle][Frame7].y = 27;
	Enemigo.FrameSpriteArray[Enemigo.idAnimacionIdle][Frame7].ancho = 43; Enemigo.FrameSpriteArray[Enemigo.idAnimacionIdle][Frame7].alto = 33;


}




/* Funcion principal. Encargada de hacer el redibujado en pantalla cada intervalo (o "tick") del timer que se haya creado.
	@param hWnd. Manejador de la ventana.
	*/


void guardarPuntaje(int newRecord)
{
	ofstream archivo;
	archivo.open("../Plantilla LENS 2023 EX/Puntajes.txt", ios::out | ios::trunc);
	int indexMin = 0; // índice del puntaje más bajo
	for (int i = 0; i < cont; i++)
	{
		if (archivo.fail())
		{
			exit(1);
		}

		if (punts[i].puntajesArr < punts[indexMin].puntajesArr) {
			indexMin = i;
		}
	}
	if (newRecord >= punts[indexMin].puntajesArr) {
		// reemplazar puntaje más bajo con nuevo puntaje y nombre
		punts[indexMin].puntajesArr = newRecord;
		punts[indexMin].nombresArr[0] = Auxnombre[0];
		punts[indexMin].nombresArr[1] = Auxnombre[1];
		punts[indexMin].nombresArr[2] = Auxnombre[2];
	}
	bool swapped = false;
	do {
		swapped = false;
		for (int i = 0; i < cont - 1; i++) {
			if (punts[i].puntajesArr < punts[i + 1].puntajesArr) {
				swap(punts[i], punts[i + 1]);
				swapped = true;
			}
		}
	} while (swapped);
	for (int i = 0; i < 3; i++)
	{
		archivo << punts[i].nombresArr << endl;
		archivo << punts[i].puntajesArr << endl;
	}
}
void LeerPuntaje()
{
	{
		int contleer = 0;
		char texto[25];
		ifstream archivo("../Plantilla LENS 2023 EX/Puntajes.txt", ios::in);
		if (archivo.is_open())
		{

			while (!archivo.eof())
			{

				archivo >> texto;//guardamos los datos del archivo en la variable texto
				if (strcmp(texto, "") != 0)
				{


					if (contleer == 0)
					{
						punts[cont].nombresArr[0] = texto[0];
						punts[cont].nombresArr[1] = texto[1];
						punts[cont].nombresArr[2] = texto[2];
						contleer++;
					}
					else if (contleer == 1)
					{
						punts[cont].puntajesArr = atoi(texto);
						contleer = 0;
						cont++;
					}
				}

			}
			archivo.close();

		}
	}
}

bool compararPuntaje(int newRecord) {
	bool superaAlMenosUno = false;
	for (int i = 0; i < 3; i++) {
		if (newRecord >= punts[i].puntajesArr) {
			superaAlMenosUno = true;
			break;
		}
	}
	return superaAlMenosUno;
}


void dibujar_Anillos(HDC hdc) {
	COLORREF textColor = RGB(0, 0, 0); // Blanco
	COLORREF bgColor = RGB(255, 255, 255); // Negro
	int fontSize = 30;
	HFONT font = CreateFont(fontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_DONTCARE, TEXT("BankGothic Lt BT"));
	SelectObject(hdc, font);
	SetTextColor(hdc, textColor);
	SetBkColor(hdc, bgColor);
	SetBkMode(hdc, TRANSPARENT);

	wstring Anillos = to_wstring(RINGS);
	TextOutW(hdc, 570, 60, Anillos.c_str(), Anillos.size());
}



void DrawHitbox(HDC hdc, RECT hitbox) {
	// Configurar el color de la hitbox (en este caso, rojo)
	SetDCPenColor(hdc, RGB(255, 0, 0));
	SelectObject(hdc, GetStockObject(DC_PEN));

	// Dibujar el rectángulo de la hitbox
	Rectangle(hdc, hitbox.left, hitbox.top, hitbox.right, hitbox.bottom);
}


void UpdateObject() {

	// Actualizar la posición del objeto en cada temporizador
	if (miPersonaje.AnimacionSaltando) {
		miPersonaje.velocityY += GRAVITY;
		miPersonaje.YCurrentCoordDraw += static_cast<int>(miPersonaje.velocityY);
		if (miPersonaje.YCurrentCoordDraw >= ALTO_VENTANA) {
			// El objeto ha llegado al suelo, finalizar el salto
			miPersonaje.YCurrentCoordDraw = ALTO_VENTANA;
			miPersonaje.velocityY = 0.0f;
			miPersonaje.AnimacionSaltando = false;
		}
	}

}

void UpdateTimer()
{
	countdownSeconds--;

	if (countdownSeconds < 0)
	{
		PANTALLADERROTA = true;
		PANTALLAINICIAL = true;
		running = false;
		KillTimer(hWnd, 2);
	}

	// Redibujar el cronómetro
	InvalidateRect(hWnd, NULL, TRUE);
}


void dibujarcontador(HDC hdc) {

	// Convertir los segundos restantes a una cadena de tiempo
	std::wstring timeString = SecondsToTimeString(countdownSeconds);

	// Establecer el color y la fuente del texto
	COLORREF textColor = RGB(0, 0, 0); // Blanco
	COLORREF bgColor = RGB(255, 255, 255); // Negro
	SetTextColor(hdc, textColor);
	SetBkColor(hdc, bgColor);
	int fontSize = 30;
	HFONT font = CreateFont(fontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_DONTCARE, TEXT("BankGothic Lt BT"));
	SelectObject(hdc, font);

	// Obtener las dimensiones de la ventana
	RECT rect;
	rect.left = 160;
	rect.top = 30;
	rect.right = rect.left + 100;  // Ancho del rectángulo
	rect.bottom = rect.top + 50;  // Alto del rectángulo
	GetClientRect(hWnd, & rect);

	// Dibujar el texto centrado en la ventana
	DrawTextW(hdc, timeString.c_str(), -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	/* TextOut(hdc, 160, 30, (LPCSTR)tiempo.c_str(), tiempo.size());*/

	 // Liberar los recursos de la fuente
	DeleteObject(font);
}



void marcador(HDC hdc) {

	// Configurar el color del texto y el tamaño de fuente
	COLORREF textColor = RGB(0, 0, 0); // Blanco
	COLORREF bgColor = RGB(255, 255, 255); // Negro
	int fontSize = 30;
	HFONT font = CreateFont(fontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_DONTCARE, TEXT("BankGothic Lt BT"));
	SelectObject(hdc, font);
	SetTextColor(hdc, textColor);
	SetBkColor(hdc, bgColor);
	SetBkMode(hdc, TRANSPARENT); // Hacer el fondo transparente


	string puntoss = to_string(puntos);
	TextOut(hdc, 510, 30, (LPCSTR)puntoss.c_str(), puntoss.size());
}

void DibujaPixeles()
{

	

	if (PANTALLAINICIAL && !PANTALLADERROTA && !PANTALLAVICTORIA)
	{



		//Dibujamos el fondo
		TranScaleblt(ptrBufferPixelsWindow, (miStage.ImagenEscenario1.pixeles),
			0, 0,//Iniciamos a dibujar en la ventana en 0,0
			0, 0,//Indicamos cuales son las coordenadas para dibujar desde nuestra imagen; iniciamos en 0,0 desde nuestro escenario
			ANCHO_VENTANA, ALTO_VENTANA,//Definimos cuantos pixeles dibujaremos de nuestra imagen a la pantalla
			1800, miStage.ImagenEscenario1.ancho,
			1, 1);//Si ponemos un numero mayor a 1 estaremos repitiendo 2 veces la linea de pixeles en X o en Y

	}
	else if (!PANTALLAINICIAL && !PANTALLADERROTA && !PANTALLAVICTORIA) {


		if (!PANTALLAINICIAL && !PANTALLADERROTA && RECORERPANTALLA && !POSICIONPERSONAJE) {
			largoXmapa -= 20; // actualiza la posición vertical del escenario en cada fotograma
			// Si la posición vertical supera el borde superior de la imagen del escenario o alcanza la mitad del escenario, detenemos el movimiento del mapa
			if (largoXmapa >= miStage.ImagenEscenario2.ancho / 2 || largoXmapa + ANCHO_VENTANA > miStage.ImagenEscenario2.ancho) {
				largoXmapa = miStage.ImagenEscenario2.ancho / 2 - ANCHO_VENTANA;
			}

			// Si la posición horizontal del personaje supera el centro de la pantalla, detenemos su movimiento horizontal
			if (miPersonaje.XCurrentCoordDraw >= 1800 / 2) {
				largoXmapa += 20; // Avanzar el mapa mientras el personaje permanece en su posición
			}
			// Si la posición vertical es menor que el borde superior de la ventana, la ajustamos
			if (largoXmapa < 0) {
				largoXmapa = 0;
			}
			if (largoXmapa == 0) {
				largoXmapa = miStage.ImagenEscenario2.ancho - ANCHO_VENTANA;
			}
		}

		if (!PANTALLAINICIAL && !PANTALLADERROTA && RECORERPANTALLA && POSICIONPERSONAJE) {
			largoXmapa += 20; // Actualiza la posición vertical del escenario en cada fotograma en sentido contrario

			// Si la posición vertical supera el borde superior de la imagen del escenario, la ajustamos
			if (largoXmapa > miStage.ImagenEscenario2.ancho - ANCHO_VENTANA) {
				largoXmapa = 0;
			}

			// Si la posición vertical es mayor que el borde inferior de la ventana, la ajustamos
			if (largoXmapa + ANCHO_VENTANA > miStage.ImagenEscenario2.ancho) {
				largoXmapa = miStage.ImagenEscenario2.ancho - ANCHO_VENTANA;
			}

			// Si la posición vertical es menor que el borde superior de la ventana, la ajustamos
			if (largoXmapa < 0) {
				largoXmapa = 0;
			}
			if (largoXmapa == 0) {
				largoXmapa = miStage.ImagenEscenario2.ancho - ANCHO_VENTANA;
			}
		}



		//Dibujamos el fondo
		TranScaleblt(ptrBufferPixelsWindow, (miStage.ImagenEscenario2.pixeles),
			0, 0,//Iniciamos a dibujar en la ventana en 0,0
			largoXmapa, 0,//Indicamos cuales son las coordenadas para dibujar desde nuestra imagen; iniciamos en 0,0 desde nuestro escenario
			ANCHO_VENTANA, ALTO_VENTANA,//Definimos cuantos pixeles dibujaremos de nuestra imagen a la pantalla
			1800, miStage.ImagenEscenario2.ancho,
			1, 1);//Si ponemos un numero mayor a 1 estaremos repitiendo 2 veces la linea de pixeles en X o en Y
		CrearMono();
		CrearRings();
		CrearEnemigo();

	

		if (DetectarColision(miPersonaje.HitboxX, miPersonaje.HitboxY, miPersonaje.HitboxAncho * 2, miPersonaje.HitboxAlto * 2, Enemigo.HitboxX, Enemigo.HitboxY, Enemigo.HitboxAncho * 3, Enemigo.HitboxAlto * 3))
		{
			ReproductorEfecto("Recursos/GolpeSonido.mp3");
			PANTALLADERROTA = true;
			
		}

		if (DetectarColision(miPersonaje.HitboxX, miPersonaje.HitboxY, miPersonaje.HitboxAncho * 2, miPersonaje.HitboxAlto * 2, miRings.HitboxX, miRings.HitboxY, miRings.HitboxAncho * 3, miRings.HitboxAlto * 3))
		{
			ReproductorEfecto("Recursos/SonicRing.mp3");
			RINGS++;
			Tomarcoins = true;


		} 

		if (PANTALLADERROTA)
		{
			PANTALLAINICIAL = true;
			ReproductorCambiarCancionYReproduce(2);
		}

		if (RINGS == 10)
		{
			PANTALLAVICTORIA = true;
			PANTALLAINICIAL = true;
		}

	}
	else if (PANTALLADERROTA && !PANTALLAVICTORIA && PANTALLAINICIAL)
	{

		//Dibujamos el fondo
		TranScaleblt(ptrBufferPixelsWindow, (miStage.ImagenEscenario3.pixeles),
			0, 0,//Iniciamos a dibujar en la ventana en 0,0
			0, 0,//Indicamos cuales son las coordenadas para dibujar desde nuestra imagen; iniciamos en 0,0 desde nuestro escenario
			ANCHO_VENTANA, ALTO_VENTANA,//Definimos cuantos pixeles dibujaremos de nuestra imagen a la pantalla
			1800, miStage.ImagenEscenario3.ancho,
			1, 1);//Si ponemos un numero mayor a 1 estaremos repitiendo 2 veces la linea de pixeles en X o en Y
	}
	else if(!PANTALLADERROTA && PANTALLAVICTORIA && PANTALLAINICIAL) {
		//Dibujamos el fondo
		TranScaleblt(ptrBufferPixelsWindow, (miStage.ImagenEscenario4.pixeles),
			0, 0,//Iniciamos a dibujar en la ventana en 0,0
			0, 0,//Indicamos cuales son las coordenadas para dibujar desde nuestra imagen; iniciamos en 0,0 desde nuestro escenario
			ANCHO_VENTANA, ALTO_VENTANA,//Definimos cuantos pixeles dibujaremos de nuestra imagen a la pantalla
			1800, miStage.ImagenEscenario4.ancho,
			1, 1);//Si ponemos un numero mayor a 1 estaremos repitiendo 2 veces la linea de pixeles en X o en Y
	}

		////Dibujamos a nuestro personaje
		//TranScaleblt(ptrBufferPixelsWindow, (miPersonaje.HojaSprite.pixeles),
		//	miPersonaje.XCurrentCoordDraw, miPersonaje.YCurrentCoordDraw,
		//	miPersonaje.FrameSpriteArray[AnimacionActual][FrameActual].x, miPersonaje.FrameSpriteArray[AnimacionActual][FrameActual].y,
		//	miPersonaje.FrameSpriteArray[AnimacionActual][FrameActual].ancho, miPersonaje.FrameSpriteArray[AnimacionActual][FrameActual].alto,
		//	1869, miPersonaje.HojaSprite.ancho, 2, 2);
		//DibujaHitbox(ptrBufferPixelsWindow, 0xFFFF00FF, ANCHO_VENTANA, ALTO_VENTANA,
		//	miPersonaje.HitboxX, miPersonaje.HitboxY, miPersonaje.HitboxAncho, miPersonaje.HitboxAlto,
		//	miPersonaje.HitboxScaleX, miPersonaje.HitboxScaleY);

	//}else if (POSICIONPERSONAJE == false){

	//	

	//	//Dibujamos el fondo
	//	TranScaleblt(ptrBufferPixelsWindow, (miStage.ImagenEscenario2.pixeles),
	//		0, 0,//Iniciamos a dibujar en la ventana en 0,0
	//		0, 0,//Indicamos cuales son las coordenadas para dibujar desde nuestra imagen; iniciamos en 0,0 desde nuestro escenario
	//		ANCHO_VENTANA, ALTO_VENTANA,//Definimos cuantos pixeles dibujaremos de nuestra imagen a la pantalla
	//		1869, miStage.ImagenEscenario2.ancho,
	//		1, 1);//Si ponemos un numero mayor a 1 estaremos repitiendo 2 veces la linea de pixeles en X o en Y



	//	mirrorxblt(ptrBufferPixelsWindow, (miPersonaje.HojaSprite.pixeles),
	//		miPersonaje.XCurrentCoordDraw, miPersonaje.YCurrentCoordDraw,
	//		miPersonaje.FrameSpriteArray[AnimacionActual][FrameActual].x, miPersonaje.FrameSpriteArray[AnimacionActual][FrameActual].y,
	//		miPersonaje.FrameSpriteArray[AnimacionActual][FrameActual].ancho, miPersonaje.FrameSpriteArray[AnimacionActual][FrameActual].alto,
	//		1869, miPersonaje.HojaSprite.ancho, 2, 2, 0xFF00FF000);

	//	DibujaHitbox(ptrBufferPixelsWindow, 0xFFFF00FF, ANCHO_VENTANA, ALTO_VENTANA,
	//		miPersonaje.HitboxX, miPersonaje.HitboxY, miPersonaje.HitboxAncho, miPersonaje.HitboxAlto,
	//		miPersonaje.HitboxScaleX, miPersonaje.HitboxScaleY);
	//}


}

void CrearEnemigo() {
	static int velocidadX = 7;
	static int startY[] = {575}; // Posiciones Y posibles
	static int numStartY = sizeof(startY) / sizeof(startY[0]); // Número de posiciones Y posibles
	static int currentStartY = 575; // Posición Y actual
	static int startX = 1600; // Coordenada X inicial
	static int endX = 0; // Coordenada X final
	static int spriteHeight = 35; // Altura del sprite
	static bool resetPosition = false; // Bandera para reiniciar la posición
	static bool initialDelay = true; // Bandera para retraso inicial
	static clock_t lastResetTime = 0; // Variable para almacenar el tiempo del último reinicio

	

	// Verificar si ha transcurrido el tiempo de retraso inicial (8 segundos = 8 * CLOCKS_PER_SEC)
	if (initialDelay && (clock() - lastResetTime) < 2 * CLOCKS_PER_SEC || resetPosition && (clock() - lastResetTime) < 6 * CLOCKS_PER_SEC) {
		return; // Salir de la función si todavía no ha transcurrido el tiempo necesario
	}

	// Reiniciar posición y establecer el tiempo del reinicio
	if (resetPosition) {
		currentStartY = startY[rand() % numStartY];
		resetPosition = false;
	}

	// Establecer el retraso inicial en el primer ciclo
	if (initialDelay) {
		lastResetTime = clock();
		initialDelay = false;
	}
	

	/*DibujaHitbox(ptrBufferPixelsWindow, 0xFFFF00FF, ANCHO_VENTANA, ALTO_VENTANA,
		Enemigo.HitboxX, Enemigo.HitboxY, Enemigo.HitboxAncho, Enemigo.HitboxAlto,
		Enemigo.HitboxScaleX, Enemigo.HitboxScaleY);*/

	TranScaleblt(ptrBufferPixelsWindow, (Enemigo.HojaSpriteE.pixeles),
		startX, currentStartY,
		Enemigo.FrameSpriteArray[0][FrameActualE].x, Enemigo.FrameSpriteArray[0][FrameActualE].y,
		Enemigo.FrameSpriteArray[0][FrameActualE].ancho, spriteHeight,
		1800, Enemigo.HojaSpriteE.ancho, 2, 2);


	Enemigo.HitboxX = startX;
	Enemigo.HitboxY = currentStartY;
	Enemigo.HitboxAlto = 20;


	startX -= 24;
	if (startX < endX) {
			if (Enemigo.HitboxX <= 0) {
				startX = 1600;
				initialDelay = true;

			}
			// Reiniciar posición
			spriteHeight = 35;
			lastResetTime = clock();
			resetPosition = true;
	}

}

void CrearRings() {
	static int startY[] = { 420, 575, 500, 490 }; // Posiciones Y posibles
	static int numStartY = sizeof(startY) / sizeof(startY[4]); // Número de posiciones Y posibles
	static int currentStartY = startY[rand() % numStartY]; // Posición Y actual
	static int startX = 1600; // Coordenada X inicial
	static int endX = 0; // Coordenada X final
	static int spriteHeight = 20; // Altura del sprite
	static bool resetPosition = false; // Bandera para reiniciar la posición
	static bool initialDelay = true; // Bandera para retraso inicial
	static clock_t lastResetTime = 0; // Variable para almacenar el tiempo del último reinicio

	// Verificar si ha transcurrido el tiempo de retraso inicial (8 segundos = 8 * CLOCKS_PER_SEC)
	if (initialDelay && (clock() - lastResetTime) < 6 * CLOCKS_PER_SEC || resetPosition && (clock() - lastResetTime) < 10 * CLOCKS_PER_SEC) {
		return; // Salir de la función si todavía no ha transcurrido el tiempo necesario
	}

	// Reiniciar posición y establecer el tiempo del reinicio
	if (resetPosition) {
		currentStartY = startY[rand() % numStartY];
		resetPosition = false;
	}

	// Establecer el retraso inicial en el primer ciclo
	if (initialDelay) {
		lastResetTime = clock();
		initialDelay = false;
	}


	/*DibujaHitbox(ptrBufferPixelsWindow, 0xFFFF00FF, ANCHO_VENTANA, ALTO_VENTANA,
		miRings.HitboxX, miRings.HitboxY, miRings.HitboxAncho, miRings.HitboxAlto,
		miRings.HitboxScaleX, miRings.HitboxScaleY);*/
	if (!Tomarcoins)
	{
		TranScaleblt(ptrBufferPixelsWindow, (miRings.HojaSpriteR.pixeles),
			startX, currentStartY,
			miRings.FrameSpriteArray[0][FrameActualE].x, miRings.FrameSpriteArray[0][FrameActualE].y,
			miRings.FrameSpriteArray[0][FrameActualE].ancho, spriteHeight,
			1800, miRings.HojaSpriteR.ancho, 3, 3);

		miRings.HitboxX = startX;
		miRings.HitboxY = currentStartY;
		miRings.HitboxAlto = 20;
		startX -= 22;
	} else if (Tomarcoins)
	{
		startX = -22;
		miRings.HitboxX = startX;
		miRings.HitboxY = currentStartY;
		miRings.HitboxAlto = 20;
		

	} 
	if (startX < endX) {
		if (miRings.HitboxX <= 0) {
			startX = 1600;
			Tomarcoins = false;
			resetPosition = true;
		} 
		else
		{

			// Reiniciar posición
			startX = 0;
			spriteHeight = 20;
			lastResetTime = clock();
			resetPosition = true;
			Tomarcoins = false;
	}

		

	}

} 

void CrearMono() {

	switch (AnimacionActual)
	{
	case Idle: {
		miPersonaje.HitboxX = miPersonaje.XCurrentCoordDraw;
		miPersonaje.HitboxY = miPersonaje.YCurrentCoordDraw;


		}break;
	case Running: {
		miPersonaje.HitboxX = miPersonaje.XCurrentCoordDraw;
		miPersonaje.HitboxY = miPersonaje.YCurrentCoordDraw;

		if (miPersonaje.XCurrentCoordDraw >= ANCHO_VENTANA / 2)
		{
			haAlcanzadoMitad = true;
		}

		if (miPersonaje.XCurrentCoordDraw <= (ANCHO_VENTANA <= 120))
		{
			PUNTOINICIAL = true;
		}
		

	}break;

	case Jump: {

		miPersonaje.HitboxX = miPersonaje.XCurrentCoordDraw;
		miPersonaje.HitboxY = miPersonaje.YCurrentCoordDraw;

	}break;


	
	}

	if (POSICIONPERSONAJE == true)
	{
		TranScaleblt(ptrBufferPixelsWindow, (miPersonaje.HojaSprite.pixeles),
			miPersonaje.XCurrentCoordDraw, miPersonaje.YCurrentCoordDraw,
			miPersonaje.FrameSpriteArray[AnimacionActual][FrameActual].x, miPersonaje.FrameSpriteArray[AnimacionActual][FrameActual].y,
			miPersonaje.FrameSpriteArray[AnimacionActual][FrameActual].ancho, miPersonaje.FrameSpriteArray[AnimacionActual][FrameActual].alto,
			1800, miPersonaje.HojaSprite.ancho, 2, 2);


		/*DibujaHitbox(ptrBufferPixelsWindow, 0xFFFF00FF, ANCHO_VENTANA, ALTO_VENTANA,
			miPersonaje.HitboxX, miPersonaje.HitboxY, miPersonaje.HitboxAncho, miPersonaje.HitboxAlto,
			miPersonaje.HitboxScaleX, miPersonaje.HitboxScaleY);*/
	}
	else if (POSICIONPERSONAJE == false){

		mirrorxblt(ptrBufferPixelsWindow, (miPersonaje.HojaSprite.pixeles),
			miPersonaje.XCurrentCoordDraw, miPersonaje.YCurrentCoordDraw,
			miPersonaje.FrameSpriteArray[AnimacionActual][FrameActual].x, miPersonaje.FrameSpriteArray[AnimacionActual][FrameActual].y,
			miPersonaje.FrameSpriteArray[AnimacionActual][FrameActual].ancho, miPersonaje.FrameSpriteArray[AnimacionActual][FrameActual].alto,
			1800, miPersonaje.HojaSprite.ancho, 2, 2, 0xFF00FF000);

		/*DibujaHitbox(ptrBufferPixelsWindow, 0xFFFF00FF, ANCHO_VENTANA, ALTO_VENTANA,
			miPersonaje.HitboxX, miPersonaje.HitboxY, miPersonaje.HitboxAncho, miPersonaje.HitboxAlto,
			miPersonaje.HitboxScaleX, miPersonaje.HitboxScaleY);*/

	}

	


}




void ActualizaAnimacion(HWND hWnd) {
	switch (AnimacionActual) {

	case Idle:
		
		RECORERPANTALLA = false;
		if (TICK % TICK == 0 && FrameActual == 0)//125
		{
			DelayFrameAnimation++;
		}
		else if (TICK % TICK == 0 && FrameActual == 2)
		{
			DelayFrameAnimation += 1;
		}
		if (DelayFrameAnimation % 18 == 0)
		{
			FrameActual++;
			if (FrameActual > 5) FrameActual = 0;
		}


		FrameActualR++;

		if (FrameActualR > 5) {
			FrameActualR = 0;
		}

		FrameActualE++;

		if (FrameActualE > 5) {
			FrameActualE = 0;
		}

		break;

	case Running:

	
		//if (TICK % TICK == 0 && FrameActual == 2500)//125
		//{
		//	DelayFrameAnimation++;
		//}
		//else if (TICK % TICK == 0 && FrameActual == 2)
		//{
		//	DelayFrameAnimation += 0;
		//}
		if (DelayFrameAnimation % 18 == 0)
		{
			FrameActual++;
			if (FrameActual > 5)
				FrameActual = 0;
			AnimacionActual = Idle;


		}

	
		FrameActualR++;

		if (FrameActualR > 5) {
			FrameActualR = 0;
		}

		FrameActualE++;

		if (FrameActualE > 5) {
			FrameActualE = 0;
		}
	
		break;

	

	case Jump:

		if (TICK % TICK == 0 && FrameActual == 125)//125
		{
			DelayFrameAnimation++;
		}
		else if (TICK % TICK == 0 && FrameActual == 2)
		{
			DelayFrameAnimation += 0;
		}
	

		if (DelayFrameAnimation % 8 == 0)
		{
		
			FrameActual++;
			if (FrameActual > 5)
				FrameActual = 0;
			miPersonaje.velocityY = 0.0f;
			miPersonaje.AnimacionSaltando = false;
			AnimacionActual = fall;


		}

		
		FrameActualR++;

		if (FrameActualR > 5) {
			FrameActualR = 0;
		}
			

		FrameActualE++;

		if (FrameActualE > 5) {
			FrameActualE = 0;
		}
	

		/*miPersonaje.velocityY = 0.0f;
		miPersonaje.AnimacionSaltando = false;
		AnimacionActual = fall;*/
		break;

	case fall:

		if (DelayFrameAnimation % 18 == 0)
		{
			FrameActual++;
			if (FrameActual > 5)
				FrameActual = 0;
			AnimacionActual = Idle;


		}

		
		FrameActualR++;

		if (FrameActualR > 5) {
			FrameActualR = 0;
		}
			
		FrameActualE++;

		if (FrameActualE > 5) {
			FrameActualE = 0;
		}

		
		break;

		

		
	}
	InvalidateRect(hWnd, NULL, FALSE);
	UpdateWindow(hWnd);
}
void MainRender(HWND hWnd)
{
	LimpiarFondo(ptrBufferPixelsWindow, 0xFFFFFFFF, (ANCHO_VENTANA * ALTO_VENTANA));
	KeysEvents();
	DibujaPixeles();
	ActualizaAnimacion(hWnd);
}
void KeysEvents()
{
	if (!PANTALLADERROTA && PANTALLAVICTORIA && PANTALLAINICIAL)
	{
		if (KEYS[input.W] || KEYS[input.Up] || Gapad->GetState().Gamepad.wButtons == XINPUT_GAMEPAD_DPAD_UP || Gapad->GetState().Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			Letraint = (Letraint - 1 < A) ? Z : Letraint - 1;
		}
		if (KEYS[input.S] || KEYS[input.Down] || Gapad->GetState().Gamepad.wButtons == XINPUT_GAMEPAD_DPAD_DOWN || Gapad->GetState().Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			Letraint = (Letraint + 1 > Z) ? A : Letraint + 1;
		}
		if (KEYS[input.Z] || KEYS[input.Space] ||Gapad->GetState().Gamepad.wButtons == XINPUT_GAMEPAD_B)
		{
			if (Pletra)
			{
				PLetraAux[0] = static_cast<wchar_t>(Letraint);
				Pletra = false;
				Sletra = true;
			}
			else if (Sletra)
			{
				SLetraAux[0] = static_cast<wchar_t>(Letraint);
				Sletra = false;
				Tletra = true;
			}
			else if (Tletra)
			{
				TLetraAux[0] = static_cast<wchar_t>(Letraint);
				Pletra = false;
				Sletra = false;
				Tletra = false;

				Auxnombre[0] = PLetraAux[0];
				Auxnombre[1] = SLetraAux[0];
				Auxnombre[2] = TLetraAux[0];

				//InvalidateRect(hWnd, NULL, TRUE); // Actualiza la ventana
			}
			else {
				ReproductorCambiarCancionYReproduce(0);
				ReinicioMemoria();
			}
		}
	} else if (!PANTALLAINICIAL && !PANTALLADERROTA && !PANTALLAVICTORIA)
		{
		if (KEYS[input.W] || KEYS[input.Up] || Gapad->GetState().Gamepad.wButtons == XINPUT_GAMEPAD_DPAD_UP || Gapad->GetState().Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE || Gapad->GetState().Gamepad.wButtons == XINPUT_GAMEPAD_B)
		{
			///*miPersonaje.YCurrentCoordDraw -= 5;*/
			

			if (!miPersonaje.AnimacionSaltando) {
				AnimacionActual = Jump;
				miPersonaje.velocityY = JUMP_VELOCITY;
				miPersonaje.AnimacionSaltando = true;
				/*miPersonaje.YCurrentCoordDraw -= 5;*/

			}
		}

		if (KEYS[input.D] || KEYS[input.Right] || Gapad->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT || Gapad->GetState().Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			// Dentro del bucle de movimiento del personaje
			if (!haAlcanzadoMitad) {
				/*miPersonaje.XCurrentCoordDraw += 16;*/ // Mover el personaje hacia adelante
				startX += 5;
				POSICIONPERSONAJE = true;
				RECORERPANTALLA = true;
				AnimacionActual = Running;
				miPersonaje.XCurrentCoordDraw += 16;
				DelayFrameAnimation = 0;
			}
			else if (haAlcanzadoMitad) {
				POSICIONPERSONAJE = true;
				RECORERPANTALLA = true;
				AnimacionActual = Running;
				miPersonaje.XCurrentCoordDraw += 0;
				DelayFrameAnimation = 0;
			}

			/*	if (haAlcanzadoMitad)
				{
					startX += 5;
					POSICIONPERSONAJE = true;
					RECORERPANTALLA = true;
					AnimacionActual = Running;
					miPersonaje.XCurrentCoordDraw += 0;
					DelayFrameAnimation = 0;
				}*/


				/*startX += 5;
				POSICIONPERSONAJE = true;
				RECORERPANTALLA = true;
				AnimacionActual = Running;
				miPersonaje.XCurrentCoordDraw += 16;
				DelayFrameAnimation = 0;*/

		}
		if (KEYS[input.S] || KEYS[input.Down] || Gapad->GetState().Gamepad.wButtons == XINPUT_GAMEPAD_DPAD_DOWN || Gapad->GetState().Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{

		}
		if ((KEYS[input.A] || KEYS[input.Left] || Gapad->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT || Gapad->GetState().Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
		{
			int LIMITE_IZQUIERDO = 220;



			if (miPersonaje.XCurrentCoordDraw <= LIMITE_IZQUIERDO)
			{
				POSICIONPERSONAJE = false;
				haAlcanzadoMitad = false;
				RECORERPANTALLA = false;
				AnimacionActual = Idle;
				miPersonaje.XCurrentCoordDraw -= 0;
				DelayFrameAnimation = 0;
			}
			else {

				POSICIONPERSONAJE = false;
				haAlcanzadoMitad = false;
				RECORERPANTALLA = true;
				AnimacionActual = Running;
				miPersonaje.XCurrentCoordDraw -= 16;
				DelayFrameAnimation = 0;

			}


	/*		if (!PUNTOINICIAL)
			{
				POSICIONPERSONAJE = false;
				haAlcanzadoMitad = false;
				RECORERPANTALLA = true;
				AnimacionActual = Running;
				miPersonaje.XCurrentCoordDraw -= 16;
				DelayFrameAnimation = 0;
			}
			else if (PUNTOINICIAL)
			{
				POSICIONPERSONAJE = false;
				haAlcanzadoMitad = false;
				RECORERPANTALLA = false;
				AnimacionActual = Idle;
				miPersonaje.XCurrentCoordDraw -= 0;
				DelayFrameAnimation = 0;

			}*/

			


		} 
		
			/*if (KEYS[input.Enter] || KEYS[input.Space] || Gapad->GetState().Gamepad.wButtons == XINPUT_GAMEPAD_START)
			{
				PANTALLAINICIAL = 0;
				ReinicioMemoria();
				AnimacionActual = Idle;
			}
	*/

	} else if (PANTALLADERROTA && !PANTALLAVICTORIA && PANTALLAINICIAL) {

		if (KEYS[input.Enter] || KEYS[input.Space] || Gapad->GetState().Gamepad.wButtons == XINPUT_GAMEPAD_START)
		{
			PANTALLAINICIAL = 0;
			ReinicioMemoria();
			AnimacionActual = Idle;
		}

	} else if (PANTALLAINICIAL && !PANTALLADERROTA && !PANTALLAVICTORIA)
{
	//Puedes cambiar la tecla a lo que tu quieras puede ser enter o otra
	if (KEYS[input.Enter] || Gapad->GetState().Gamepad.wButtons == XINPUT_GAMEPAD_START)
	{

		PANTALLAINICIAL = false;
		Init();
		AnimacionActual = Idle;
	}
}

	////Puedes cambiar la tecla a lo que tu quieras puede ser enter o otra
	//if (KEYS[input.Enter]  || Gapad->GetState().Gamepad.wButtons == XINPUT_GAMEPAD_START)
	//{
	//	ReinicioMemoria();
	//	PANTALLAINICIAL = 0;
	//	Init();
	//	AnimacionActual = Idle;
	//}

}
#pragma region LENS_CODE
/* Pinta el fondo de la ventana de acuerdo al color especificado.
	@param *ptrBuffer.	Puntero al area de memoria reservada para el proceso de dibujado.
	@param color.		Color expresado en formato hexadecimal.
	@param area.		Area de la ventana.
	*/
void LimpiarFondo(int* ptrBuffer, unsigned int colorFondo, int area)
{
	__asm {
		mov edi, ptrBuffer
		mov ecx, area
		mov eax, colorFondo

		draw :
		mov[edi], eax
			add edi, BPP
			loop draw

			rep stosd
	}
}

/* Funcion que pinta una figura rectangular en pantalla.
	@param *ptrBuffer.	Puntero al area de memoria reservada para el dibujado.
	@param color.		Color de la figura expresado en formato hexadecimal.
	@param anchoWnd.	Ancho total de la ventana.
	@param altoWnd.		Alto total de la ventana.
	@param dmnFigura.	Especifica las dimensiones de la figura en relacion con la ventana.
	@param posFigura.	Posiciona la figura en la ventana.
	*/

void TranScaleblt(int* punteroDestino, int* punteroOrigen, int inicioXDestino, int inicioYDestino, int inicioXOrigen, int inicioYOrigen, int ancho, int alto, int anchodefondo, int anchodeorigen, int escalaX, int escalaY) {
	//blt = block transfer, transferencia de bloque de imagen
	int bytesporlineafondo = anchodefondo * 4;
	int bytesporlineaorigen = anchodeorigen * 4;
	int bytesporlineaimagen = ancho * 4;

	__asm {

		mov edi, punteroDestino //movemos la direccion del bitmap a edi, para poder escribir en el
		//Conseguimos el pixel inicial donde empezaremos a dibujar
		mov eax, inicioYDestino
		mul bytesporlineafondo //inicioY * bytesporlineafondo, asi iniciamos en la linea donde queremos 
		mov ebx, eax //ebx contendra el resultado anterior
		mov eax, 4
		mul inicioXDestino //inicioX*4, para asi encontrar la columna donde queremos empezar a dibujar
		add eax, ebx //posicion de columna + posicion de linea
		add edi, eax //Sumamos el desplazamiento anterior al inicio de nuestra imagen para empezar a trabajar en la posicion deseada
		mov esi, punteroOrigen //movemos la direccion de la imagen a dibujar a esi, para poder escribir de ella
		//Conseguimos el pixel inicial DEL CUAL empezaremos a dibujar
		mov eax, inicioYOrigen
		mul bytesporlineaorigen //inicioY * bytesporlineaorigen, asi iniciamos en la linea donde queremos 
		mov ebx, eax //ebx contendra el resultado anterior
		mov eax, 4
		mul inicioXOrigen //inicioX*4, para asi encontrar la columnda de donde queremos empezar a leer
		add eax, ebx //posicion de columna + posicion de linea
		add esi, eax //Sumamos el desplazamiento anterior al inicio de nuestra imagen para empezar a trabajar en la posicion deseada
		mov eax, [esi]
		mov ecx, alto //movemos a ecx la cantidad de lineas que dibujaremos
		lazollenarY :
		push ecx
			mov ecx, escalaY
			escaladoY :
		push ecx //guardamos el valor anterior de ecx, porque lo reemplazaremos en un nuevo ciclo
			mov ecx, ancho //la cantidad de columnas que dibujaremos
			lazollenarX :
		//mueve un pixel de la direccion apuntada por esi a la apuntada por edi, e incrementa esi y edi por 4
		push ecx
			mov ecx, escalaX
			escaladoX :
		cmp eax, [esi]
			je nodibujar
			mov edx, [esi]
			mov[edi], edx
			nodibujar :
		add edi, 4
			loop escaladoX
			add esi, 4
			pop ecx
			loop lazollenarX
			add edi, bytesporlineafondo //le sumamos la cantidad de bytes de la linea del fondo para pasar a la siguiente linea
			push eax
			mov eax, bytesporlineaimagen
			mul escalaX
			sub edi, eax //y retrocedemos una cantidad igual al a su ancho para dibujar desde la posicion X inicial y que no quede escalonado
			pop eax
			sub esi, bytesporlineaimagen
			pop ecx
			loop escaladoY
			//Lo mismo para esi
			add esi, bytesporlineaorigen
			pop ecx //recuperamos el valor del contador del ciclo exterior
			loop lazollenarY
	}
}

void TranScalebltRings(int* punteroDestino, int* punteroOrigen, int inicioXDestino, int inicioYDestino, int inicioXOrigen, int inicioYOrigen, int ancho, int alto, int anchodefondo, int anchodeorigen, int escalaX, int escalaY) {

	//blt = block transfer, transferencia de bloque de imagen
	int bytesporlineafondo = anchodefondo * 4;
	int bytesporlineaorigen = anchodeorigen * 4;
	int bytesporlineaimagen = ancho * 4;

	__asm {

		mov edi, punteroDestino //movemos la direccion del bitmap a edi, para poder escribir en el
		//Conseguimos el pixel inicial donde empezaremos a dibujar
		mov eax, inicioYDestino
		mul bytesporlineafondo //inicioY * bytesporlineafondo, asi iniciamos en la linea donde queremos 
		mov ebx, eax //ebx contendra el resultado anterior
		mov eax, 4
		mul inicioXDestino //inicioX*4, para asi encontrar la columna donde queremos empezar a dibujar
		add eax, ebx //posicion de columna + posicion de linea
		add edi, eax //Sumamos el desplazamiento anterior al inicio de nuestra imagen para empezar a trabajar en la posicion deseada
		mov esi, punteroOrigen //movemos la direccion de la imagen a dibujar a esi, para poder escribir de ella
		//Conseguimos el pixel inicial DEL CUAL empezaremos a dibujar
		mov eax, inicioYOrigen
		mul bytesporlineaorigen //inicioY * bytesporlineaorigen, asi iniciamos en la linea donde queremos 
		mov ebx, eax //ebx contendra el resultado anterior
		mov eax, 4
		mul inicioXOrigen //inicioX*4, para asi encontrar la columnda de donde queremos empezar a leer
		add eax, ebx //posicion de columna + posicion de linea
		add esi, eax //Sumamos el desplazamiento anterior al inicio de nuestra imagen para empezar a trabajar en la posicion deseada
		mov eax, [esi]
		mov ecx, alto //movemos a ecx la cantidad de lineas que dibujaremos
		lazollenarY :
		push ecx
			mov ecx, escalaY
			escaladoY :
		push ecx //guardamos el valor anterior de ecx, porque lo reemplazaremos en un nuevo ciclo
			mov ecx, ancho //la cantidad de columnas que dibujaremos
			lazollenarX :
		//mueve un pixel de la direccion apuntada por esi a la apuntada por edi, e incrementa esi y edi por 4
		push ecx
			mov ecx, escalaX
			escaladoX :
		cmp eax, [esi]
			je nodibujar
			mov edx, [esi]
			mov[edi], edx
			nodibujar :
		add edi, 4
			loop escaladoX
			add esi, 4
			pop ecx
			loop lazollenarX
			add edi, bytesporlineafondo //le sumamos la cantidad de bytes de la linea del fondo para pasar a la siguiente linea
			push eax
			mov eax, bytesporlineaimagen
			mul escalaX
			sub edi, eax //y retrocedemos una cantidad igual al a su ancho para dibujar desde la posicion X inicial y que no quede escalonado
			pop eax
			sub esi, bytesporlineaimagen
			pop ecx
			loop escaladoY
			//Lo mismo para esi
			add esi, bytesporlineaorigen
			pop ecx //recuperamos el valor del contador del ciclo exterior
			loop lazollenarY
	}


}

void mirrorxblt(int* punteroDestino,//Buffer donde comenzara a pasar los datos
	int* punteroOrigen, // Buffer que copiara los datos del bmp
	int inicioXDestino,//ventana
	int inicioYDestino,
	int inicioXOrigen,//bmp
	int inicioYOrigen,
	int ancho,//ancho del sprite
	int alto,//alto del sprite
	int anchodefondo,//dimensiones
	int anchodeorigen,
	int escalaX,//tamaño de escala X bmp
	int escalaY,//tamaño de escala Y bmp
	const unsigned int TRANSPARENCY) {//Color Transparencia


	//blt = block transfer, transferencia de bloque de imagen
	int bytesporlineafondo = anchodefondo * 4;//fondo-destino total
	int bytesporlineaorigen = anchodeorigen * 4;//bmp total
	int bytesporlineaimagen = ancho * 4;

	__asm {

		mov edi, punteroDestino //movemos la direccion del bitmap a edi, para poder escribir en el
		//Conseguimos el pixel inicial donde empezaremos a dibujar
		mov eax, inicioYDestino
		mul bytesporlineafondo //inicioY * bytesporlineafondo, asi iniciamos en la linea donde queremos 
		mov ebx, eax //ebx contendra el resultado anterior
		mov eax, 4
		mul inicioXDestino //inicioX*4, para asi encontrar la columnda donde queremos empezar a dibujar
		add eax, ebx //posicion de columna + posicion de linea
		add edi, eax //Sumamos el desplazamiento anterior al inicio de nuestra imagen para empezar a trabajar en la posicion deseada

		mov esi, punteroOrigen //movemos la direccion de la imagen a dibujar a esi, para poder escribir de ella
		//Conseguimos el pixel inicial DEL CUAL empezaremos a dibujar
		mov eax, inicioYOrigen
		mul bytesporlineaorigen //inicioY * bytesporlineaorigen, asi iniciamos en la linea donde queremos 
		mov ebx, eax //ebx contendra el resultado anterior
		mov eax, 4
		mul inicioXOrigen //inicioX*4, para asi encontrar la columnda de donde queremos empezar a leer
		add eax, ebx //posicion de columna + posicion de linea
		add esi, eax //Sumamos el desplazamiento anterior al inicio de nuestra imagen para empezar a trabajar en la posicion deseada


		mov eax, TRANSPARENCY
		mov ecx, alto //movemos a ecx la cantidad de lineas que dibujaremos
		lazollenarY :
		push ecx
			mov ecx, escalaY//es como el ancho
			sub esi, 4
			escaladoY :
			push ecx //guardamos el valor anterior de ecx, porque lo reemplazaremos en un nuevo ciclo
			mov ecx, ancho //la cantidad de columnas que dibujaremos
			add esi, bytesporlineaimagen

			lazollenarX :
		//movsd //mueve un pixel de la direccion apuntada por esi a la apuntada por edi, e incrementa esi y edi por 4
		push ecx
			mov ecx, escalaX
			escaladoX :
		cmp eax, [esi]
			je nodibujar
			mov edx, [esi]
			mov[edi], edx
			nodibujar :
		add edi, 4
			loop escaladoX
			sub esi, 4
			pop ecx
			loop lazollenarX
			add edi, bytesporlineafondo //le sumamos la cantidad de bytes de la linea del fondo para pasar a la siguiente linea
			push eax
			mov eax, escalaX
			mul bytesporlineaimagen
			mov ebx, eax
			pop eax
			sub edi, ebx //y retrocedemos una cantidad igual al a su ancho para dibujar desde la posicion X inicial y que no quede escalonado
			//sub edi, bytesporlineaimagen //y retrocedemos una cantidad igual al a su ancho para dibujar desde la posicion X inicial y que no quede escalonado

			pop ecx
			loop escaladoY
			//Lo mismo para esi
			add esi, bytesporlineaorigen
			add esi, 4
			//sub esi, bytesporlineaimagen
			pop ecx //recuperamos el valor del contador del ciclo exterior
			loop lazollenarY
	}

}


void DibujaHitbox(int* ptrBuffer, unsigned int color, int anchoWnd, int altoWnd, int HitboxX, int HitboxY, int HitboxAncho, int HitboxAlto, int escalaX, int escalaY)
{
	HitboxAncho = HitboxAncho * escalaX;
	HitboxAlto = HitboxAlto * escalaY;
	__asm {
		mov edi, ptrBuffer

		mov eax, HitboxY
		mul BPP
		mul anchoWnd
		add edi, eax

		mov eax, HitboxX
		mul BPP
		add edi, eax

		mov eax, color

		mov ecx, HitboxAlto
		lazo_alto :
		push ecx
			mov ecx, HitboxAncho
			lazo_ancho :
		mov[edi], eax
			add edi, BPP
			//stosd
			loop lazo_ancho
			push eax
			mov eax, anchoWnd
			sub eax, HitboxAncho
			mul BPP
			add edi, eax
			pop eax
			pop ecx
			loop lazo_alto
	}
}

void ReinicioMemoria() {
	PANTALLAINICIAL = true;
	RECORERPANTALLA = false;
	PANTALLADERROTA = false;
	PANTALLACOMOJUGAR = false;
	POSICIONPERSONAJE = true;
	PANTALLAVICTORIA = false;
	haAlcanzadoMitad = false;
	PUNTOINICIAL = false;
	colisionmoneda = false;
	coliionpinchos = false;
	RegName = false;
	Pletra = true;
	Sletra = false;
	Tletra = false;
	Letraint = A;
	puntos = 2500;
	RINGS = 0;
	countdownSeconds = 120;
	miRings.HitboxX = 0;
	Enemigo.HitboxX = 0;


	__asm {
		xor eax, eax
		xor ebx, ebx
		xor ecx, ecx
		xor edx, edx
		xor esi, esi
		xor edi, edi
	}
	ReproductorCambiarCancionYReproduce(1);

}

#pragma endregion



bool DetectarColision(int x1, int y1, int ancho1, int alto1, int x2, int y2, int ancho2, int alto2) {
	bool colisionn = (x1 < (x2 + ancho2)) && ((x1 + ancho1) > x2) && (y1 < (y2 + alto2)) && (y1 + alto1 > y2);
	return colisionn;
}






void ReproductorInicializaYReproduce() {
	Cancion[0].Nombre = "Inicio";
	Cancion[0].Dir = "Recursos/Soniclvl1.mp3";
	Cancion[1].Nombre = "Title Song";
	Cancion[1].Dir = "Recursos/Title.mp3";
	Cancion[2].Nombre = "Derrota";
	Cancion[2].Dir = "Recursos/GameOver.mp3";
	ifstream inputFile(Cancion[0].Dir.c_str());

	if (!inputFile.good())
		printf("No file found");
	else
  		player->OpenFile(Cancion[0].Dir.c_str(), sfAutodetect);
	player->Play();
}

void ReproductorCambiarCancionYReproduce(int NumeroCancionAeproducir) {
	player->Stop();
	ifstream inputFile(Cancion[NumeroCancionAeproducir].Dir.c_str());

	if (!inputFile.good())
		printf("No file found");
	else
		player->OpenFile(Cancion[NumeroCancionAeproducir].Dir.c_str(), sfAutodetect);
	player->Play();
}

void ReproductorPausa() {
	player->Pause();
	pausa = true;
}

void ReproductorReproduce() {
	if (pausa)
		player->Resume();
	else
		player->Play();
	pausa = false;


}

void ReproductorEfecto(string dir) {
	ifstream inputFile(dir.c_str());

	if (!inputFile.good()) {
		printf("No se encontró el archivo de efecto de sonido");
	}
	else {
		// Detiene cualquier reproducción existente antes de cargar y reproducir el efecto de sonido
		playerEfecto->Stop();
		playerEfecto->OpenFile(dir.c_str(), sfAutodetect);
		playerEfecto->Play();
	}
}
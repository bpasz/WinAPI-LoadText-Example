#include <Windows.h>
#include <string>
#include <tchar.h>
#include <stdio.h>

LPSTR NazwaKlasy = "Klasa Okienka";
MSG Komunikat;

LRESULT CALLBACK ObslugaZdarzen(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void LoadFile(LPCTSTR FileName);
std::string GetLastErrorAsString();

HWND g_LoadButton;
HWND g_Text;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	
	// WYPE£NIANIE STRUKTURY
	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = ObslugaZdarzen;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = NazwaKlasy;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	// REJESTROWANIE KLASY OKNA
	RegisterClassEx(&wc);
	
	HWND hwnd; //Tworzenie okna
	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, NazwaKlasy, "Oto okienko", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL);

	g_LoadButton = CreateWindowEx(0, "BUTTON", "Laduj Tekst", WS_CHILD | WS_VISIBLE,
		50, 30 , 150, 30, hwnd, NULL, hInstance, NULL);

	g_Text = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER |
		WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL, 250 , 5 , 150, 150, hwnd, NULL, hInstance, NULL);

	ShowWindow(hwnd, nCmdShow); // Poka¿ okienko...
	UpdateWindow(hwnd);


	DWORD Result;
	LPCTSTR FileName = TEXT("testowy.plik");
	TCHAR lpBuffer[4096] = TEXT("");
	TCHAR** lpFilePart = { NULL };

	Result = GetFullPathName(FileName, 4096, lpBuffer, lpFilePart);

	while (GetMessage(&Komunikat, NULL, 0, 0))
	{
		TranslateMessage(&Komunikat);
		DispatchMessage(&Komunikat);
	}

	return Komunikat.wParam;
}


LRESULT CALLBACK ObslugaZdarzen(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_COMMAND:
		if ((HWND) lParam == g_LoadButton)
		{

			LoadFile("test.txt");
			
			//Prezentacja sciezki do programu
			TCHAR szFileName[MAX_PATH];
			GetModuleFileName(NULL, szFileName, MAX_PATH);
			MessageBox(hwnd, szFileName, "Sciezka", MB_ICONEXCLAMATION | MB_OK);
	}
		break;

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return 0;
}

void LoadFile(LPCTSTR FileName) {
	LPSTR Bufor;
	DWORD dwRozmiar, dwPrzeczyt;
	HANDLE hPlik;

	hPlik = CreateFile((LPCTSTR) FileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	GetLastErrorAsString();

	TCHAR Sciezka[4096] = TEXT("");
	GetFinalPathNameByHandle(hPlik, Sciezka, 4096, FILE_NAME_NORMALIZED);
	MessageBox(NULL, Sciezka, "Sciezka do pliku", MB_OK);

	/*DWORD WINAPI GetFinalPathNameByHandle(
		_In_  HANDLE hFile,
		_Out_ LPTSTR lpszFilePath,
		_In_  DWORD  cchFilePath,
		_In_  DWORD  dwFlags
	);*/


	if (hPlik == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, "Nie mo¿na otworzyæ pliku.", "A to pech!", MB_ICONEXCLAMATION);
		PostQuitMessage(0); // Zakoñcz program
	}

	dwRozmiar = GetFileSize(hPlik, NULL);
	GetLastErrorAsString();
	if (dwRozmiar == 0xFFFFFFFF) {
		MessageBox(NULL, "Nieprawid³owy rozmiar pliku!", "Niedobrze...", MB_ICONEXCLAMATION);
		PostQuitMessage(0); // Zakoñcz program
	}

	Bufor = (LPSTR)GlobalAlloc(GPTR, dwRozmiar + 1);
	if (Bufor == NULL) {
		MessageBox(NULL, "Za ma³o pamiêci!", "Ale wiocha...", MB_ICONEXCLAMATION);
		PostQuitMessage(0); // Zakoñcz program
	}

	if (!ReadFile(hPlik, Bufor, dwRozmiar, &dwPrzeczyt, NULL)) {
		MessageBox(NULL, "B³¹d czytania z pliku", "Dupa blada!", MB_ICONEXCLAMATION);
		PostQuitMessage(0); // Zakoñcz program
	}
	GetLastErrorAsString();
	Bufor[dwRozmiar] = 0; // dodaj zero na koñcu stringa
	SetWindowText(g_Text, Bufor); // zrób coœ z tekstem, np. wyœwietl go
	MessageBox(NULL, Bufor, "Odczytano", MB_OK);
	GlobalFree(Bufor); // Zwolnij bufor
	CloseHandle(hPlik); // Zamknij plik
}


std::string GetLastErrorAsString()
{
	//Get the error message, if any.
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0)
		return std::string(); //No error message has been recorded

	LPSTR messageBuffer = nullptr;
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	std::string message(messageBuffer, size);

	//Free the buffer.
	LocalFree(messageBuffer);
	MessageBox(NULL,messageBuffer , "Error", MB_OK);
	return message;
}
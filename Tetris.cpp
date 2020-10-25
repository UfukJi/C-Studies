#include <iostream>
#include <thread>
#include <vector>
using namespace std;

#include <stdio.h>
#include <Windows.h>

//creating storage for assests 
wstring tetromino[7];
int nFieldWidth = 12;
int nFieldHeight = 18;
unsigned char* pField = nullptr;

int nScreenWidth = 120; // console screen size x (columns)
int nScreenHeight = 30; // console screen size y (rows)




//returns appropriate index
int rotate(int px, int py, int r) {
	switch (r % 4){

	case 0: return py * 4 + px; // 0 degrees
	case 1: return 12 + py - (px * 4); // 90 degrees
	case 2: return 15 - (py * 4) - px; // 180 degrees
	case 3: return 3 - py + (px * 4); // 270 
	}
	return 0;
}

bool doesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY) {

	for (int px = 0; px < 4; px++)
		for (int py = 0; py < 4; py++) {

			// Get index into piece
			int pi = rotate(px, py, nRotation);

			// Get index into field
			int fi = (nPosY + py) * (nFieldWidth)+(nPosX + px);

			if (nPosX + px >= 0 && nPosX + px < nFieldWidth) {
				if (nPosY + py >= 0 && nPosY + py < nFieldHeight) {
					if(tetromino[nTetromino][pi] == L'X' && pField[fi] != 0)
						return false; //fail on first hit
				}
			}

		}

	return true;
}


int main() {

	//Creates assests

	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");

	tetromino[1].append(L"..X.");
	tetromino[1].append(L".XX.");
	tetromino[1].append(L".X..");
	tetromino[1].append(L"....");

	tetromino[2].append(L".X..");
	tetromino[2].append(L".XX.");
	tetromino[2].append(L"..X.");
	tetromino[2].append(L"....");

	tetromino[3].append(L"....");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L"....");

	tetromino[4].append(L"..X.");
	tetromino[4].append(L".XX.");
	tetromino[4].append(L"..X.");
	tetromino[4].append(L"....");

	tetromino[5].append(L"....");
	tetromino[5].append(L".XX.");
	tetromino[5].append(L"..X.");
	tetromino[5].append(L"..X.");

	tetromino[6].append(L"..X.");
	tetromino[6].append(L".XX.");
	tetromino[6].append(L".X..");
	tetromino[6].append(L".X..");

	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
	for (int i = 0; i < nScreenWidth * nScreenHeight; i++) screen[i] = L' ';
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	pField = new unsigned char[nFieldWidth * nFieldHeight]; // Create play field buffer
	for (int x = 0; x < nFieldWidth; x++) // Board Boundary
		for (int y = 0; y < nFieldHeight; y++)
			pField[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;

	


	// game logic variables
	

	int nCurrentPiece = 6;
	int nCurrentRotation = 0;
	int nCurrentX = nFieldWidth / 2;
	int nCurrentY = 0;

	bool bGameOver = false;
	bool bKey[4]; //arrow keys will be stored
	bool bRotateHold = false;

	int nSpeed = 20;
	int nSpeedCounter = 0;
	bool bForceDown = false;
	int nPieceCount = 0;
	int nScore = 0;

	vector<int> vLines;
	

	while (!bGameOver) {

		// game timing =============

		this_thread::sleep_for(50ms);
		nSpeedCounter++;
		bForceDown = (nSpeedCounter == nSpeed);

		// input ===================

		for (int k = 0; k < 4; k++)								  //R  L   D  Z
			bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;

		// game logic ==============
		
		nCurrentX += (bKey[0]) && (doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) ? 1 : 0;
		nCurrentX -= (bKey[1]) && (doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)) ? 1 : 0;
		nCurrentY += (bKey[2]) && (doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) ? 1 : 0;
	
		if (bKey[3]){
			nCurrentRotation += (!bRotateHold && doesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY)) ? 1 : 0;
			bRotateHold = true;
		}
		else
			bRotateHold = false;
		
		if (bForceDown) {
			if (doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
				nCurrentY++; // go down
			else {
				// lock the current piece in the field

				for (int px = 0; px < 4; px++)
					for (int py = 0; py < 4; py++)
						if (tetromino[nCurrentPiece][rotate(px, py, nCurrentRotation)] == L'X')
							pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;
							
				nPieceCount++;
				if (nPieceCount % 10 == 0)
					if (nSpeed >= 10) nSpeed--;

				// check lines

				for (int py = 0; py < 4; py++)
					if (nCurrentY + py < nFieldHeight - 1) {
						bool bLine = true;
						for (int px = 1; px < nFieldWidth - 1; px++)
							bLine &= (pField[(nCurrentY + py) * nFieldWidth + px]) != 0;

						if (bLine) {
							// removes line, set to =
							for (int px = 1; px < nFieldWidth - 1; px++)
								pField[(nCurrentY + py) * nFieldWidth + px] = 8;

							vLines.push_back(nCurrentY + py);
						}
					}

				nScore += 25;
				if (!vLines.empty()) nScore += (1 << vLines.size()) * 100;

				// choose next piece

				nCurrentX = nFieldWidth / 2;
				nCurrentY = 0;
				nCurrentRotation = 0;
				nCurrentPiece = rand() % 7;

				// game over
				bGameOver = !doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
					
			}

			nSpeedCounter = 0;
		}

		// render outpt ============

		//draw field
		for (int x = 0; x < nFieldWidth; x++)
			for (int y = 0; y < nFieldHeight; y++)
				screen[(y + 2) * nScreenWidth + (x + 2)] = L" ABCDEFG=#"[pField[y * nFieldWidth + x]];
		
		// draw current piece
		for (int px = 0; px < 4; px++)
			for (int py = 0; py < 4; py++)
				if (tetromino[nCurrentPiece][rotate(px, py, nCurrentRotation)] == L'X')
					screen[(nCurrentY + py + 2) * nScreenWidth + (nCurrentX + px + 2)] = nCurrentPiece + 65;
		
		if (!vLines.empty()) {

			// display frame
			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
			this_thread::sleep_for(400ms); 

			for (auto &v : vLines)
				for (int px = 1; px < nFieldWidth - 1; px++) {
					for (int py = v; py > 0; py--)
						pField[py * nFieldWidth + px] = pField[(py - 1) * nFieldWidth + px];
					pField[px] = 0;
				}
			vLines.clear();
		}

		// draw score

		swprintf_s(&screen[2*nScreenWidth + nFieldHeight + 6], 16, L"SCORE:	%8d", nScore);

		// display frame
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
	}

	// game over text
	CloseHandle(hConsole);
	cout << "Game Over!! Score:" << nScore << endl;
	system("pause");

	return 0;
}
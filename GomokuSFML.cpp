#include <SFML/Graphics.hpp>
#include <time.h>
#include <conio.h>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <codecvt>
#define N 15	// 棋盤大小

using namespace sf;
using namespace std;

wstring stringToWString(const string&);	// string轉wstring
void Welcome();							// 遊戲開始介面
void Co_op();							// 雙人對戰
void Computer();						// 電腦對戰
void WriteCBoardChess();				// 繪製棋盤棋子
void Regret();							// 悔棋
int EmptyChess(int, int);				// 判斷棋盤是否有棋子
int GameJudge(int, int, int);			// 判斷輸贏
void MachineCount();					// 計算電腦落子位置
void MachineAttack();					// 計算電腦進攻權值
void MachineDefend();					// 計算電腦防守權值
void FindBestPosition();				// 找到權重最高的位置
void Reset();							// 重置棋盤
void WaringOwnChess(int, int);			// 警告玩家該格子擁有棋子
void Gametips();						// 遊戲說明
void Updatediary();						// 更新日誌
int PauseGame();						// 暫停

int status[N][N] = { { 0 },{ 0 } };					// 記錄棋盤情況 0為無子 1為紅子|玩家 2為白子|電腦 
int gameWinner = 0;									// 輸贏狀態
int computerPosition[2] = { 292, 292 };				// 電腦落子位置
int attackValue[N][N] = { { 0 },{ 0 } };			// 儲存進攻權值
int defendValue[N][N] = { { 0 },{ 0 } };			// 儲存防守權值
int playerRegretPosition[2][2] = { { 0 }, { 0 } },	// 玩家悔棋座標
	machineRegretPosition[2] = { 0 };				// 電腦悔棋座標
int gameRound = 1,									// 計算回合數
	chessCount = 0;									// 計算棋盤上棋子數量

int windowWidth = 900, windowHeight = 615;    // 視窗大小

RenderWindow GomokuWindow(VideoMode(windowWidth, windowHeight), L"五子棋");	// 創建遊戲視窗

Image icon; // 遊戲視窗Icon
Font font; // 字型
Texture bg_main, bg_gameStart, bg_gametips, bg_coop, bg_computer, bg_pauseGame, bg_updatediary;	// 背景
Texture playerARect, playerBRect, blockStone, whiteStone, gameEnd;								// 遊戲
Texture waringOwnChess;																			// 警告

struct keyboardList	// 鍵盤按鍵
{
	//
	//	玩家A按鍵
	//
	Keyboard::Key playerA_Up = Keyboard::W;				// 向上移動
	Keyboard::Key playerA_Down = Keyboard::S;			// 向下移動
	Keyboard::Key playerA_Left = Keyboard::A;			// 向左移動
	Keyboard::Key playerA_Right = Keyboard::D;			// 向右移動
	Keyboard::Key playerA_dropChess = Keyboard::Space;	// 放置棋子
	Keyboard::Key playerA_regret = Keyboard::Q;			// 悔棋
	//
	//	玩家B按鍵
	//
	Keyboard::Key playerB_Up = Keyboard::Up;			// 向上移動
	Keyboard::Key playerB_Down = Keyboard::Down;		// 向下移動
	Keyboard::Key playerB_Left = Keyboard::Left;		// 向左移動
	Keyboard::Key playerB_Right = Keyboard::Right;		// 向右移動
	Keyboard::Key playerB_dropChess = Keyboard::Num0;	// 放置棋子
	Keyboard::Key playerB_regret = Keyboard::B;			// 悔棋
}key;	// 鍵盤按鍵

wstring stringToWString(const string& str) {
	wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;
	return converter.from_bytes(str);
}

void Welcome()	// 遊戲開始介面
{
    int KeyPressed[6] = { 0 };
	while (true) {
		Sprite sBackground(bg_main);	// 背景
		GomokuWindow.clear();			// 清除畫面
		sBackground.setPosition(0, 0);	// 設定背景位置
		GomokuWindow.draw(sBackground);	// 繪製背景
		GomokuWindow.display();			// 顯示畫面
		//
		// 事件處理
		//
		Event event;
		while (GomokuWindow.pollEvent(event))
		{
			switch (event.type)
			{
				// 
				// 關閉視窗
				//
			case Event::Closed:
				GomokuWindow.close();
				break;
				//
				// 按下鍵盤按鍵
				//
            case Event::KeyPressed:
                // 雙人對戰
                if (Keyboard::isKeyPressed(Keyboard::Num1)) KeyPressed[1] = 1;
                // 電腦對戰
                if (Keyboard::isKeyPressed(Keyboard::Num2)) KeyPressed[2] = 1;
                // 遊戲說明
                if (Keyboard::isKeyPressed(Keyboard::Num3)) KeyPressed[3] = 1;
                // 更新日誌
                if (Keyboard::isKeyPressed(Keyboard::Num4)) KeyPressed[4] = 1;
                // 離開遊戲
                if (Keyboard::isKeyPressed(Keyboard::Num5)) KeyPressed[5] = 1;
                break;
				//
				// 鬆開鍵盤按鍵
				//
            case Event::KeyReleased:
				if (event.key.code == Keyboard::Num1 && KeyPressed[1]) { KeyPressed[1] = 0; Co_op(); goto end; }
				if (event.key.code == Keyboard::Num2 && KeyPressed[2]) { KeyPressed[2] = 0; Computer(); goto end; }
                if (event.key.code == Keyboard::Num3 && KeyPressed[3]) { KeyPressed[3] = 0; Gametips(); goto end; }
                if (event.key.code == Keyboard::Num4 && KeyPressed[4]) { KeyPressed[4] = 0; Updatediary(); goto end; }
                if (event.key.code == Keyboard::Num5 && KeyPressed[5]) { KeyPressed[5] = 0; GomokuWindow.close(); goto end; }
                break;
				//
				// 預設
				//
			default:
				break;
			}
		}
	}
    end:
        return;
}

void Co_op()	// 雙人對戰
{
	Reset();	// 重置棋盤
	int KeyPressed[3] = { 0 },
		ESCPressed = 0;
	while (true)
	{
		Sprite sBackground(bg_coop);	// 背景
		GomokuWindow.clear();			// 清除畫面
		sBackground.setPosition(0, 0);	// 設定背景位置
		GomokuWindow.draw(sBackground);	// 繪製背景
		GomokuWindow.display();			// 顯示畫面
		//
		// 事件處理
		//
		Event event;
		while (GomokuWindow.pollEvent(event))
		{
			switch (event.type)
			{
				// 
				// 關閉視窗
				//
			case Event::Closed:
				GomokuWindow.close();
				break;
				//
				// 按下鍵盤按鍵
				//
			case Event::KeyPressed:
				// 紅子先手
				if (Keyboard::isKeyPressed(Keyboard::Num1)) KeyPressed[1] = 1;
				// 白子先手
				if (Keyboard::isKeyPressed(Keyboard::Num2)) KeyPressed[2] = 1;
				// 按下ESC返回
				if (Keyboard::isKeyPressed(Keyboard::Escape)) ESCPressed = 1;
				break;
				//
				// 鬆開鍵盤按鍵
				//
			case Event::KeyReleased:
				if (event.key.code == Keyboard::Num1 && KeyPressed[1]) { if (KeyPressed[2]) KeyPressed[2] = 0; goto startGame; }
				if (event.key.code == Keyboard::Num2 && KeyPressed[2]) { if (KeyPressed[1]) KeyPressed[1] = 0; goto startGame; }
				if (event.key.code == Keyboard::Escape && ESCPressed) { ESCPressed = 0; return; }
				break;
				//
				// 預設
				//
			default:
				break;
			}
		}
	}
startGame:
	int control = 0;	// 控制目前執手
	int playerPosition[2][2] = { { 292, 292 }, { 292, 292 } };
	if (KeyPressed[1]) { control = 1; KeyPressed[1] = 0; }
	else if (KeyPressed[2]) { control = -1; KeyPressed[2] = 0; }
	int pauseKey = 0,			// 暫停按鍵
		placeKey[2] = { 0 },	// 落子按鍵
		regretKey[2] = { 0 };	// 悔棋按鍵
	int pauseGame = 0;			// 暫停遊戲
	int regretLicense = 1;		// 悔棋許可
	while (gameWinner == 0)
	{
		Sprite sBackground(bg_gameStart);	// 背景
		GomokuWindow.clear();				// 清除畫面
		sBackground.setPosition(0, 0);		// 設定背景位置
		GomokuWindow.draw(sBackground);		// 繪製背景

		WriteCBoardChess();	// 繪製棋盤上棋子 
		// 繪製棋盤資訊
		{
			string s;
			wstring ws;
			//
			// 繪製遊戲模式
			//
			Text text(L"雙人對戰", font);			// 繪製遊戲模式
			text.setCharacterSize(50);				// 設定文字大小
			text.setFillColor(Color::Black);		// 設定文字顏色
			text.setStyle(Text::Bold);				// 設定文字樣式
			text.setPosition(655, 50);				// 設定文字位置
			GomokuWindow.draw(text);				// 繪製文字
			//
			// 繪製回合數
			//
			text.setString(L"回合數");				// 繪製回合數
			text.setCharacterSize(30);				// 設定文字大小
			text.setPosition(710, 200);				// 設定文字位置
			GomokuWindow.draw(text);				// 繪製文字
			//
			// 繪製回合數字
			//
			if (gameRound < 10) { s += "00"; s += to_string(gameRound); }
			else if (gameRound < 100) { s += "0"; s += to_string(gameRound); }
			else s += to_string(gameRound);
			ws = stringToWString(s);
			text.setString(ws);						// 繪製回合數字
			text.setCharacterSize(30);				// 設定文字大小
			text.setPosition(730, 240);				// 設定文字位置
			GomokuWindow.draw(text);				// 繪製文字
			//
			// 執手狀態
			//
			if (control == 1)
			{
				text.setString(L"黑子執手");		// 繪製回合數
				text.setFillColor(Color::Magenta);		// 設定文字顏色
			}
			else
			{
				text.setString(L"白子執手");		// 繪製回合數
				text.setFillColor(Color::White);	// 設定文字顏色
			}
			text.setCharacterSize(30);				// 設定文字大小
			text.setPosition(695, 360);				// 設定文字位置
			GomokuWindow.draw(text);				// 繪製文字
		}

		Sprite sBlock(playerARect);		// 黑子執手
		Sprite sWhite(playerBRect);		// 白子執手
		Sprite sBlockStone(blockStone);	// 黑子
		Sprite sWhiteStone(whiteStone);	// 白子

		gameRound = chessCount / 2 + 1;	// 計算回合數

		if (pauseGame == -1) goto end;	// 回主選單

		if (control == 1)
		{	
			sBlock.setPosition((float)playerPosition[0][0], (float)playerPosition[0][1]);	// 設定黑子瞄準位置
			GomokuWindow.draw(sBlock);														// 繪製黑子瞄準格子
			GomokuWindow.display();															// 顯示畫面
		}
		else if (control == -1)
		{
			sWhite.setPosition((float)playerPosition[1][0], (float)playerPosition[1][1]);	// 設定白子瞄準位置
			GomokuWindow.draw(sWhite);														// 繪製白子瞄準格子
			GomokuWindow.display();															// 顯示畫面
		}
		//
		// 事件處理
		//
		Event event;
		while (GomokuWindow.pollEvent(event))
		{
			switch (event.type)
			{
				// 
				// 關閉視窗
				//
			case Event::Closed:
				GomokuWindow.close();
				goto end;
				break;

				//
				// 按下鍵盤按鍵
				//
			case Event::KeyPressed:
				//
				// 遊戲
				//
				if (Keyboard::isKeyPressed(Keyboard::Escape)) pauseKey = 1;
				//
				// 玩家A
				// 
				if (control == 1)
				{
					// 玩家A向上
					if (Keyboard::isKeyPressed(key.playerA_Up)) { playerPosition[0][1] -= 38; if (playerPosition[0][1] < 26) playerPosition[0][1] = 26; }
					// 玩家A向下
					if (Keyboard::isKeyPressed(key.playerA_Down)) { playerPosition[0][1] += 38; if (playerPosition[0][1] > 558) playerPosition[0][1] = 558; }
					// 玩家A向左
					if (Keyboard::isKeyPressed(key.playerA_Left)) { playerPosition[0][0] -= 38; if (playerPosition[0][0] < 26) playerPosition[0][0] = 26; }
					// 玩家A向右
					if (Keyboard::isKeyPressed(key.playerA_Right)) { playerPosition[0][0] += 38; if (playerPosition[0][0] > 558) playerPosition[0][0] = 558; }
					// 玩家A落子
					if (Keyboard::isKeyPressed(key.playerA_dropChess)) placeKey[0] = 1;
					// 玩家A悔棋
					if (Keyboard::isKeyPressed(key.playerA_regret)) regretKey[0] = 1;
				}
				//
				// 玩家B
				// 
				if (control == -1)
				{
					// 玩家B向上
					if (Keyboard::isKeyPressed(key.playerB_Up)) { playerPosition[1][1] -= 38; if (playerPosition[1][1] < 26) playerPosition[1][1] = 26; }
					// 玩家B向下
					if (Keyboard::isKeyPressed(key.playerB_Down)) { playerPosition[1][1] += 38; if (playerPosition[1][1] > 558) playerPosition[1][1] = 558; }
					// 玩家B向左
					if (Keyboard::isKeyPressed(key.playerB_Left)) { playerPosition[1][0] -= 38; if (playerPosition[1][0] < 26) playerPosition[1][0] = 26; }
					// 玩家B向右
					if (Keyboard::isKeyPressed(key.playerB_Right)) { playerPosition[1][0] += 38; if (playerPosition[1][0] > 558) playerPosition[1][0] = 558; }
					// 玩家B落子
					if (Keyboard::isKeyPressed(key.playerB_dropChess)) placeKey[1] = 1;
					// 玩家B悔棋
					if (Keyboard::isKeyPressed(key.playerB_regret)) regretKey[1] = 1;
				}
				break;

				//
				// 鬆開鍵盤按鍵
				//
			case Event::KeyReleased:
				//
				// 暫停
				//
				if (event.key.code == Keyboard::Escape && pauseKey == 1) { pauseKey = 0; pauseGame = PauseGame(); }
				//
				// 玩家A
				//
				// 落子
				if (event.key.code == key.playerA_dropChess && placeKey[0] == 1 && control == 1)
				{
					placeKey[0] = 0;
					// 判斷棋盤上是否有棋子
					if (EmptyChess(playerPosition[0][0], playerPosition[0][1]))
					{
						sBlockStone.setPosition((float)playerPosition[0][0], (float)playerPosition[0][1]);	// 設定黑子位置為瞄準位置
						GomokuWindow.draw(sBlockStone);														// 繪製黑子
						status[(playerPosition[0][0] - 26) / 38][(playerPosition[0][1] - 26) / 38] = 1;		// 設定位置為有黑子
						playerRegretPosition[0][0] = playerPosition[0][0];									// 儲存玩家A悔棋X位置
						playerRegretPosition[0][1] = playerPosition[0][1];									// 儲存玩家A悔棋Y位置
						gameWinner = GameJudge(playerPosition[0][0], playerPosition[0][1], 1);				// 判斷遊戲是否結束
						GomokuWindow.display();																// 顯示畫面
						chessCount++;																		// 計算棋子數量
						regretLicense = 1;																	// 開啟悔棋權限
						control = -1;																		// 轉換控制權
					}
					else WaringOwnChess(playerPosition[0][0], playerPosition[0][1]);						// 顯示棋盤上已有棋子
				}
				// 悔棋
				if (event.key.code == key.playerA_regret && regretKey[0] == 1 && control == 1 && regretLicense) { Regret(); regretLicense = 0; }
				//
				// 玩家B
				//
				// 落子
				if (event.key.code == key.playerB_dropChess && placeKey[1] == 1 && control == -1)
				{
					placeKey[1] = 0;
					// 判斷棋盤上是否有棋子
					if (EmptyChess(playerPosition[1][0], playerPosition[1][1]))
					{
						sWhiteStone.setPosition((float)playerPosition[1][0], (float)playerPosition[1][1]);	// 設定白子位置為瞄準位置
						GomokuWindow.draw(sWhiteStone);														// 繪製白子
						status[(playerPosition[1][0] - 26) / 38][(playerPosition[1][1] - 26) / 38] = 2;		// 設定位置為有白子
						playerRegretPosition[1][0] = playerPosition[1][0];									// 儲存玩家B悔棋X位置
						playerRegretPosition[1][1] = playerPosition[1][1];									// 儲存玩家B悔棋Y位置
						gameWinner = GameJudge(playerPosition[1][0], playerPosition[1][1], 2);				// 判斷遊戲是否結束
						GomokuWindow.display();																// 顯示畫面
						chessCount++;																		// 計算棋子數量
						regretLicense = 1;																	// 開啟悔棋權限
						control = 1;																		// 轉換控制權
					}
					else WaringOwnChess(playerPosition[1][0], playerPosition[1][1]);						// 顯示棋盤上已有棋子
				}
				// 悔棋
				if (event.key.code == key.playerB_regret && regretKey[1] == 1 && control == -1 && regretLicense) { Regret(); regretLicense = 0; }
				break;
				//
				// 預設
				//
			default:
				break;
			}
		}
	}
	while (true)
	{
		gameRound = chessCount / 2 + 1;			// 計算回合數
		GomokuWindow.clear();					// 清除畫面
		if (gameWinner == 1)
		{
			Sprite sGameEndScreen(gameEnd);			// 獲勝提醒視窗
			Sprite sBackground(bg_gameStart);		// 背景
			Text text(L"黑子獲勝", font);			// 文字
			sBackground.setPosition(0, 0);			// 設定背景位置
			GomokuWindow.draw(sBackground);			// 繪製背景
			WriteCBoardChess();						// 繪製棋盤上棋子
			sGameEndScreen.setPosition(57, 207);	// 設定背景位置
			GomokuWindow.draw(sGameEndScreen);		// 繪製背景
			text.setCharacterSize(30);				// 設定文字大小
			text.setFillColor(Color::Black);		// 設定文字顏色
			text.setStyle(Text::Bold);				// 設定文字樣式
			text.setPosition(245, 300);				// 設定文字位置
			GomokuWindow.draw(text);				// 繪製文字
		}
		else if (gameWinner == 2)
		{
			Sprite sGameEndScreen(gameEnd);			// 獲勝提醒視窗
			Sprite sBackground(bg_gameStart);		// 背景
			Text text(L"白子獲勝", font);			// 文字
			sBackground.setPosition(0, 0);			// 設定背景位置
			GomokuWindow.draw(sBackground);			// 繪製背景
			WriteCBoardChess();						// 繪製棋盤上棋子
			sGameEndScreen.setPosition(57, 207);	// 設定背景位置
			GomokuWindow.draw(sGameEndScreen);		// 繪製背景
			text.setCharacterSize(30);				// 設定文字大小
			text.setFillColor(Color::Black);		// 設定文字顏色
			text.setStyle(Text::Bold);				// 設定文字樣式
			text.setPosition(245, 300);				// 設定文字位置
			GomokuWindow.draw(text);				// 繪製文字
		}
		GomokuWindow.display();					// 顯示畫面
		//
		// 事件處理
		//
		Event event;
		while (GomokuWindow.pollEvent(event))
		{
			switch (event.type)
			{
				// 
				// 關閉視窗
				//
			case Event::Closed:
				GomokuWindow.close();
				break;
				//
				// 按下鍵盤按鍵
				//
			case Event::KeyPressed:
				KeyPressed[0] = 1;
				break;
				//
				// 鬆開鍵盤按鍵
				//
			case Event::KeyReleased:
				if (KeyPressed) { KeyPressed[0] = 0; goto end; }
				break;
				//
				// 預設
				//
			default:
				break;
			}
		}
	}
end:
	return;
}

void Computer()
{
	Reset();	// 重置棋盤
	int KeyPressed[3] = { 0 },
		ESCPressed = 0;
	while (true)
	{
		Sprite sBackground(bg_computer);	// 背景
		GomokuWindow.clear();				// 清除畫面
		sBackground.setPosition(0, 0);		// 設定背景位置
		GomokuWindow.draw(sBackground);		// 繪製背景
		GomokuWindow.display();				// 顯示畫面
		//
		// 事件處理
		//
		Event event;
		while (GomokuWindow.pollEvent(event))
		{
			switch (event.type)
			{
				// 
				// 關閉視窗
				//
			case Event::Closed:
				GomokuWindow.close();
				break;
				//
				// 按下鍵盤按鍵
				//
			case Event::KeyPressed:
				// 紅子先手
				if (Keyboard::isKeyPressed(Keyboard::Num1)) KeyPressed[1] = 1;
				// 電腦(白子)先手
				if (Keyboard::isKeyPressed(Keyboard::Num2)) KeyPressed[2] = 1;
				// 按下ESC返回
				if (Keyboard::isKeyPressed(Keyboard::Escape)) ESCPressed = 1;
				break;
				//
				// 鬆開鍵盤按鍵
				//
			case Event::KeyReleased:
				if (event.key.code == Keyboard::Num1 && KeyPressed[1]) { if (KeyPressed[2]) KeyPressed[2] = 0; goto startGame; }
				if (event.key.code == Keyboard::Num2 && KeyPressed[2]) { if (KeyPressed[1]) KeyPressed[1] = 0; goto startGame; }
				if (event.key.code == Keyboard::Escape && ESCPressed) { ESCPressed = 0; return; }
				break;
				//
				// 預設
				//
			default:
				break;
			}
		}
	}
startGame:
	int control = 0;	// 控制目前執手
	int playerPosition[2] = { 292, 292 };
	if (KeyPressed[1]) { control = 1; KeyPressed[1] = 0; }
	else if (KeyPressed[2]) { control = -1; KeyPressed[2] = 0; }
	int pauseKey = 0,		// 暫停按鍵
		placeKey = 0,		// 落子按鍵
		regretKey = 0;		// 悔棋按鍵
	int pauseGame = 0;		// 暫停遊戲
	int regretLicense = 1;	// 悔棋許可
	computerPosition[0] = 292;
	computerPosition[1] = 292;
	while (gameWinner == 0)
	{
		Sprite sBackground(bg_gameStart);	// 背景
		GomokuWindow.clear();				// 清除畫面
		sBackground.setPosition(0, 0);		// 設定背景位置
		GomokuWindow.draw(sBackground);		// 繪製背景

		WriteCBoardChess();	// 繪製棋盤上棋子
		// 繪製棋盤資訊
		{
			string s;
			wstring ws;
			//
			// 繪製遊戲模式
			//
			Text text(L"電腦對戰", font);			// 繪製遊戲模式
			text.setCharacterSize(50);				// 設定文字大小
			text.setFillColor(Color::Black);		// 設定文字顏色
			text.setStyle(Text::Bold);				// 設定文字樣式
			text.setPosition(655, 50);				// 設定文字位置
			GomokuWindow.draw(text);				// 繪製文字
			//
			// 繪製回合數
			//
			text.setString(L"回合數");				// 繪製回合數
			text.setCharacterSize(30);				// 設定文字大小
			text.setPosition(710, 200);				// 設定文字位置
			GomokuWindow.draw(text);				// 繪製文字
			//
			// 繪製回合數字
			//
			if (gameRound < 10) { s += "00"; s += to_string(gameRound); }
			else if (gameRound < 100) { s += "0"; s += to_string(gameRound); }
			else s += to_string(gameRound);
			ws = stringToWString(s);
			text.setString(ws);						// 繪製回合數字
			text.setCharacterSize(30);				// 設定文字大小
			text.setPosition(730, 240);				// 設定文字位置
			GomokuWindow.draw(text);				// 繪製文字
			//
			// 執手狀態
			//
			if (control == 1)
			{
				text.setString(L"玩家執手");		// 繪製回合數
				text.setFillColor(Color::Magenta);	// 設定文字顏色
			}
			else
			{
				text.setString(L"電腦執手");		// 繪製回合數
				text.setFillColor(Color::White);	// 設定文字顏色
			}
			text.setCharacterSize(30);				// 設定文字大小
			text.setPosition(695, 360);				// 設定文字位置
			GomokuWindow.draw(text);				// 繪製文字
		}

		Sprite sBlock(playerARect);		// 黑子執手
		Sprite sBlockStone(blockStone);	// 黑子
		Sprite sWhiteStone(whiteStone);	// 白子

		gameRound = chessCount / 2 + 1;	// 計算回合數

		if (pauseGame == -1) goto end;	// 回主選單

		if (control == 1)
		{
			sBlock.setPosition((float)playerPosition[0], (float)playerPosition[1]);	// 設定黑子瞄準位置
			GomokuWindow.draw(sBlock);												// 繪製黑子瞄準格子
			GomokuWindow.display();													// 顯示畫面
		}
		else if (control == -1 && placeKey == 0)
		{
			MachineCount();																		// 計算電腦落子位置
			if (chessCount == 0) { computerPosition[0] = 292, computerPosition[1] = 292; }		// 設定白子位置(第一手)
			sWhiteStone.setPosition((float)computerPosition[0], (float)computerPosition[1]);	// 設定白子位置
			GomokuWindow.draw(sWhiteStone);														// 繪製白子格子
			status[(computerPosition[0] - 26) / 38][(computerPosition[1] - 26) / 38] = 2;		// 設定位置為有白子
			machineRegretPosition[0] = computerPosition[0];										// 儲存電腦悔棋X位置
			machineRegretPosition[1] = computerPosition[1];										// 儲存電腦悔棋Y位置
			gameWinner = GameJudge(computerPosition[0], computerPosition[1], 2);				// 判斷遊戲是否結束
			GomokuWindow.display();																// 顯示畫面
			chessCount++;																		// 計算棋子數量
			regretLicense = 1;																	// 開啟悔棋權限
			control = 1;																		// 換玩家執手
		}
		//
		// 事件處理
		//
		Event event;
		while (GomokuWindow.pollEvent(event))
		{
			switch (event.type)
			{
				// 
				// 關閉視窗
				//
			case Event::Closed:
				GomokuWindow.close();
				goto end;
				break;

				//
				// 按下鍵盤按鍵
				//
			case Event::KeyPressed:
				//
				// 遊戲
				//
				if (Keyboard::isKeyPressed(Keyboard::Escape)) pauseKey = 1;
				//
				// 玩家
				// 
				if (control == 1)
				{
					// 玩家向上
					if (Keyboard::isKeyPressed(key.playerA_Up)) { playerPosition[1] -= 38; if (playerPosition[1] < 26) playerPosition[1] = 26; }
					// 玩家向下
					if (Keyboard::isKeyPressed(key.playerA_Down)) { playerPosition[1] += 38; if (playerPosition[1] > 558) playerPosition[1] = 558; }
					// 玩家向左
					if (Keyboard::isKeyPressed(key.playerA_Left)) { playerPosition[0] -= 38; if (playerPosition[0] < 26) playerPosition[0] = 26; }
					// 玩家向右
					if (Keyboard::isKeyPressed(key.playerA_Right)) { playerPosition[0] += 38; if (playerPosition[0] > 558) playerPosition[0] = 558; }
					// 玩家落子
					if (Keyboard::isKeyPressed(key.playerA_dropChess)) placeKey = 1;
					// 玩家悔棋
					if (Keyboard::isKeyPressed(key.playerA_regret)) regretKey = 1;
				}
				break;

				//
				// 鬆開鍵盤按鍵
				//
			case Event::KeyReleased:
				//
				// 暫停
				//
				if (event.key.code == Keyboard::Escape && pauseKey == 1) { pauseKey = 0; pauseGame = PauseGame(); }
				//
				// 玩家
				//
				// 落子
				if (event.key.code == key.playerA_dropChess && placeKey == 1 && control == 1)
				{
					placeKey = 0;
					// 判斷棋盤上是否有棋子
					if (EmptyChess(playerPosition[0], playerPosition[1]))
					{
						sBlockStone.setPosition((float)playerPosition[0], (float)playerPosition[1]);	// 設定黑子位置為瞄準位置
						GomokuWindow.draw(sBlockStone);													// 繪製黑子
						status[(playerPosition[0] - 26) / 38][(playerPosition[1] - 26) / 38] = 1;		// 設定位置為有黑子
						playerRegretPosition[0][0] = playerPosition[0];									// 儲存玩家悔棋X位置
						playerRegretPosition[0][1] = playerPosition[1];									// 儲存玩家悔棋Y位置
						gameWinner = GameJudge(playerPosition[0], playerPosition[1], 1);				// 判斷遊戲是否結束
						GomokuWindow.display();															// 顯示畫面
						chessCount++;																	// 計算棋子數量
						control = -1;																	// 轉換控制權
					}
					else WaringOwnChess(playerPosition[0], playerPosition[1]);							// 顯示棋盤上已有棋子
				}
				// 悔棋
				if (event.key.code == key.playerA_regret && regretKey == 1 && control == 1 && regretLicense) { Regret(); regretLicense = 0; }
				break;
				//
				// 預設
				//
			default:
				break;
			}
		}
	}
	while (true)
	{
		gameRound = chessCount / 2 + 1;			// 計算回合數
		GomokuWindow.clear();					// 清除畫面
		if (gameWinner == 1)
		{
			Sprite sGameEndScreen(gameEnd);			// 獲勝提醒視窗
			Sprite sBackground(bg_gameStart);		// 背景
			Text text(L"玩家獲勝", font);			// 文字
			sBackground.setPosition(0, 0);			// 設定背景位置
			GomokuWindow.draw(sBackground);			// 繪製背景
			WriteCBoardChess();						// 繪製棋盤上棋子
			sGameEndScreen.setPosition(57, 207);	// 設定背景位置
			GomokuWindow.draw(sGameEndScreen);		// 繪製背景
			text.setCharacterSize(30);				// 設定文字大小
			text.setFillColor(Color::Black);		// 設定文字顏色
			text.setStyle(Text::Bold);				// 設定文字樣式
			text.setPosition(245, 300);				// 設定文字位置
			GomokuWindow.draw(text);				// 繪製文字
		}
		else if (gameWinner == 2)
		{
			Sprite sGameEndScreen(gameEnd);			// 獲勝提醒視窗
			Sprite sBackground(bg_gameStart);		// 背景
			Text text(L"電腦獲勝", font);			// 文字
			sBackground.setPosition(0, 0);			// 設定背景位置
			GomokuWindow.draw(sBackground);			// 繪製背景
			WriteCBoardChess();						// 繪製棋盤上棋子
			sGameEndScreen.setPosition(57, 207);	// 設定背景位置
			GomokuWindow.draw(sGameEndScreen);		// 繪製背景
			text.setCharacterSize(30);				// 設定文字大小
			text.setFillColor(Color::Black);		// 設定文字顏色
			text.setStyle(Text::Bold);				// 設定文字樣式
			text.setPosition(245, 300);				// 設定文字位置
			GomokuWindow.draw(text);				// 繪製文字
		}
		GomokuWindow.display();					// 顯示畫面
		//
		// 事件處理
		//
		Event event;
		while (GomokuWindow.pollEvent(event))
		{
			switch (event.type)
			{
				// 
				// 關閉視窗
				//
			case Event::Closed:
				GomokuWindow.close();
				break;
				//
				// 按下鍵盤按鍵
				//
			case Event::KeyPressed:
				KeyPressed[0] = 1;
				break;
				//
				// 鬆開鍵盤按鍵
				//
			case Event::KeyReleased:
				if (KeyPressed) { KeyPressed[0] = 0; goto end; }
				break;
				//
				// 預設
				//
			default:
				break;
			}
		}
	}
end:
	return;
}

void WriteCBoardChess()
{
	Sprite sBlockStone(blockStone);	// 黑子
	Sprite sWhiteStone(whiteStone);	// 白子
	//
	// 繪製棋盤
	//
	for (int y = 0; y < 15; y++) {
		for (int x = 0; x < 15; x++) {
			if (status[x][y] == 1) {
				sBlockStone.setPosition((float)(x * 38) + 26, (float)(y * 38) + 26);	// 設定黑子位置
				GomokuWindow.draw(sBlockStone);											// 繪製黑子
			}
			else if (status[x][y] == 2) {
				sWhiteStone.setPosition((float)(x * 38) + 26, (float)(y * 38) + 26);	// 設定白子位置
				GomokuWindow.draw(sWhiteStone);											// 繪白子
			}
		}
	}
	return;
}

void Regret()
{
	if (chessCount < 2) return;																		// 棋子數量小於2時不可悔棋
	if (playerRegretPosition[0][0] == -1 || playerRegretPosition[0][1] == -1) return;				// 玩家A|玩家悔過棋子
	if (playerRegretPosition[1][0] == -1 || playerRegretPosition[1][1] == -1) return;				// 玩家B悔過棋子	
	// 棋盤上該位置設為無棋子
	status[(playerRegretPosition[0][0] - 26) / 38][(playerRegretPosition[0][1] - 26) / 38] = 0;		// 玩家A
	if (!(playerRegretPosition[1][0] == 0 || playerRegretPosition[1][1] == 0))
		status[(playerRegretPosition[1][0] - 26) / 38][(playerRegretPosition[1][1] - 26) / 38] = 0;	// 玩家B
	if (!(machineRegretPosition[0] == 0 || machineRegretPosition[1] == 0))
		status[(machineRegretPosition[0] - 26) / 38][(machineRegretPosition[1] - 26) / 38] = 0;		// 電腦
	playerRegretPosition[0][0] = -1, playerRegretPosition[0][1] = -1;
	playerRegretPosition[1][0] = -1, playerRegretPosition[1][1] = -1;
	chessCount -= 2;																				// 復原棋子數量
	gameRound = chessCount / 2 + 1;																		// 復原回合數
	return;
}

int EmptyChess(int x, int y)
{
	if (status[(x - 26) / 38][(y - 26) / 38] == 0) return 1;
	else return 0;
}

int GameJudge(int x, int y, int target)
{
	x = (x - 26) / 38;
	y = (y - 26) / 38;
	int i, j, n1, n2;
	n1 = n2 = 0;
	//
	// 直線判斷
	//
	for (i = x, j = y; j >= 0; j--)  // 上
	{
		if (status[i][j] == target) n1++;
		else break;
	}
	for (i = x, j = y + 1; j < N; j++) // 下
	{
		if (status[i][j] == target) n2++;
		else break;
	}
	if (n1 + n2 >= 5) return target;
	n1 = n2 = 0;
	//
	// 橫線判斷
	//
	for (i = x, j = y; i >= 0; i--)  // 左 
	{
		if (status[i][j] == target) n1++;
		else break;
	}
	for (i = x + 1, j = y; i < N; i++) // 右
	{
		if (status[i][j] == target) n2++;
		else break;
	}
	if (n1 + n2 >= 5) return target;
	n1 = n2 = 0;
	//
	// 斜線判斷
	//
	for (i = x, j = y; i < N && j >= 0; i++, j--)   // 右上 
	{
		if (status[i][j] == target) n1++;
		else break;
	}
	for (i = x - 1, j = y + 1; i >= 0 && j < N; i--, j++)   // 左下
	{
		if (status[i][j] == target) n2++;
		else break;
	}
	if (n1 + n2 >= 5) return target;
	n1 = n2 = 0;
	for (i = x, j = y; i >= 0 && j >= 0; i--, j--)    // 左上 
	{
		if (status[i][j] == target) n1++;
		else break;
	}
	for (i = x + 1, j = y + 1; i < N && j < N; i++, j++)  // 右下
	{
		if (status[i][j] == target) n2++;
		else break;
	}
	if (n1 + n2 >= 5) return target;
	return 0;
}


void MachineCount()	// 計算電腦落子位置
{
	MachineAttack();	// 計算電腦攻擊權值
	MachineDefend();	// 計算電腦防守權值
	FindBestPosition();	// 尋找電腦最佳落子位置
	return;
}

void MachineAttack()
{
	int i, j;
	int k1, k2, k;
	for (int x = 0; x < N; x++)
	{
		for (int y = 0; y < N; y++)
		{
			if (status[x][y] != 0) attackValue[x][y] = 0;	// 如果該位置有棋子，則不考慮
			if (status[x][y] == 0)							// 如果該位置沒有棋子，則考慮
			{
				//
				// 橫向搜索
				//
				k1 = k2 = 0;
				for (i = x - 1, j = y; i >= 0; i--)	// 向左搜索電腦棋子數
				{
					if (status[i][j] == 2) k1++;
					else break;
				}
				for (i = x + 1, j = y; i < N; i++) // 向右搜索電腦棋子數
				{
					if (status[i][j] == 2) k2++;
					else break;
				}
				k = k1 > k2 ? k1 : k2;
				k1 = k2 = 0;
				//
				// 縱向搜索
				//
				for (i = x, j = y - 1; j >= 0; j--)	// 向上搜索電腦棋子數
				{
					if (status[i][j] == 2) k1++;
					else break;
				}
				for (i = x, j = y + 1; j < N; j++)	// 向下搜索電腦棋子數
				{
					if (status[i][j] == 2) k2++;
					else break;
				}
				k1 = k1 > k2 ? k1 : k2;
				k = k > k1 ? k : k1;
				k1 = k2 = 0;
				//
				// 斜向搜索
				//
				for (i = x - 1, j = y - 1; i >= 0 && j >= 0; i--, j--)	// 向左上搜索電腦棋子數
				{
					if (status[i][j] == 2) k1++;
					else break;
				}
				for (i = x + 1, j = y + 1; i < N && j < N; i++, j++)	// 向右下搜索電腦棋子數
				{
					if (status[i][j] == 2) k2++;
					else break;
				}
				k1 = k1 > k2 ? k1 : k2;
				k = k > k1 ? k : k1;
				k1 = k2 = 0;
				for (i = x - 1, j = y + 1; i >= 0 && j < N; i--, j++)	// 向左下搜索電腦棋子數
				{
					if (status[i][j] == 2) k1++;
					else break;
				}
				for (i = x + 1, j = y - 1; i < N && j >= 0; i++, j--)	// 向右上搜索電腦棋子數
				{
					if (status[i][j] == 2) k2++;
					else break;
				}
				k1 = k1 > k2 ? k1 : k2;
				k = k > k1 ? k : k1;
				switch (k)  // 根據電腦棋子數給予權值
				{
				case 3:		// 電腦有三子權值設為15
					attackValue[x][y] = 15; break;
				case 4:		// 電腦有四子權值設為 25
					attackValue[x][y] = 25; break;
				default:
					attackValue[x][y] = 3 + 2 * k; break;
				}
			}
		}
	}
	return;
}

void MachineDefend()
{
	int i, j;
	int k1, k2, k;
	for (int x = 0; x < N; x++)
	{
		for (int y = 0; y < N; y++)
		{
			if (status[x][y] != 0) defendValue[x][y] = 0;	// 如果該位置有棋子，則不考慮
			if (status[x][y] == 0)							// 如果該位置沒有棋子，則考慮
			{
				k1 = k2 = 0;
				//
				//	橫向搜索
				//
				for (i = x - 1, j = y; i >= 0; i--)	// 向左搜索玩家棋子數
				{
					if (status[i][j] == 1) k1++;
					else break;
				}
				for (i = x + 1, j = y; i < N; i++)	// 向右搜索玩家棋子數
				{
					if (status[i][j] == 1) k2++;
					else break;
				}
				k = k1 > k2 ? k1 : k2;
				k1 = k2 = 0;
				//
				// 縱向搜索
				//
				for (i = x, j = y - 1; j >= 0; j--)	// 向上搜索玩家棋子數
				{
					if (status[i][j] == 1) k1++;
					else break;
				}
				for (i = x, j = y + 1; j < N; j++)	// 向下搜索玩家棋子數
				{
					if (status[i][j] == 1) k2++;
					else break;
				}
				k1 = k1 > k2 ? k1 : k2;
				k = k > k1 ? k : k1;
				k1 = k2 = 0;
				//
				// 斜向搜索
				//
				for (i = x - 1, j = y - 1; i >= 0 && j >= 0; i--, j--)	// 向左上搜索玩家棋子數
				{
					if (status[i][j] == 1) k1++;
					else break;
				}
				for (i = x + 1, j = y + 1; i < N && j < N; i++, j++)	// 向右下搜索玩家棋子數
				{
					if (status[i][j] == 1) k2++;
					else break;
				}
				k1 = k1 > k2 ? k1 : k2;
				k = k > k1 ? k : k1;
				k1 = k2 = 0;
				for (i = x - 1, j = y + 1; i >= 0 && j < N; i--, j++)	// 向左下搜索玩家棋子數
				{
					if (status[i][j] == 1) k1++;
					else break;
				}
				for (i = x + 1, j = y - 1; i < N && j >= 0; i++, j--)	// 向右上搜索玩家棋子數
				{
					if (status[i][j] == 1) k2++;
					else break;
				}
				k1 = k1 > k2 ? k1 : k2;
				k = k > k1 ? k : k1;
				switch (k)  // 根據玩家棋子數給予權值
				{
				case 3:		// 玩家有三子權值設為 10
					defendValue[x][y] = 10; break;
				case 4:		// 玩家有四子權值設為 20
					defendValue[x][y] = 20; break;
				default:
					defendValue[x][y] = 2 + k * 2; break;
				}
			}
		}
	}
	return;
}

void FindBestPosition()
{
	int k1 = 0, k2 = 0;
	int x, y, max = 0;
	for (y = 0; y < N; y++)
		for (x = 0; x < N; x++)
		{
			if (attackValue[x][y] >= max)
			{
				max = attackValue[x][y];
				k1 = x;
				k2 = y;
			}
		}
	for (y = 0; y < N; y++)
		for (x = 0; x < N; x++)
		{
			if (defendValue[x][y] >= max)
			{
				max = defendValue[x][y];
				k1 = x;
				k2 = y;
			}
		}
	// 設定電腦落子位置
	computerPosition[0] = (k1 * 38) + 26;
	computerPosition[1] = (k2 * 38) + 26;
	return;
}

void Reset()
{
	// 初始化
	gameWinner = 0;	// 遊戲狀態重置
	gameRound = 1;		// 回合數
	chessCount = 0;		// 落子數
	for(int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
		{
			gameWinner = 0;
			status[i][j] = 0;
			attackValue[i][j] = 0;
			defendValue[i][j] = 0;
		}
	playerRegretPosition[0][0] = 0, playerRegretPosition[0][1] = 0;
	playerRegretPosition[1][0] = 0, playerRegretPosition[1][1] = 0;
	machineRegretPosition[0] = 0, machineRegretPosition[1] = 0;
	// 電腦先下中間
	computerPosition[0] = 292;
	computerPosition[1] = 292;
	return;
}

void WaringOwnChess(int x, int y)
{
	Sprite sWaringOwnChess(waringOwnChess);
	sWaringOwnChess.setPosition((float)x - 142, (float)y - 10);
	GomokuWindow.draw(sWaringOwnChess);
	GomokuWindow.display();
	sleep(milliseconds(1500));
	return;
}

void Gametips()
{
	int KeyPressed = 0;
	while (true)
	{
		Sprite sBackground(bg_gametips);	// 背景
		GomokuWindow.clear();				// 清除畫面
		sBackground.setPosition(0, 0);		// 設定背景位置
		GomokuWindow.draw(sBackground);		// 繪製背景
		GomokuWindow.display();				// 顯示畫面
		//
		// 事件處理
		//
		Event event;
		while (GomokuWindow.pollEvent(event))
		{
			switch (event.type)
			{
				// 
				// 關閉視窗
				//
			case Event::Closed:
				GomokuWindow.close();
				break;
				//
				// 按下鍵盤按鍵
				//
			case Event::KeyPressed:
				KeyPressed = 1;
				break;
				//
				// 鬆開鍵盤按鍵
				//
			case Event::KeyReleased:
				if (KeyPressed) { KeyPressed = 0; goto end; }
				break;
				//
				// 預設
				//
			default:
				break;
			}
		}
	}
end:
	return;
}

void Updatediary()
{
	int KeyPressed = 0;
	while (true)
	{
		Sprite sBackground(bg_updatediary);	// 背景
		GomokuWindow.clear();				// 清除畫面
		sBackground.setPosition(0, 0);		// 設定背景位置
		GomokuWindow.draw(sBackground);		// 繪製背景
		GomokuWindow.display();				// 顯示畫面
		//
		// 事件處理
		//
		Event event;
		while (GomokuWindow.pollEvent(event))
		{
			switch (event.type)
			{
				// 
				// 關閉視窗
				//
			case Event::Closed:
				GomokuWindow.close();
				break;
				//
				// 按下鍵盤按鍵
				//
			case Event::KeyPressed:
				KeyPressed = 1;
				break;
				//
				// 鬆開鍵盤按鍵
				//
			case Event::KeyReleased:
				if (KeyPressed) { KeyPressed = 0; goto end; }
				break;
				//
				// 預設
				//
			default:
				break;
			}
		}
	}
end:
	return;
}

int PauseGame()
{
	int KeyPressed = 0;
	while (true)
	{
		Sprite sBackground(bg_pauseGame);	// 背景
		GomokuWindow.clear();				// 清除畫面
		sBackground.setPosition(0, 0);		// 設定背景位置
		GomokuWindow.draw(sBackground);		// 繪製背景
		GomokuWindow.display();				// 顯示畫面
		//
		// 事件處理
		//
		Event event;
		while (GomokuWindow.pollEvent(event))
		{
			switch (event.type)
			{
				// 
				// 關閉視窗
				//
			case Event::Closed:
				GomokuWindow.close();
				break;
				//
				// 按下鍵盤按鍵
				//
			case Event::KeyPressed:
				if (Keyboard::isKeyPressed(Keyboard::Escape)) KeyPressed = 1;
				if (Keyboard::isKeyPressed(Keyboard::X)) KeyPressed = 2;
				break;
				//
				// 鬆開鍵盤按鍵
				//
			case Event::KeyReleased:
				if (event.key.code == Keyboard::Escape && KeyPressed == 1) { KeyPressed = 0; goto end; }
				if (event.key.code == Keyboard::X && KeyPressed == 2) { KeyPressed = 0; return -1; }
				break;
				//
				// 預設
				//
			default:
				break;
			}
		}
	}
end:
	return 0;
}

int main(void) {
	//
	// 載入圖片
	//
	if (!icon.loadFromFile("./Image/黑子.png")) return EXIT_FAILURE;
	if (!font.loadFromFile("./Font/msjhbd.ttc")) return EXIT_FAILURE;
	if (!bg_main.loadFromFile("./Image/主畫面背景.png")) return EXIT_FAILURE;
	if (!bg_gameStart.loadFromFile("./Image/五子棋背景.png")) return EXIT_FAILURE;
	if (!bg_coop.loadFromFile("./Image/雙人對戰主選單背景.png")) return EXIT_FAILURE;
	if (!bg_computer.loadFromFile("./Image/電腦對戰主選單背景.png")) return EXIT_FAILURE;
	if (!bg_pauseGame.loadFromFile("./Image/暫停畫面背景.png")) return EXIT_FAILURE;
	if (!bg_gametips.loadFromFile("./Image/遊戲操作背景.png")) return EXIT_FAILURE;
	if (!bg_updatediary.loadFromFile("./Image/更新日誌背景.png")) return EXIT_FAILURE;
	if (!playerARect.loadFromFile("./Image/playerARect.png")) return EXIT_FAILURE;
	if (!playerBRect.loadFromFile("./Image/playerBRect.png")) return EXIT_FAILURE;
	if (!blockStone.loadFromFile("./Image/黑子.png")) return EXIT_FAILURE;
	if (!whiteStone.loadFromFile("./Image/白子.png")) return EXIT_FAILURE;
	if (!waringOwnChess.loadFromFile("./Image/警告_已經擁有棋子.png")) return EXIT_FAILURE;
	if (!gameEnd.loadFromFile("./Image/遊戲結束.png")) return EXIT_FAILURE;
	bg_main.setSmooth(true);
	bg_gameStart.setSmooth(true);
	bg_coop.setSmooth(true);
	bg_computer.setSmooth(true);
	bg_gametips.setSmooth(true);
	bg_updatediary.setSmooth(true);
	playerARect.setSmooth(true);
	playerBRect.setSmooth(true);
	blockStone.setSmooth(true);
	whiteStone.setSmooth(true);
	waringOwnChess.setSmooth(true);
	gameEnd.setSmooth(true);
	//
	//	視窗設定
	//
	// 開啟垂直同步
	GomokuWindow.setVerticalSyncEnabled(true);
	// 設定最高 fps
	GomokuWindow.setFramerateLimit(60);  // 使用不太準的 sf::Clock (+-10-15ms)
	// 設定icon
	GomokuWindow.setIcon(30, 30, icon.getPixelsPtr());

    int gameStart = 0;	// 遊戲是否開始

	while (GomokuWindow.isOpen())
	{
        if (!gameStart) Welcome();	// 遊戲開始介面
		//
		// 事件處理
		//
		Event event;
		while (GomokuWindow.pollEvent(event))
		{
			switch (event.type)
			{
				// 
				// 關閉視窗
				//
			case Event::Closed:
				GomokuWindow.close();
				break;
				//
				// 預設
				//
			default:
				break;
			}
		}
	}
	return 0;
}
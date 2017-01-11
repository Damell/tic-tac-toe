#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#define POZADI 0
#define KRIZKY 1
#define KOLECKA 2
#define POLE 20
#define DIVISIONS 30

int pole[DIVISIONS][DIVISIONS];
signed int direction[8][2] = {-1, -1, 0, -1, 1, -1, 1, 0, 1, 1, 0, 1, -1, 1, -1, 0};
int total = 0;
int o_total = 0;
int best[3] = {0, 0, 0};
int help[3] = {0, 0, 0};
int number;
int znak;
int end = 0;

void show(void);
void clear(void);
void krizky(void);
void kolecka(void);
void search();
void around(int i, int j, int znak);
void other(int i, int j, int a, int znak);
void computer();
void c_search();
void c_around(int i, int j);
void c_other(int i, int j, int a, int znak);

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
     static TCHAR szAppName[] = TEXT ("Piskvorky") ;
     HWND         hwnd ;
     MSG          msg ;
     WNDCLASS     wndclass ;

     wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
     wndclass.lpfnWndProc   = WndProc ;
     wndclass.cbClsExtra    = 0 ;
     wndclass.cbWndExtra    = 0 ;
     wndclass.hInstance     = hInstance ;
     wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
     wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
     wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
     wndclass.lpszMenuName  = NULL ;
     wndclass.lpszClassName = szAppName ;
     
     if (!RegisterClass (&wndclass))
     {
          MessageBox (NULL, TEXT ("Program vyžaduje Windows NT!"), 
                      szAppName, MB_ICONERROR) ;
          return 0 ;
     }
     
     hwnd = CreateWindow (szAppName, TEXT ("Piskvorky 2.0"),
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          708, 710,
                          NULL, NULL, hInstance, NULL) ;
     
     ShowWindow (hwnd, iCmdShow) ;
     UpdateWindow (hwnd) ;
     
     while (GetMessage (&msg, NULL, 0, 0))
     {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
     }
     return msg.wParam ;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
     static int  cxBlock, cyBlock ;
     HDC         hdc ;
     int         wnd_x, wnd_y ;
     PAINTSTRUCT ps ;
     POINT       point ;
     RECT        rect ;
     HPEN        hPen;
     
     switch (message)
     {
     case WM_SIZE :
          cxBlock = LOWORD (lParam) / DIVISIONS ;
          cyBlock = HIWORD (lParam) / DIVISIONS ;
          return 0 ;
          
     case WM_SETFOCUS :
          ShowCursor (TRUE) ;
          return 0 ;
          
     case WM_KILLFOCUS :
          ShowCursor (FALSE) ;
          return 0 ;
          
     case WM_KEYDOWN :
          GetCursorPos (&point) ;
          ScreenToClient (hwnd, &point) ;
          
          wnd_x = max (0, min (DIVISIONS - 1, point.x / cxBlock)) ;
          wnd_y = max (0, min (DIVISIONS - 1, point.y / cyBlock)) ;
          
          switch (wParam)
          {
          case VK_UP :
               wnd_y-- ;
               break ;
               
          case VK_DOWN :
               wnd_y++ ;
               break ;
               
          case VK_LEFT :
               wnd_x-- ;
               break ;
               
          case VK_RIGHT :
               wnd_x++ ;
               break ;
               
          case VK_HOME :
               wnd_x = wnd_y = 0 ;
               break ;
               
          case VK_END :
               wnd_x = wnd_y = DIVISIONS - 1 ;
               break ;
               
          case VK_RETURN :
          case VK_SPACE :
               SendMessage (hwnd, WM_LBUTTONDOWN, MK_LBUTTON,
                            MAKELONG (wnd_x * cxBlock, wnd_y * cyBlock)) ;
               break ;
          }
          wnd_x = (wnd_x + DIVISIONS) % DIVISIONS ;
          wnd_y = (wnd_y + DIVISIONS) % DIVISIONS ;
          
          point.x = wnd_x * cxBlock + cxBlock / 2 ;
          point.y = wnd_y * cyBlock + cyBlock / 2 ;
          
          ClientToScreen (hwnd, &point) ;
          SetCursorPos (point.x, point.y) ;
          return 0 ;
          
     case WM_LBUTTONDOWN :
          best[0] = 0; 
          best[1] = 0;
          best[2] = 0;
          wnd_x = LOWORD (lParam) / cxBlock ;
          wnd_y = HIWORD (lParam) / cyBlock ;
          
          if (wnd_x < DIVISIONS && wnd_y < DIVISIONS)
          {
               if(pole[wnd_x][wnd_y]!=KRIZKY && pole[wnd_x][wnd_y]!=KOLECKA)
               {
               pole[wnd_x][wnd_y] = KRIZKY ;
               rect.left   = wnd_x * cxBlock ;
               rect.top    = wnd_y * cyBlock ;
               rect.right  = (wnd_x + 1) * cxBlock ;
               rect.bottom = (wnd_y + 1) * cyBlock ;
               InvalidateRect (hwnd, &rect, FALSE) ;
               search();
               if(end==1){
               MessageBox(NULL, TEXT("Vyhral jste. Gratuluji!"), TEXT("Gratulace!"), 0);
               return 1;
               }
               else{
               computer();
               rect.left  = best[0] * cxBlock;
               rect.top    = best[1] * cyBlock ;
               rect.right  = (best[0]+1) * cxBlock ;
               rect.bottom = (best[1]+1) * cyBlock ;
               InvalidateRect (hwnd, &rect, FALSE);
               search();
               if(end==2){
               MessageBox(NULL, TEXT("Prohral jste. Zkuste znovu"), TEXT("Jste pokoren"), 0);
               }
               }
               }
               else{
               MessageBox(NULL, TEXT("Prosim nesnazte se podvadet!"), TEXT("Varovani"), 0);
               }
          }
          else
               MessageBeep (0) ;
          return 0 ;
          
     case WM_PAINT :
          hdc = BeginPaint (hwnd, &ps) ;

          for (wnd_x = 0 ; wnd_x < DIVISIONS ; wnd_x++)
          for (wnd_y = 0 ; wnd_y < DIVISIONS ; wnd_y++)
          {
               SelectObject(hdc, GetStockObject(BLACK_PEN));
               Rectangle (hdc, wnd_x * cxBlock, wnd_y * cyBlock,
                          (wnd_x + 1) * cxBlock, (wnd_y + 1) * cyBlock) ;
                    
               if (pole[wnd_x][wnd_y]==KRIZKY)
               {
                    hPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
                    SelectObject(hdc, hPen);
                    MoveToEx (hdc,  wnd_x   *cxBlock+1,  wnd_y   *cyBlock+1, NULL) ;
                    LineTo   (hdc, (wnd_x+1)*cxBlock-1, (wnd_y+1)*cyBlock-1) ;
                    MoveToEx (hdc,  wnd_x   *cxBlock+1, (wnd_y+1)*cyBlock-1, NULL) ;
                    LineTo   (hdc, (wnd_x+1)*cxBlock-1,  wnd_y   *cyBlock+1) ;
                    DeleteObject(hPen);
                    
               }
               else if(pole[wnd_x][wnd_y]==KOLECKA)
               {
                    hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
                    SelectObject(hdc, hPen);
                    Ellipse (hdc, wnd_x*cxBlock+1, wnd_y*cyBlock+1, (wnd_x+1)*cxBlock-1, (wnd_y+1)*cyBlock-1);
                    DeleteObject(hPen);
               }
          }

          EndPaint (hwnd, &ps) ;
          return 0 ;
               
     case WM_DESTROY :
          PostQuitMessage (0) ;
          return 0 ;
     }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
}


void search(){
int i, j;
for(i=0; i<POLE; i++){
  for(j=0; j<POLE; j++){
    if(pole[i][j]!=0){
      znak = pole[i][j];
      around(i, j, znak);}
  }
}
}

void around(int i, int j, int znak){
int a;
int k, l;
for(a=0; a<8; a++){
  if(pole[i+direction[a][0]][j+direction[a][1]]==znak){
  k = i+direction[a][0];
  l = j+direction[a][1];
  total = 2;
  other(k, l, a, znak);
  }
}
}

void other(int i, int j, int a, int znak){
int k, l;
if(pole[i+direction[a][0]][j+direction[a][1]]==znak){
  total++;
  if(total==5){
    if(znak==1)
    end = 1;
    else if(znak==2)
    end = 2;}
  k = i+direction[a][0];
  l = j+direction[a][1];
  other(k, l, a, znak);}
}

void computer(){
 int x, y;
 c_search();
 x = best[0];
 y = best[1];
 pole[x][y] = KOLECKA;

}

void c_search(){
int i, j;
for(i=0; i<POLE; i++){
  for(j=0; j<POLE; j++){
    if(pole[i][j]==POZADI){
      c_around(i, j);}
}}
}

void c_around(int i, int j){
int a;
int u, k, l;
help[0] = i;
help[1] = j;
help[2] = 0;
for(a=0; a<8; a++){
  if(pole[i+direction[a][0]][j+direction[a][1]]==KRIZKY){
  k = i+direction[a][0];
  l = j+direction[a][1];
  total = 1;
  help[2]+=6;
  c_other(k, l, a, KRIZKY);}
  else if(pole[i+direction[a][0]][j+direction[a][1]]==KOLECKA){
  k = i+direction[a][0];
  l = j+direction[a][1];
  total = 1;
  help[2]+=7;
  c_other(k, l, a, KOLECKA);
  }
}
if(help[2]>best[2]){
best[0] = help[0];
best[1] = help[1];
best[2] = help[2];}
}


void c_other(int i, int j, int a, int znak){
int k, l, o_a;
int o_znak;
if(znak==1)
  o_znak = 2;
else if(znak==2)
  o_znak = 1;
if(a<4)
o_a = a + 4;
else if(a>=4)
o_a = a - 4;
if(pole[i+direction[a][0]][j+direction[a][1]]==znak){
  total++;
  help[2] += 15;
  if(total>=3){
    if(znak==2){
      help[2] += 20;}
    help[2] += 50;}
  if(total>=4){
    if(znak==2){
      help[2] += 800;}
    help[2] += 1000;}
  if(total==4 && znak==2)
  end = 2;
  if(total==5 && znak==1)
  end = 1;
  k = i+direction[a][0];
  l = j+direction[a][1];
  c_other(k, l, a, znak);}
else if(pole[i+direction[a][0]][j+direction[a][1]]==o_znak){
  if(znak==1){
    if(total==2)
    help[2] -= 4;
    if(total==3)
    help[2] -= 35;}
  else if(znak==2){
    if(total==2)
    help[2] -= 5;
    if(total==3)
    help[2] -= 40;}}
else if(pole[help[0]+direction[o_a][0]][help[1]+direction[o_a][1]]==znak){
  if(total==1)
    help[2] += 10;
  if(total==2){
    if(znak==2){
      help[2] += 20;}
    help[2] += 50;}
  if(total==3){
    if(znak==2){
      help[2] += 800;}
    help[2] += 1000;}
}
}


#include <LiquidCrystal.h>
#include "pitches.h"

//Lyd
int buzzer = 45; // pinne til piezzo buzzer

unsigned int lydrett[] = {0, NOTE_A4, NOTE_A4, NOTE_A4, NOTE_A4, NOTE_G4, NOTE_A4};
int noteLengder1[] = {16, 8, 8, 8, 4, 8, 4};

unsigned int lydfeil[] = {0, NOTE_D4, NOTE_CS4, NOTE_C4, NOTE_B3, NOTE_AS3};
int noteLengder2[] = {8, 4, 4, 4, 4, 4};

unsigned int lydintro[] = {0, NOTE_A4, NOTE_B4, NOTE_C5, NOTE_G5, NOTE_E5, NOTE_E5, NOTE_D5, NOTE_G5, NOTE_E5, NOTE_D5, NOTE_C5, NOTE_A4, NOTE_E5, NOTE_D5,
                           NOTE_C5, NOTE_A4, NOTE_C3, NOTE_C3, NOTE_G2, NOTE_G2, NOTE_G5, NOTE_E5, NOTE_E5, NOTE_D5, NOTE_G5, NOTE_E5, NOTE_D5, NOTE_G5, NOTE_E5, NOTE_D5, NOTE_C5, NOTE_F5, NOTE_E5, NOTE_D5, NOTE_C5};
int noteLengder3[] = {1, 8, 8, 4, 4, 4, 8, 8, 4, 4, 8, 8, 4, 4, 8, 8, 4, 4, 4, 4, 4, 4, 4, 8, 8, 4, 8, 8, 4, 4, 8, 8, 4, 4, 2, 4};

unsigned long lydTidSist;
unsigned long lydTidStans;

int noteCounter1, noteCounter2, noteCounter3;

enum SpillSang
{
  ingen,
  spillIntro,
  spillRett,
  spillGalt
};

//RGB:
int redpinne = A3;
int greenpinne = A4;
int bluepinne = A5;

enum FargeRGB
{
  TURKIS,
  GUL,
  LILLA,
  OFF
};
unsigned long LedLysTidSistOppdatert;
int fargeTeller = 0;

LiquidCrystal lcd(23, 25, 27, 29, 31, 33);

//JoystickShield:
// Select button is triggered when joystick is pressed
const byte PIN_BUTTON_SELECT = 2;
const byte PIN_BUTTON_RIGHT = 3;
const byte PIN_BUTTON_UP = 4;
const byte PIN_BUTTON_DOWN = 5;
const byte PIN_BUTTON_LEFT = 6;

const byte PIN_ANALOG_X = 0;
const byte PIN_ANALOG_Y = 1;

//Et spørsmål inneholder: Spørsmål, 4 svaralternativer, og et av de rett svar:
//struct Oppgave {
//  char oppgaveTekst[11];
//  const char* valg[4];
//  int rettSvar;
//};

enum Direction
{
  finish = 0,
  right,
  left,
  up,
  down,
  troll,
  spm,
  stille
};

//Karakterene
byte MainCharacter[8] = {
    B00000,
    B01110,
    B01110,
    B00100,
    B01110,
    B00100,
    B01010,
};

byte Troll[8] = {
    B11111,
    B11111,
    B00100,
    B11111,
    B00100,
    B01010,
    B11011,
};

byte Up[8] = {
    B00100,
    B01110,
    B10101,
    B00100,
    B00100,
    B00000,
    B00000,
};

byte Down[8] = {
    B00000,
    B00000,
    B00100,
    B00100,
    B10101,
    B01110,
    B00100,
};

byte Left[8] = {
    B00000,
    B00100,
    B01000,
    B11111,
    B01000,
    B00100,
    B00000,
};

byte Right[8] = {
    B00000,
    B00100,
    B00010,
    B11111,
    B00010,
    B00100,
    B00000,
};

enum Character
{
  rightImg,
  leftImg,
  upImg,
  downImg,
  heroImg,
  trollImg,
  blackImg
};

enum Spilltilstand
{
  neutral,
  tapt,
  vunnet
};

Direction requiredMoves[11] = {up, down, right, troll, left, finish};

//Oppgave o1 = { { "1+1= "}, {"1", "2", "3", "4"}, 1 };
//Oppgave o2 = { {"1+2= "}, {"1", "2", "3", "4"}, 2 };
//Oppgave o3 = { { "1+3= "}, {"1", "2", "3", "4"}, 3 };
//Oppgave o4 = { {"2+2= "}, {"1", "2", "3", "4"}, 3 };
//Oppgave o[] = { o1, o2, o3, o4 };

//Tiden lest nå:
unsigned long lastUpdate;

unsigned long startLesInputTid;

//For troll og spm:
// Tidene i funksjonene resettes hver gang posisjonene økes/skjerm endres?
unsigned long startTrollTid;
unsigned long startPilTid;

//Definisjonen for en tilstand. Inneholder disse verdiene:
struct GameState
{
  bool notStarted;
  bool ChallengeMode;
  int spillfigurPosition;
  int trollPosition;
  int pilPosition;
  SpillSang melodi;
  // spørsmåls array Oppgaver oppgaver[] = {}
  //Retning sier om det er en retning som skal gjøres i arrayen.
  Direction retning;
  Direction Utfordring;
  Spilltilstand tilstand;
  bool whitePressed;
  bool bluePressed;
  bool yellowPressed;
  bool redPressed;
  int x;
  int y;
};

//Spilltilstand blir lagret og hentet herfra:
GameState game = {true, false, 0, 15, 16}; // ingen, stille };

void setup()
{
  //Lager bildene av karakterene:
  lcd.createChar(heroImg, MainCharacter);
  lcd.createChar(trollImg, Troll);
  lcd.createChar(upImg, Up);
  lcd.createChar(downImg, Down);
  lcd.createChar(leftImg, Left);
  lcd.createChar(rightImg, Right);

  lcd.begin(16, 2);
  //For testing til skjerm:
  Serial.begin(9600);

  //RGB-LED:
  pinMode(redpinne, OUTPUT);
  pinMode(greenpinne, OUTPUT);
  pinMode(bluepinne, OUTPUT);

  pinMode(buzzer, OUTPUT);

  lydTidSist = millis();

  noteCounter1 = 0;
  noteCounter2 = 0;
  noteCounter3 = 0;

  LedLysTidSistOppdatert = millis();

  pinMode(PIN_BUTTON_RIGHT, INPUT);
  digitalWrite(PIN_BUTTON_RIGHT, HIGH);

  pinMode(PIN_BUTTON_LEFT, INPUT);
  digitalWrite(PIN_BUTTON_LEFT, HIGH);

  pinMode(PIN_BUTTON_UP, INPUT);
  digitalWrite(PIN_BUTTON_UP, HIGH);

  pinMode(PIN_BUTTON_DOWN, INPUT);
  digitalWrite(PIN_BUTTON_DOWN, HIGH);

  pinMode(PIN_BUTTON_SELECT, INPUT);
  digitalWrite(PIN_BUTTON_SELECT, HIGH);

  lcd.display();
  lcd.clear();
  game.retning = stille;
  game.melodi = spillIntro;

  //Husker denne tiden, som et tall.
  //Slik at man kan finne differansen til den er i rett del av programmet.Gjelder for alle.
  lastUpdate = millis();
  startLesInputTid = millis();
  startPilTid = millis();
  startTrollTid = millis();

  lcd.setCursor(0, 0);
  lcd.println("ArduinoQuest");
  lcd.setCursor(0, 1);
  lcd.println("Trykk paa hvit knapp for aa starte!");
}

void loop()
{

  LesInput();
  GameLogic();
  SpillLyd();
  KontrollerLED();
  DrawScreen();
}

void LesInput()
{
  if (game.tilstand == vunnet || game.tilstand == tapt)
  {
    return;
  }

  unsigned long startLesInputSluttTid = millis();

  Direction maGjores = requiredMoves[game.spillfigurPosition];

  game.x = analogRead(PIN_ANALOG_X);
  game.y = analogRead(PIN_ANALOG_Y);

  Serial.print("x:");
  Serial.println(game.x);

  Serial.print("y:");
  Serial.println(game.y);

  tilstanderForShield();

  if (game.notStarted)
  {
    game.retning = stille;
    game.melodi = spillIntro;
  }
  else
  {
    if (maGjores == troll || maGjores == spm)
    {
      game.ChallengeMode = true;
      game.Utfordring = maGjores;
      game.melodi = spillIntro;
    }
    else if (maGjores == finish)
    {
      game.retning = finish;

      game.ChallengeMode = false;
      game.melodi = spillIntro;

      tilstanderForShield();
    }
  }
}

//Spillets regler
void GameLogic()
{
  if (game.tilstand == vunnet || game.tilstand == tapt)
  {
    return;
  }

  if (game.retning == finish)
  {
    game.tilstand = vunnet;
    game.melodi = spillIntro;
    return;
  }

  if (game.yellowPressed && game.whitePressed == false)
  {
    TestModus();
    game.notStarted = false;
  }

  if (game.notStarted && game.whitePressed)
  {
    game.notStarted = false;
    if (game.retning == stille)
    {
      game.melodi = ingen;
    }
  }
  else if (game.ChallengeMode)
  {
    if (game.Utfordring == troll)
    {
      unsigned long trollPilTidSlutt = millis();
      if (trollPilTidSlutt - startTrollTid > 1000)
      {
        game.trollPosition--;
        startTrollTid = trollPilTidSlutt;
      }
      if (game.bluePressed && game.pilPosition == 16)
      {
        game.pilPosition = game.spillfigurPosition;
      }
      if (game.pilPosition != 16)
      {
        if ((trollPilTidSlutt - startPilTid) > 500)
        {
          game.pilPosition++;
          startPilTid = trollPilTidSlutt;
        }
      }
      // Det er teoretisk mulig at pil og troll kan krysse hverandre om det bare
      // sjekkes på trollPosition == pilPosition
      if (game.trollPosition <= game.pilPosition && game.pilPosition != 16)
      {
        game.trollPosition = 15;
        game.pilPosition = 16;
        game.spillfigurPosition++;
        game.ChallengeMode = false;
        game.melodi = spillRett;
      }
      else if (game.trollPosition == game.spillfigurPosition)
      {
        game.tilstand = tapt;
        game.ChallengeMode = false;
        Serial.println(game.melodi);
      }
    }
    else if (game.Utfordring == spm)
    {
      //Kode for spm her
    }
  }
  else
  {
    if (game.retning == requiredMoves[game.spillfigurPosition])
    {
      game.melodi = spillRett;
      game.spillfigurPosition++;
    }
    else
    {
      if (game.retning != stille)
      {
      }
    }
  }
}

//Tegner skjerm:
void DrawScreen()
{
  unsigned long currentTime = millis();
  // hvis det har gått mer enn 500 ms siden setup() sluttet
  // eller sist DrawScreen() ble kjørt.
  if (currentTime - lastUpdate > 250)
  {
    lcd.setCursor(0, 0);

    if (game.tilstand == vunnet)
    {
      lcd.println("Du har vunnet");
    }
    else if (game.tilstand == tapt)
    {
      lcd.println("Du tapte");
    }
    else
    {

      if (game.notStarted)
      {
        lcd.scrollDisplayLeft();
      }
      else if (game.ChallengeMode)
      {
        lcd.clear();
        if (game.Utfordring == spm)
        {
          lcd.setCursor(0, 0);
        }
        else if (game.Utfordring == troll)
        {
          lcd.setCursor(8, 0);
          lcd.print("TROLL!!!");
          DrawMapAndHero();
        }
      }
      else
      {
        // Vis kart
        lcd.clear();
        DrawMapAndHero();
      }
    }
    lastUpdate = currentTime;
  }
}

// Tegner kartet og figuren:
void DrawMapAndHero()
{
  for (int i = 0; i < 11; i++)
  {
    lcd.setCursor(i, 0);
    if (requiredMoves[i] != finish)
    {
      switch (requiredMoves[i])
      {
      case (right):
        WriteCharacterToScreen(rightImg, i, 0);
        break;
      case (left):
        WriteCharacterToScreen(leftImg, i, 0);
        break;
      case (up):
        WriteCharacterToScreen(upImg, i, 0);
        break;
      case (down):
        WriteCharacterToScreen(downImg, i, 0);
        break;
      default:
        lcd.print("?");
      }
    }
  }
  if (game.ChallengeMode == true && game.Utfordring == troll)
  {
    WriteCharacterToScreen(trollImg, game.trollPosition, 1);
    WriteCharacterToScreen(rightImg, game.pilPosition, 1);
  }
  WriteCharacterToScreen(heroImg, game.spillfigurPosition, 1);
}

void WriteCharacterToScreen(Character figur, int ax, int by)
{
  lcd.setCursor(ax, by);
  lcd.write(byte(figur));
}

void KontrollerLED()
{
  unsigned long ledLysTidNaa = millis();

  if (ledLysTidNaa - LedLysTidSistOppdatert > 1000)
  {
    setStatuslampe(OFF);
    if (game.ChallengeMode || game.notStarted)
    {
      setStatuslampe((FargeRGB)(fargeTeller % 3));
      fargeTeller++;
    }
    else
    {
      setStatuslampe(OFF);
    }
    if (game.retning == requiredMoves[game.spillfigurPosition])
    {
      setStatuslampe(LILLA);
    }
    else
    {
      setStatuslampe(OFF);
    }
    if (game.tilstand == tapt)
    {
      setStatuslampe(GUL);
    }
    else
    {
      setStatuslampe(OFF);
    }
    if (game.tilstand == vunnet)
    {
      setStatuslampe(TURKIS);
    }
    else
    {
      setStatuslampe(OFF);
    }
    LedLysTidSistOppdatert = ledLysTidNaa;
  }
}

//Lysets farger:
void setStatuslampe(FargeRGB lystilstand)
{
  switch (lystilstand)
  {
  case TURKIS:
    analogWrite(redpinne, 0);
    analogWrite(greenpinne, 255); //ikke 0-127 gjelder alle
    analogWrite(bluepinne, 255);
    break;
  case GUL:
    analogWrite(redpinne, 255);
    analogWrite(greenpinne, 230);
    analogWrite(bluepinne, 0);
    break;
  case LILLA:
    analogWrite(redpinne, 230);
    analogWrite(greenpinne, 0);
    analogWrite(bluepinne, 255);
    break;
  case OFF:
    break;
  }
}

void SpillLyd()
{
  unsigned long lydTidNaa = millis();
  if (game.melodi == ingen)
  {
    return;
  }
  else
  {
    if (game.melodi == spillIntro)
    {
      Intro(lydTidNaa);
    }
    if (game.melodi == spillRett)
    {
      LydRett(lydTidNaa);
    }
    if (game.melodi == spillGalt)
    {
      LydFeil(lydTidNaa);
    }
  }
}

void LydRett(unsigned long lydTidNaa)
{
  if (noteCounter1 < 7)
  {
    unsigned long noteVarighet = 1000 / noteLengder1[noteCounter1]; // 1 sek, deles opp i 2,4,8 osv, der 1 sekund representerer helnoten

    if (lydTidNaa - lydTidStans > (noteVarighet * 1.3))
    {
      noTone(buzzer);
    }

    if (lydTidNaa - lydTidSist > noteVarighet)
    {
      tone(buzzer, lydrett[noteCounter1], noteVarighet); // sender lydrett ut på piezo, med riktig takt via PIN11
      lydTidStans = lydTidNaa;
      lydTidSist = lydTidNaa;
      noteCounter1++;
    }
  }
  else
  {
    resattMelodi();
  }
}

void LydFeil(unsigned long lydTidNaa)
{
  if (noteCounter2 < 6)
  {
    unsigned long noteVarighet = 1000 / noteLengder2[noteCounter2]; // 1 sek, deles opp i 2,4,8 osv, der 1 sekund representerer helnoten

    if (lydTidNaa - lydTidStans > (noteVarighet * 1.3))
    {
      noTone(buzzer);
    }

    if (lydTidNaa - lydTidSist > noteVarighet)
    {
      tone(buzzer, lydfeil[noteCounter2], noteVarighet); // sender lydrett ut på piezo, med riktig takt via PIN11
      lydTidStans = lydTidNaa;
      lydTidSist = lydTidNaa;
      noteCounter2++;
    }
  }
  else
  {
    resattMelodi();
  }
}

void Intro(unsigned long lydTidNaa)
{
  if (noteCounter3 < 36)
  {
    unsigned long noteVarighet = 1250 / noteLengder3[noteCounter3]; // 1 sek, deles opp i 2,4,8 osv, der 1 sekund representerer helnoten

    if (noteCounter3 != 0 && lydTidNaa - lydTidStans > (noteVarighet * 1.3))
    {
      noTone(buzzer);
    }

    if (lydTidNaa - lydTidSist > noteVarighet)
    {
      tone(buzzer, lydintro[noteCounter3], noteVarighet); // sender lydrett ut på piezo, med riktig takt via PIN11
      lydTidStans = lydTidNaa;
      lydTidSist = lydTidNaa;
      noteCounter3++;
    }
  }
  else
  {
    resattMelodi();
  }
}

// Melodien spiller hver gang man kaller de forskjellige melodifunksjonene.
void resattMelodi()
{
  noteCounter1 = 0;
  noteCounter2 = 0;
  noteCounter3 = 0;
  game.melodi = ingen;
}

void tilstanderForShield()
{
  //Bygger tilstand.
  if (digitalRead(PIN_BUTTON_UP) == LOW)
  {
    game.bluePressed = true;
  }
  else
  {
    game.bluePressed = false;
  }
  if (digitalRead(PIN_BUTTON_LEFT) == LOW)
  {
    game.yellowPressed = true;
  }
  else
  {
    game.yellowPressed = false;
  }
  if (digitalRead(PIN_BUTTON_DOWN) == LOW)
  {
    game.whitePressed = true;
  }
  else
  {
    game.whitePressed = false;
  }
  if (digitalRead(PIN_BUTTON_RIGHT) == LOW)
  {
    game.redPressed = true;
  }
  else
  {
    game.redPressed = false;
  }

  if (game.x < 500 || game.x > 530)
  {
    if (game.x > 530)
    {
      game.retning = right;
    }
    else
    {
      game.retning = left;
    }
  }
  else if (game.y < 500 || game.y > 530)
  {
    if (game.y > 530)
    {
      game.retning = up;
    }
    else
    {
      game.retning = down;
    }
  }
}

void TestModus()
{
  requiredMoves[0] = troll;
  requiredMoves[1] = down;
  requiredMoves[2] = up;
  requiredMoves[3] = troll;
  requiredMoves[4] = right;
  requiredMoves[5] = left;
}

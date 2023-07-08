#include <Arduino.h>
#include <U8g2lib.h>

#include "images.h"

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0); //[page buffer, size = 128 bytes]

struct menuItem {
  const unsigned char *iconBits;
  uint8_t iconWidth;
  uint8_t iconHeight;
  const char *labelText;
}; 

#define NUM_MENU_ITEMS 6
const menuItem menuItems[NUM_MENU_ITEMS] = {
    {icon_weld_bits, icon_weld_width, icon_weld_height, "Weld"},
    {icon_time_bits, icon_time_width, icon_time_height, "Set Delay"},
    {icon_power_bits, icon_power_width, icon_power_height, "Set Power"},
    {icon_manual_bits, icon_manual_width, icon_manual_height, "Manual Mode"},
    {icon_setting_bits, icon_setting_width, icon_setting_height, "Settings"},
    {icon_info_bits, icon_info_width, icon_info_height, "Information"}
};

#define BUTTON_UP 2
#define BUTTON_SELECT 3
#define BUTTON_DOWN 4

#define maxDelay 20000
#define minDelay 50


int item_curr = 0;
int item_prev;
int item_next;

int page = 0;

int currentSetDelayPageItem = 0;

unsigned int timeDelay = 1032;

bool button_up_clicked = false;
bool button_down_clicked = false;
bool button_select_clicked = false;



void drawMainMenu(menuItem item1, menuItem item2, menuItem item3) {
  //Draw first item
  u8g2.drawXBMP(2, 4, item1.iconWidth, item1.iconHeight, item1.iconBits);
  u8g2.setFont(u8g2_font_tenthinnerguys_tr);
  u8g2.drawStr(24, 16, item1.labelText);

  //Draw second item
  u8g2.drawXBMP(2, 24, item2.iconWidth, item2.iconHeight, item2.iconBits);
  u8g2.setFont(u8g2_font_tenthinguys_tr);
  u8g2.drawStr(24, 36, item2.labelText);

  //Draw selection box
  u8g2.drawXBMP(0, 21, img_text_selector_width, img_text_selector_height, img_text_selector_bits);

  //Draw third item
  u8g2.drawXBMP(2, 46, item3.iconWidth, item3.iconHeight, item3.iconBits);
  u8g2.setFont(u8g2_font_tenthinnerguys_tr);
  u8g2.drawStr(24, 58, item3.labelText);

  //Draw scroll bar
  u8g2.drawXBMP(120, 0, img_side_scroll_bar_width, img_side_scroll_bar_height, img_side_scroll_bar_bits);
  u8g2.drawBox(125, 64/NUM_MENU_ITEMS * item_curr, 3, 64/NUM_MENU_ITEMS);
}

void setup() {
  Serial.begin(9600);

  pinMode(BUTTON_UP, INPUT);
  pinMode(BUTTON_SELECT, INPUT);
  pinMode(BUTTON_DOWN, INPUT);

  u8g2.setColorIndex(1);
  u8g2.begin();
  u8g2.setBitmapMode(1);
}

void drawSetDelayPage(unsigned int currentDelay, int currentSelectItem) {
  struct selectButton {
    int position;
    char *label;
    int x_space;
    int y_space;
  };
  const selectButton selectButtons[7] = {
      {10, "-1k", 2, 2},
      {31, "-50", 2, 2},
      {50, "-1", 2, 2},
      {64, "\x0044", 1, 2},
      {79, "+1", 2, 2},
      {97, "+50", 2, 2},
      {118, "+1k", 2, 2}
  };
  
  u8g2.setFont(u8g2_font_tenthinguys_tr);
  u8g2.drawButtonUTF8(64, 9, U8G2_BTN_HCENTER|U8G2_BTN_BW0, 0, 0, 0, "Set Delay");

  u8g2.drawHLine(0, 12, 128);

  u8g2.setFont(u8g2_font_fub20_tr);
  String text = String(currentDelay) + " ms";
  u8g2.drawStr(4, 40, text.c_str());

  for (int i=0; i<7; i++) {
    selectButton currentButton = selectButtons[i];
    int flags = U8G2_BTN_HCENTER|U8G2_BTN_BW1;
    if ((i-3) == currentSelectItem) {flags = flags|U8G2_BTN_INV;}
    if ((i-3) == 0) {
      u8g2.setFont(u8g2_font_m2icon_7_tf);
    }
    else {
      u8g2.setFont(u8g2_font_5x8_tr);
    }
    u8g2.drawButtonUTF8(currentButton.position, 59, flags, 0, currentButton.x_space, currentButton.y_space, currentButton.label);
  }
}

void loop() {

  //Serial.println("page: " + (String)page);

  switch (page)
  {
    case 0:
      if (digitalRead(BUTTON_SELECT) == HIGH && !button_select_clicked) {
        button_select_clicked = true;
        page = item_curr + 1;
      }
      if (digitalRead(BUTTON_UP) == HIGH && !button_up_clicked) {
        button_up_clicked = true;
        item_curr--;
        if (item_curr < 0) {item_curr = NUM_MENU_ITEMS - 1;}
      }
      if (digitalRead(BUTTON_DOWN) == HIGH && !button_down_clicked) {
        button_down_clicked = true;
        item_curr++;
        if (item_curr >= NUM_MENU_ITEMS) {item_curr = 0;}
      }
      break;

    case 1:
        digitalWrite(7, HIGH);
        digitalWrite(8, HIGH);
        delay(500);
        digitalWrite(7, LOW); 
        digitalWrite(8, LOW);
        page = 0;
      break;
    
    case 2:
    //Handle inputs when on 
      if (digitalRead(BUTTON_DOWN) == HIGH && !button_down_clicked) {
        button_down_clicked = true;
        currentSetDelayPageItem++;
        if (currentSetDelayPageItem > 3) {currentSetDelayPageItem = 3;}
      }
      if (digitalRead(BUTTON_UP) == HIGH && !button_up_clicked) {
        button_up_clicked = true;
        currentSetDelayPageItem--;
        if (currentSetDelayPageItem < -3) {currentSetDelayPageItem = -3;}
      }
      if (digitalRead(BUTTON_SELECT) == HIGH) {
        int changeDelay = 0;
        switch (currentSetDelayPageItem){
          case -3:
            changeDelay = -1000;
            break;
          case -2:
            changeDelay = -50;
            break;
          case -1:
            changeDelay = -1;
            break;
          case 0:
            if (!button_select_clicked){
              page = 0; 
              item_curr = 1;
            }
            break;
          case 1:
            changeDelay = 1;
            break;
          case 2:
            changeDelay = 50;
            break;
          case 3:
            changeDelay = 1000;
            break;
        }
        
        if (changeDelay < 0 && timeDelay < (abs(changeDelay)+minDelay)) {timeDelay = minDelay;}
        else if (timeDelay + changeDelay > maxDelay) {timeDelay = maxDelay;}
        else { timeDelay = timeDelay + changeDelay;}

        button_select_clicked = true;
      }
      break;

    default:
      if (digitalRead(BUTTON_SELECT) == HIGH && !button_select_clicked) {
        button_select_clicked = true;
        page = 0;
      }
      break;
  }



  // Reset buttons if they are released
  if (digitalRead(BUTTON_UP) == LOW && button_up_clicked) {
    button_up_clicked = false;
  }
  if (digitalRead(BUTTON_DOWN) == LOW && button_down_clicked) {
    button_down_clicked = false;
  }

  if (digitalRead(BUTTON_SELECT) == LOW && button_select_clicked) {
    button_select_clicked = false;
  }

  //Update Main Menu items
  item_prev = item_curr - 1;
  if (item_prev < 0) {item_prev = NUM_MENU_ITEMS - 1;}
  item_next = item_curr + 1;
  if (item_next >= NUM_MENU_ITEMS) {item_next = 0;}



  u8g2.firstPage();
  do {
    switch (page)
    {
      case 0:
        drawMainMenu(menuItems[item_prev], menuItems[item_curr], menuItems[item_next]);
        break;
      
      case 2:
        drawSetDelayPage(timeDelay, currentSetDelayPageItem);
        break;

      default:
        u8g2.setFont(u8g2_font_tenthinguys_tr);
        u8g2.drawButtonUTF8(64, 9, U8G2_BTN_HCENTER|U8G2_BTN_BW0, 0, 0, 0, menuItems[item_curr].labelText);

        u8g2.drawHLine(0, 12, 128);
        break;
    }
  } while ( u8g2.nextPage() );
}


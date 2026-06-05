#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "display.h"
#include "config.h"
#include "state.h"

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

void configurarDisplay() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Abrigo IoT");
  lcd.setCursor(0, 1);
  lcd.print("Iniciando...");
  delay(1200);
}

void atualizarDisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ocupacao:");
  lcd.setCursor(10, 0);
  lcd.print(ocupacaoAtual);
  lcd.print("/");
  lcd.print(CAPACIDADE_MAXIMA);

  lcd.setCursor(0, 1);
  lcd.print(getStatusAbrigo());
}

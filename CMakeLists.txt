idf_component_register(
	SRCS "mch2022_keyb.c" "pax-keyboard/pax_keyboard.c"
	INCLUDE_DIRS . "pax-keyboard"
	REQUIRES pax-graphics spi-ili9341 bus-i2c esp32-component-i2c-pca9555 esp_rom
)

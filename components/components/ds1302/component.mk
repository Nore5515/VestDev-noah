COMPONENT_ADD_INCLUDEDIRS = .

ifdef CONFIG_IDF_TARGET_ESP8266
COMPONENT_DEPENDS = esp8266 freertos log esp_idf_lib_helpers
else
COMPONENT_DEPENDS = driver freertos log esp_idf_lib_helpers
endif

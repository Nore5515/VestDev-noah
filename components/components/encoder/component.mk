COMPONENT_ADD_INCLUDEDIRS = .

ifdef CONFIG_IDF_TARGET_ESP8266
COMPONENT_DEPENDS = esp8266 freertos log
else
COMPONENT_DEPENDS = driver freertos log
endif

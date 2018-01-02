deps_config := \
	/home/xukai/esp/esp-idf/components/app_trace/Kconfig \
	/home/xukai/esp/esp-idf/components/aws_iot/Kconfig \
	/home/xukai/esp/esp-idf/components/bt/Kconfig \
	/home/xukai/esp/esp-idf/components/esp32/Kconfig \
	/home/xukai/esp/esp-idf/components/ethernet/Kconfig \
	/home/xukai/esp/esp-idf/components/fatfs/Kconfig \
	/home/xukai/esp/esp-idf/components/freertos/Kconfig \
	/home/xukai/esp/esp-idf/components/heap/Kconfig \
	/home/xukai/esp/esp-idf/components/libsodium/Kconfig \
	/home/xukai/esp/esp-idf/components/log/Kconfig \
	/home/xukai/esp/esp-idf/components/lwip/Kconfig \
	/home/xukai/esp/esp-idf/components/mbedtls/Kconfig \
	/home/xukai/esp/esp-idf/components/openssl/Kconfig \
	/home/xukai/esp/esp-idf/components/pthread/Kconfig \
	/home/xukai/esp/esp-idf/components/spi_flash/Kconfig \
	/home/xukai/esp/esp-idf/components/spiffs/Kconfig \
	/home/xukai/esp/esp-idf/components/tcpip_adapter/Kconfig \
	/home/xukai/esp/esp-idf/components/wear_levelling/Kconfig \
	/home/xukai/esp/esp-idf/components/bootloader/Kconfig.projbuild \
	/home/xukai/esp/esp-idf/components/esptool_py/Kconfig.projbuild \
	/home/xukai/esp/ESP32_PULSE/main/Kconfig.projbuild \
	/home/xukai/esp/esp-idf/components/partition_table/Kconfig.projbuild \
	/home/xukai/esp/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)


$(deps_config): ;

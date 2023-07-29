TARGET=""
SECONDS_SINCE_LAST_BUILD = $(shell echo "$$(date +%s) - $$(stat -c %Y build/esp8266.esp8266.nodemcuv2/QDLED.ino.bin)" | bc)
NEWER_COUNT= $(shell find . -maxdepth 0 -newermt '$(SECONDS_SINCE_LAST_BUILD) seconds ago'| wc -l)


b:
	arduino-cli compile -b esp8266:esp8266:nodemcuv2 -e .

update:
	echo $(SECONDS_SINCE_LAST_BUILD)
	if [ $(NEWER_COUNT) -ne 0 ] ; then $(MAKE) b; fi
	$(MAKE) push

push:
	arduino-cli upload -p ${TARGET}  -b esp8266:esp8266:nodemcuv2 --board-options="baud=74880" --board-options="eesz=autoflash" .


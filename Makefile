CC = gcc
CFLAGS = -Wall -O2
TARGET = backlight-ctl
SOURCE = backlight-ctl.c

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

install: $(TARGET)
	sudo cp $(TARGET) /usr/local/bin/
	sudo chmod +x /usr/local/bin/$(TARGET)
	sudo cp backlight-config /usr/local/bin/
	sudo cp backlight-control-panel /usr/local/bin/
	sudo chmod +x /usr/local/bin/backlight-config
	sudo chmod +x /usr/local/bin/backlight-control-panel
	sudo cp backlight-ctl.1 /usr/share/man/man1/
	sudo mandb -q

service: install
	sudo cp backlight-control.service /etc/systemd/system/
	sudo systemctl daemon-reload
	sudo systemctl enable backlight-control.service
	@echo "Service installed. Start with: sudo systemctl start backlight-control"

full-install:
	./install-service.sh

uninstall:
	sudo systemctl stop backlight-control 2>/dev/null || true
	sudo systemctl disable backlight-control 2>/dev/null || true
	sudo rm -f /etc/systemd/system/backlight-control.service
	sudo rm -f /usr/local/bin/backlight-ctl
	sudo rm -f /usr/share/man/man1/backlight-ctl.1
	sudo rm -f /etc/udev/rules.d/90-backlight.rules
	sudo rm -f /etc/sudoers.d/backlight-control
	sudo systemctl daemon-reload
	sudo mandb -q
	@echo "Uninstall complete"

clean:
	rm -f $(TARGET)

.PHONY: all install service full-install uninstall clean
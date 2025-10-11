#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/epoll.h>
#include <linux/input.h>
#include <dirent.h>
#include <errno.h>


#define VERSION "1.0.0"
#define BACKLIGHT_PATH "/sys/class/leds/tpacpi::kbd_backlight/brightness"
#define CONFIG_PATH "/etc/backlight-control.conf"
#define USER_CONFIG_PATH "/.config/backlight-control.conf"

#define MAX_INPUT_DEVICES 32

typedef struct {
    int enabled;
    int battery_timeout;
    int ac_timeout;
    int ac_default_level;
    int battery_default_level;
} config_t;

typedef enum {
    AUTO_DISABLED = 0,
    AUTO_ENABLED = 1
} auto_mode_t;

static config_t config = {1, 30, 120, 2, 1};
static auto_mode_t auto_mode = AUTO_ENABLED;
static int running = 1;
static int debug_mode = 0;
static int config_changed = 1;
static time_t last_activity_time;
static time_t last_power_check = 0;
static int cached_power_status = 0;
static unsigned long last_interrupt_count = 0;

void load_config() {
    FILE *f;
    char config_path[256];
    
    // Try user config first
    snprintf(config_path, sizeof(config_path), "%s%s", getenv("HOME") ?: "", USER_CONFIG_PATH);
    f = fopen(config_path, "r");
    
    if (!f) {
        f = fopen(CONFIG_PATH, "r");
    }
    
    if (f) {
        fscanf(f, "enabled=%d\nbattery_timeout=%d\nac_timeout=%d\nac_default_level=%d\nbattery_default_level=%d\n",
               &config.enabled, &config.battery_timeout, &config.ac_timeout, &config.ac_default_level, &config.battery_default_level);
        fclose(f);
    }
    
    auto_mode = config.enabled ? AUTO_ENABLED : AUTO_DISABLED;
}

void save_config() {
    FILE *f;
    char config_path[256];
    char config_dir[256];
    
    snprintf(config_dir, sizeof(config_dir), "%s/.config", getenv("HOME") ?: "");
    mkdir(config_dir, 0755);
    
    snprintf(config_path, sizeof(config_path), "%s%s", getenv("HOME") ?: "", USER_CONFIG_PATH);
    f = fopen(config_path, "w");
    
    if (f) {
        fprintf(f, "enabled=%d\nbattery_timeout=%d\nac_timeout=%d\nac_default_level=%d\nbattery_default_level=%d\n",
                config.enabled, config.battery_timeout, config.ac_timeout, config.ac_default_level, config.battery_default_level);
        fclose(f);
    }
}

int get_backlight_level() {
    FILE *f = fopen(BACKLIGHT_PATH, "r");
    int level = 0;
    if (f) {
        fscanf(f, "%d", &level);
        fclose(f);
    }
    return level;
}

static time_t last_daemon_change = 0;

void mark_daemon_backlight_change() {
    last_daemon_change = time(NULL);
}

void set_backlight_level(int level) {
    FILE *f = fopen(BACKLIGHT_PATH, "w");
    if (f) {
        int result = fprintf(f, "%d", level);
        fclose(f);
        if (debug_mode) {
            printf("Write to %s: level=%d, result=%d\n", BACKLIGHT_PATH, level, result);
        }
    } else {
        if (debug_mode) {
            printf("Failed to open %s for writing\n", BACKLIGHT_PATH);
        }
    }
    mark_daemon_backlight_change();
}

int is_on_ac_power() {
    time_t now = time(NULL);
    
    // Cache power status for 10 seconds
    if (now - last_power_check >= 10) {
        FILE *f = fopen("/sys/class/power_supply/AC/online", "r");
        if (!f) f = fopen("/sys/class/power_supply/ADP1/online", "r");
        if (!f) f = fopen("/sys/class/power_supply/AC0/online", "r");
        
        if (f) {
            fscanf(f, "%d", &cached_power_status);
            fclose(f);
        }
        last_power_check = now;
    }
    
    return cached_power_status;
}

unsigned long get_interrupt_activity() {
    FILE *f = fopen("/proc/interrupts", "r");
    if (!f) return 0;
    
    char line[256];
    unsigned long total = 0;
    
    while (fgets(line, sizeof(line), f)) {
        if (strstr(line, "i8042")) {
            char *colon = strchr(line, ':');
            if (colon) {
                char *ptr = colon + 1;
                while (*ptr == ' ' || *ptr == '\t') ptr++;
                
                while (*ptr && *ptr >= '0' && *ptr <= '9') {
                    total += strtoul(ptr, &ptr, 10);
                    while (*ptr == ' ' || *ptr == '\t') ptr++;
                }
            }
        }
    }
    
    fclose(f);
    return total;
}





void signal_handler(int sig) {
    if (sig == SIGUSR1) {
        config_changed = 1;
        if (debug_mode) printf("Configuration reload requested\n");
    } else {
        running = 0;
    }
}

void daemon_loop() {
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGUSR1, signal_handler);
    
    printf("Backlight daemon started (PID: %d)\n", getpid());
    printf("Battery timeout: %ds, AC timeout: %ds\n", config.battery_timeout, config.ac_timeout);
    
    last_activity_time = time(NULL);
    last_interrupt_count = get_interrupt_activity();
    set_backlight_level(0); // Start with backlight off
    
    while (running) {
        // Only reload config when changed
        if (config_changed) {
            load_config();
            config_changed = 0;
            if (debug_mode) printf("Configuration reloaded\n");
        }
        
        if (auto_mode == AUTO_DISABLED) {
            if (debug_mode) printf("Auto-mode disabled (0), sleeping...\n");
            sleep(5);
            continue;
        }
        

        

        
        unsigned long current_activity = get_interrupt_activity();
        time_t now = time(NULL);
        int on_ac = is_on_ac_power();
        int timeout = on_ac ? config.ac_timeout : config.battery_timeout;
        int default_level = on_ac ? config.ac_default_level : config.battery_default_level;
        
        // Debug output every 5 seconds
        if (debug_mode) {
            static time_t last_debug = 0;
            if (now - last_debug >= 5) {
                printf("Activity: %lu (last: %lu), Power: %s, Timeout: %ds, Elapsed: %lds, Auto-mode: %d\n", 
                       current_activity, last_interrupt_count, on_ac ? "AC" : "Battery", timeout, 
                       now - last_activity_time, auto_mode);
                last_debug = now;
            }
        }
        
        if (current_activity != last_interrupt_count) {
            // Activity detected
            if (debug_mode) {
                printf("Activity detected: %lu -> %lu, setting level %d\n", 
                       last_interrupt_count, current_activity, default_level);
            }
            set_backlight_level(default_level);
            last_activity_time = now;
            last_interrupt_count = current_activity;
        } else {
            time_t elapsed = now - last_activity_time;
            
            if (on_ac && config.ac_default_level == 2) {
                // Gradual dimming on AC with level 2
                int half_timeout = timeout / 2;
                if (elapsed > timeout) {
                    if (debug_mode) printf("Timeout reached (%lds), turning off\n", elapsed);
                    set_backlight_level(0);
                } else if (elapsed > half_timeout) {
                    if (debug_mode) printf("Half timeout reached (%lds), dimming to level 1\n", elapsed);
                    set_backlight_level(1);
                }
            } else if (elapsed > timeout) {
                // Standard timeout
                if (debug_mode) printf("Timeout reached (%lds), turning off\n", elapsed);
                set_backlight_level(0);
            }
        }
        
        sleep(1);
    }
    
    printf("Daemon stopping\n");
}

void print_status() {
    load_config();
    
    printf("Backlight Control Status:\n");
    printf("  Current Level: %d\n", get_backlight_level());
    printf("  Power Source: %s\n", is_on_ac_power() ? "AC Power" : "Battery");
    printf("  Auto Mode: %s\n", 
           auto_mode == AUTO_ENABLED ? "Enabled" : "Disabled");
    printf("  Battery Timeout: %ds\n", config.battery_timeout);
    printf("  AC Timeout: %ds\n", config.ac_timeout);
    printf("  AC Default Level: %d\n", config.ac_default_level);
    printf("  Battery Default Level: %d\n", config.battery_default_level);
}

void print_usage() {
    printf("Usage: backlight-ctl <command> [args]\n");
    printf("Commands:\n");
    printf("  daemon [--debug]    Run as daemon (--debug for verbose output)\n");
    printf("  set <0|1|2>         Set backlight level\n");
    printf("  get                 Get current level\n");
    printf("  status              Show status\n");
    printf("  enable              Enable auto-backlight\n");
    printf("  disable             Disable auto-backlight\n");
    printf("  config <key> <val>  Set config (battery_timeout, ac_timeout, ac_default_level, battery_default_level)\n");
    printf("  --version           Show version information\n");
    printf("  --help              Show this help message\n");
}

void print_version() {
    printf("backlight-ctl %s\n", VERSION);
    printf("Keyboard backlight control daemon for Linux laptops\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }
    
    // Handle version and help before loading config
    if (strcmp(argv[1], "--version") == 0) {
        print_version();
        return 0;
    }
    
    if (strcmp(argv[1], "--help") == 0) {
        print_usage();
        return 0;
    }
    
    load_config();
    
    if (strcmp(argv[1], "daemon") == 0) {
        // Check for --debug flag
        if (argc > 2 && strcmp(argv[2], "--debug") == 0) {
            debug_mode = 1;
        }
        daemon_loop();
    }
    else if (strcmp(argv[1], "set") == 0 && argc == 3) {
        int level = atoi(argv[2]);
        if (level >= 0 && level <= 2) {
            set_backlight_level(level);
            printf("Backlight set to level %d\n", level);
        } else {
            printf("Invalid level. Use 0, 1, or 2\n");
            return 1;
        }
    }
    else if (strcmp(argv[1], "get") == 0) {
        printf("%d\n", get_backlight_level());
    }
    else if (strcmp(argv[1], "status") == 0) {
        print_status();
    }
    else if (strcmp(argv[1], "enable") == 0) {
        config.enabled = 1;
        save_config();
        printf("Auto-backlight enabled\n");
    }
    else if (strcmp(argv[1], "disable") == 0) {
        config.enabled = 0;
        save_config();
        printf("Auto-backlight disabled\n");
    }

    else if (strcmp(argv[1], "config") == 0 && argc == 4) {
        if (strcmp(argv[2], "battery_timeout") == 0) {
            config.battery_timeout = atoi(argv[3]);
        } else if (strcmp(argv[2], "ac_timeout") == 0) {
            config.ac_timeout = atoi(argv[3]);
        } else if (strcmp(argv[2], "ac_default_level") == 0) {
            config.ac_default_level = atoi(argv[3]);
        } else if (strcmp(argv[2], "battery_default_level") == 0) {
            config.battery_default_level = atoi(argv[3]);
        } else {
            printf("Unknown config key: %s\n", argv[2]);
            return 1;
        }
        save_config();
        printf("Configuration updated\n");
    }
    else {
        print_usage();
        return 1;
    }
    
    return 0;
}
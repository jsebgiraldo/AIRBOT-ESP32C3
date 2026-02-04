# ⚡ AIRBOT ESP32-C3 - Comandos Rápidos

Referencia rápida de comandos para desarrollo con ESP32-C3.

## 🔧 Setup Inicial

```bash
# Clonar repositorio
git clone https://github.com/jsebgiraldo/AIRBOT-ESP32C3.git
cd AIRBOT-ESP32C3

# Configurar WiFi
cp sdkconfig.defaults.example sdkconfig.defaults
nano sdkconfig.defaults  # Editar SSID y password

# Configurar target ESP32-C3
idf.py set-target esp32c3
```

## 🔨 Build & Flash

### Primera Vez (USB)
```bash
# Build + Flash + Monitor (todo en uno)
idf.py build flash monitor

# O paso por paso
idf.py build           # Solo compilar
idf.py flash          # Solo flashear
idf.py monitor        # Solo monitor
```

### Desarrollo (OTA WiFi)
```bash
# Build
idf.py build

# Flash OTA (reemplaza <IP> con la IP de tu ESP32)
python ota_upload.py <IP> build/airbot-esp32c3.bin

# Ejemplo
python ota_upload.py 192.168.1.100 build/airbot-esp32c3.bin
```

## 📡 OTA Commands

### Ver versión actual
```bash
curl http://<IP>:8080/version
```

### Upload binario directo
```bash
curl -X POST --data-binary @build/airbot-esp32c3.bin http://<IP>:8080/upload
```

### OTA desde URL
```bash
curl -X POST -d "http://example.com/firmware.bin" http://<IP>:8080/ota
```

### Usando scripts Python
```bash
# Upload directo
python ota_upload.py <IP> build/airbot-esp32c3.bin

# Desde URL
python ota_update.py <IP> http://example.com/firmware.bin
```

## 🔍 Debugging

### Monitor serial
```bash
# Monitor básico
idf.py monitor

# Monitor con filtro
idf.py monitor | grep -E "(ERROR|WARNING|OTA)"

# Salir del monitor: Ctrl+]
```

### Ver logs específicos
```bash
# Solo errores
idf.py monitor | grep "ERROR"

# Solo OTA
idf.py monitor | grep "ota_manager"

# Solo WiFi
idf.py monitor | grep "wifi_manager"
```

## 🧹 Limpieza

```bash
# Limpiar build completo
idf.py fullclean

# Limpiar solo build artifacts
idf.py clean

# Rebuild completo
idf.py fullclean && idf.py build
```

## 📦 Configuración

```bash
# Abrir menuconfig
idf.py menuconfig

# Ver configuración actual
cat sdkconfig

# Ver solo configuraciones custom
cat sdkconfig.defaults
```

## 🔌 Hardware

### Detectar puerto USB
```bash
# Windows
mode

# Linux/Mac
ls /dev/tty*
```

### Flash con puerto específico
```bash
idf.py -p COM3 flash          # Windows
idf.py -p /dev/ttyUSB0 flash  # Linux
idf.py -p /dev/cu.usbserial flash  # Mac
```

## 🌐 Network

### Encontrar ESP32 en la red
```bash
# Escanear red (Linux/Mac)
nmap -sn 192.168.1.0/24

# Ver ARP table (Windows)
arp -a | findstr "espressif"

# Ver ARP table (Linux/Mac)
arp -a | grep -i "espressif"
```

### Ping ESP32
```bash
ping <IP>
```

## 📊 Información del Sistema

### Ver tamaño del firmware
```bash
# Linux/Mac
ls -lh build/airbot-esp32c3.bin

# Windows
Get-Item build/airbot-esp32c3.bin | Select-Object Length, Name
```

### Ver uso de memoria
```bash
idf.py size
idf.py size-components
idf.py size-files
```

### Ver tabla de particiones
```bash
idf.py partition-table
```

## 🐛 Troubleshooting

### Reset ESP32
```bash
# Desde código - agregar al main.c
esp_restart();

# Hardware - presiona el botón RESET
```

### Borrar NVS
```bash
idf.py erase-flash
```

### Verificar conexión USB
```bash
# Windows - listar puertos COM
mode

# Linux
dmesg | grep tty

# Mac
ls /dev/cu.*
```

## 🚀 Workflow Rápido

### Desarrollo Iterativo
```bash
# Terminal 1 - Monitor (mantener abierto)
idf.py monitor

# Terminal 2 - Build & Flash loop
while true; do
  idf.py build && python ota_upload.py 192.168.1.100 build/airbot-esp32c3.bin
  sleep 5
done
```

### Un solo comando
```bash
# Build + OTA + Monitor
idf.py build && python ota_upload.py 192.168.1.100 build/airbot-esp32c3.bin && idf.py monitor
```

## 📝 Git Workflow

```bash
# Estado actual
git status

# Agregar cambios
git add .

# Commit
git commit -m "Descripción del cambio"

# Push
git push

# Pull cambios
git pull
```

## 🔐 Variables de Entorno (Opcional)

Configura en tu shell profile (`.bashrc`, `.zshrc`, `PowerShell profile`):

```bash
# Bash/Zsh
export ESP32_IP="192.168.1.100"
export IDF_TARGET="esp32c3"

# Usar
python ota_upload.py $ESP32_IP build/airbot-esp32c3.bin

# PowerShell
$env:ESP32_IP = "192.168.1.100"
python ota_upload.py $env:ESP32_IP build/airbot-esp32c3.bin
```

## 📚 Aliases Útiles

Agrega a tu shell profile:

```bash
# Bash/Zsh
alias esp-build="idf.py build"
alias esp-flash="idf.py flash"
alias esp-monitor="idf.py monitor"
alias esp-clean="idf.py fullclean"
alias esp-ota="python ota_upload.py \$ESP32_IP build/airbot-esp32c3.bin"
alias esp-dev="idf.py build && python ota_upload.py \$ESP32_IP build/airbot-esp32c3.bin"

# PowerShell
function esp-build { idf.py build }
function esp-flash { idf.py flash }
function esp-monitor { idf.py monitor }
function esp-clean { idf.py fullclean }
function esp-ota { python ota_upload.py $env:ESP32_IP build/airbot-esp32c3.bin }
function esp-dev { idf.py build; python ota_upload.py $env:ESP32_IP build/airbot-esp32c3.bin }
```

---

## 💡 Tips

1. **Siempre** deja el monitor corriendo para ver logs en tiempo real
2. **Nunca** hagas commit de `sdkconfig.defaults` con credenciales
3. **Verifica** la versión del firmware antes de OTA masivo
4. **Mantén** un firmware funcional como backup
5. **Usa** el rollback automático si algo falla

## 🆘 Help

```bash
# Ver ayuda de idf.py
idf.py --help

# Ver comandos disponibles
idf.py --list-targets

# Ver versión de IDF
idf.py --version
```

---

**Nota**: Reemplaza `<IP>` con la IP real de tu ESP32-C3 en todos los comandos.

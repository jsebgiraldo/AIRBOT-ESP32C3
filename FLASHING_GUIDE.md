# 🚀 Guía Rápida: Flashing ESP32-C3 con MCPs

## 📌 Flujo de Trabajo Recomendado

### 1️⃣ Primer Flash (USB - Una sola vez)

El primer flash **debe** hacerse por USB para cargar el bootloader y las particiones:

```bash
# Configurar target
idf.py set-target esp32c3

# Compilar
idf.py build

# Flash completo
idf.py flash monitor
```

**Importante**: Antes de flashear, configura tu WiFi en `sdkconfig.defaults`:
```
CONFIG_ESP_WIFI_SSID="TuRedWiFi"
CONFIG_ESP_WIFI_PASSWORD="TuContraseña"
```

### 2️⃣ Actualizaciones Posteriores (OTA - Inalámbrico)

Una vez que el ESP32-C3 está conectado a WiFi, **todas** las actualizaciones subsecuentes se hacen por WiFi usando MCPs:

#### Opción A: Script Python Directo (Recomendado)

```bash
# Compilar nueva versión
idf.py build

# Upload por OTA
python ota_upload.py <IP_DEL_ESP32> build/airbot-esp32c3.bin
```

#### Opción B: MCP Code-Mode (Automatización Total)

Puedes crear un script MCP que combine compilación y OTA:

```javascript
// Usar code-mode para crear un script automatizado
// Ejemplo: compilar + buscar ESP32 + flashear
```

## 🔧 Comandos Útiles

### Ver IP del ESP32
Conecta el monitor serial al iniciar:
```bash
idf.py monitor
```

Busca líneas como:
```
I (12345) main: Connected! IP Address: 192.168.1.100
I (12346) main: OTA Update endpoints:
I (12347) main:   - POST http://192.168.1.100:8080/upload
```

### Compilar sin Flash
```bash
idf.py build
```

### Limpiar Build
```bash
idf.py fullclean
```

### Ver Particiones
```bash
idf.py partition-table
```

## 📡 Endpoints OTA Disponibles

Una vez el ESP32 esté en WiFi:

### 1. Verificar Versión
```bash
curl http://<IP>:8080/version
```

### 2. Upload Binario Directo
```bash
curl -X POST --data-binary @build/airbot-esp32c3.bin http://<IP>:8080/upload
```

### 3. OTA desde URL
```bash
curl -X POST -d "http://example.com/firmware.bin" http://<IP>:8080/ota
```

## 🎯 Flujo Completo de Desarrollo

### Primera Vez
```bash
# 1. Configurar WiFi
nano sdkconfig.defaults  # Editar SSID y password

# 2. Configurar target
idf.py set-target esp32c3

# 3. Compilar
idf.py build

# 4. Flash USB (primera vez)
idf.py flash monitor

# 5. Anotar la IP que aparece en el monitor
```

### Desarrollo Diario
```bash
# 1. Hacer cambios en el código
# 2. Compilar
idf.py build

# 3. Flash OTA (¡Sin cables!)
python ota_upload.py 192.168.1.100 build/airbot-esp32c3.bin

# 4. Ver logs (opcional)
idf.py monitor
```

## 🔍 Troubleshooting

### ❌ "No se puede conectar al ESP32"
- Verifica que esté en la misma red WiFi
- Comprueba la IP con `idf.py monitor`
- Asegúrate que el puerto 8080 no esté bloqueado

### ❌ "OTA falla"
- Verifica que el firmware no sea mayor a 1MB
- Comprueba que haya suficiente espacio en la partición OTA
- Revisa los logs con `idf.py monitor`

### ❌ "El ESP32 no arranca después de OTA"
- El rollback automático debería restaurar la versión anterior
- Si no funciona, reflashea por USB

## 💡 Tips Pro

### 1. Usar GitHub Actions
El proyecto incluye un workflow que compila automáticamente en cada push. Descarga el artifact y flashea por OTA.

### 2. Servidor de Firmware Local
```bash
# Servir firmware localmente
python -m http.server 8000

# Usar URL local para OTA
python ota_update.py 192.168.1.100 http://tu-pc:8000/build/airbot-esp32c3.bin
```

### 3. Múltiples ESP32
Crea un script para actualizar múltiples dispositivos:
```bash
#!/bin/bash
for ip in 192.168.1.100 192.168.1.101 192.168.1.102; do
    echo "Updating $ip..."
    python ota_upload.py $ip build/airbot-esp32c3.bin
done
```

## 🎁 Ventajas del Sistema OTA

✅ **Sin cables**: Actualiza desde cualquier lugar de la red  
✅ **Rápido**: 30-60 segundos por actualización  
✅ **Seguro**: Rollback automático si falla  
✅ **Múltiple**: Actualiza varios dispositivos a la vez  
✅ **CI/CD**: Integrable con pipelines automáticos  

## 📚 Referencias

- [README Principal](README.md)
- [Documentación ESP-IDF OTA](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/api-reference/system/ota.html)
- [Repositorio GitHub](https://github.com/jsebgiraldo/AIRBOT-ESP32C3)

# AIRBOT ESP32-C3 Firmware

Firmware para ESP32-C3 con soporte de actualización OTA por WiFi.

## � Documentación

- **[Guía Rápida de Flashing](FLASHING_GUIDE.md)** - Instrucciones detalladas para flash USB y OTA WiFi
- **[README](README.md)** - Este archivo (documentación completa)

## �🚀 Características

- **ESP32-C3**: Optimizado para ESP32-C3
- **OTA WiFi**: Actualización de firmware inalámbrica
- **Gestión WiFi**: Conexión automática y reconexión
- **HTTP Server**: Servidor integrado para OTA
- **Rollback**: Soporte para revertir actualizaciones fallidas
- **Particiones duales**: Factory + OTA_0 + OTA_1 para actualizaciones seguras

## 📋 Requisitos

- ESP-IDF v5.0 o superior
- Python 3.8+
- ESP32-C3 DevKit

## 🔧 Configuración Inicial

### 1. Clonar el Repositorio

```bash
git clone https://github.com/jsebgiraldo/AIRBOT-ESP32C3.git
cd AIRBOT-ESP32C3
```

### 2. Configurar WiFi

Edita `sdkconfig.defaults` o usa menuconfig:

```bash
idf.py menuconfig
```

Navega a: `Component config → WiFi Configuration` y configura:
- WiFi SSID: Tu red WiFi
- WiFi Password: Tu contraseña

O edita directamente `sdkconfig.defaults`:
```
CONFIG_ESP_WIFI_SSID="TuRedWiFi"
CONFIG_ESP_WIFI_PASSWORD="TuContraseña"
```

### 3. Configurar Target ESP32-C3

```bash
idf.py set-target esp32c3
```

## 📦 Compilación

```bash
idf.py build
```

## 🔌 Primer Flash (USB)

El primer flash debe hacerse por USB usando el cable:

```bash
# Flash completo con bootloader y particiones
idf.py flash monitor

# O solo flash
idf.py flash
```

**Nota**: Presiona y mantén el botón BOOT en el ESP32-C3 durante la conexión USB si no se detecta automáticamente.

## 📡 Actualizaciones OTA (WiFi)

Después del primer flash USB, todas las actualizaciones posteriores pueden hacerse por WiFi.

### Método 1: Upload Directo del Binario

```bash
# Desde el directorio del proyecto
python ota_upload.py <IP_DEL_ESP32> build/airbot-esp32c3.bin
```

Ejemplo:
```bash
python ota_upload.py 192.168.1.100 build/airbot-esp32c3.bin
```

### Método 2: OTA desde URL

```bash
# Actualizar desde una URL remota
python ota_update.py <IP_DEL_ESP32> <URL_DEL_FIRMWARE>
```

Ejemplo:
```bash
python ota_update.py 192.168.1.100 http://example.com/firmware.bin
```

### Método 3: curl (Manual)

```bash
# Upload directo
curl -X POST --data-binary @build/airbot-esp32c3.bin http://<IP_DEL_ESP32>:8080/upload

# Desde URL
curl -X POST -d "http://example.com/firmware.bin" http://<IP_DEL_ESP32>:8080/ota
```

## 🌐 Endpoints HTTP

Una vez conectado a WiFi, el ESP32-C3 expone estos endpoints:

- **GET /version**: Obtener versión actual del firmware
  ```bash
  curl http://<IP_DEL_ESP32>:8080/version
  ```

- **POST /upload**: Subir firmware binario directamente
  ```bash
  curl -X POST --data-binary @firmware.bin http://<IP_DEL_ESP32>:8080/upload
  ```

- **POST /ota**: Actualizar desde URL
  ```bash
  curl -X POST -d "http://server.com/firmware.bin" http://<IP_DEL_ESP32>:8080/ota
  ```

## 🗂️ Estructura del Proyecto

```
AIRBOT-ESP32C3/
├── main/
│   ├── main.c              # Aplicación principal
│   ├── wifi_manager.c      # Gestión WiFi
│   ├── wifi_manager.h
│   └── CMakeLists.txt
├── components/
│   └── ota_manager/        # Componente OTA personalizado
│       ├── ota_manager.c
│       ├── include/
│       │   └── ota_manager.h
│       └── CMakeLists.txt
├── partitions.csv          # Tabla de particiones
├── sdkconfig.defaults      # Configuración por defecto
├── CMakeLists.txt
├── ota_upload.py           # Script para OTA upload
├── ota_update.py           # Script para OTA desde URL
└── README.md
```

## 🔐 Particiones

El proyecto usa una tabla de particiones personalizada con:

- **nvs**: 24 KB - Almacenamiento no volátil
- **phy_init**: 4 KB - Inicialización PHY
- **factory**: 1 MB - Firmware de fábrica
- **ota_0**: 1 MB - Partición OTA primaria
- **ota_1**: 1 MB - Partición OTA secundaria
- **ota_data**: 8 KB - Datos de OTA
- **storage**: 952 KB - SPIFFS para datos

## 🔄 Proceso de Actualización OTA

1. **Primera vez**: Flash USB con `idf.py flash`
2. ESP32-C3 se conecta a WiFi y muestra su IP
3. **Actualizaciones subsecuentes**: Usar scripts Python o curl
4. El firmware se descarga a la partición OTA inactiva
5. Verificación del firmware
6. Cambio de partición de boot
7. Reinicio automático
8. Si falla, rollback automático a la versión anterior

## 🛠️ Desarrollo

### Incrementar Versión

Edita `FIRMWARE_VERSION` en [components/ota_manager/ota_manager.c](components/ota_manager/ota_manager.c):

```c
#define FIRMWARE_VERSION "1.0.1"  // Cambiar aquí
```

### Compilar y Actualizar Vía OTA

```bash
# Compilar
idf.py build

# Encontrar IP del ESP32 (ver monitor serial o router)
# Actualizar por OTA
python ota_upload.py <IP> build/airbot-esp32c3.bin
```

## 🐛 Debugging

### Ver logs en tiempo real

```bash
idf.py monitor
```

### Ver solo logs OTA

```bash
idf.py monitor | grep -E "(ota_manager|OTA)"
```

### Limpiar build

```bash
idf.py fullclean
idf.py build
```

## ⚡ Troubleshooting

**Problema**: ESP32 no se conecta a WiFi
- Verifica SSID y password en `sdkconfig.defaults`
- Comprueba que el router esté en 2.4GHz (ESP32-C3 no soporta 5GHz)

**Problema**: OTA falla
- Verifica que el ESP32 esté en la red
- Asegúrate de que el puerto 8080 esté accesible
- Comprueba que el firmware no sea más grande que 1MB

**Problema**: No puedo hacer flash USB
- Presiona y mantén BOOT mientras conectas USB
- Verifica drivers USB-UART (CP2102 o similar)
- Prueba con otro cable USB

## 📝 Licencia

MIT License

## 🤝 Contribuir

1. Fork el proyecto
2. Crea tu rama de feature (`git checkout -b feature/AmazingFeature`)
3. Commit tus cambios (`git commit -m 'Add some AmazingFeature'`)
4. Push a la rama (`git push origin feature/AmazingFeature`)
5. Abre un Pull Request

## 📧 Contacto

Proyecto Link: [https://github.com/jsebgiraldo/AIRBOT-ESP32C3](https://github.com/jsebgiraldo/AIRBOT-ESP32C3)

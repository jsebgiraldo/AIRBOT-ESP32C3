# 🤖 MCP Automation Scripts

Automatiza el proceso de compilación y flashing usando MCPs.

## Script 1: Auto Build & Flash OTA

Este script MCP combina compilación y flashing OTA en un solo comando.

### Uso
```bash
# Ejecutar desde el root del proyecto
# El script compilará y flasheará automáticamente al ESP32
```

### Ventajas
- ✅ Compila automáticamente
- ✅ Detecta cambios en el código
- ✅ Flashea por OTA sin cables
- ✅ Muestra logs en tiempo real
- ✅ Maneja errores gracefully

## Script 2: Multi-Device Flash

Flashea múltiples ESP32s simultáneamente.

### Configuración
Edita `device_list.json` con las IPs de tus dispositivos:
```json
{
  "devices": [
    {"name": "ESP32-Lab", "ip": "192.168.1.100"},
    {"name": "ESP32-Field", "ip": "192.168.1.101"},
    {"name": "ESP32-Test", "ip": "192.168.1.102"}
  ]
}
```

### Uso
```bash
# Flashear todos los dispositivos
python mcp_scripts/multi_flash.py
```

## Script 3: CI/CD Integration

Integración con GitHub Actions para builds automáticos.

### Configuración
1. El workflow ya está configurado en `.github/workflows/build.yml`
2. Cada push a `main` compila automáticamente
3. Los artifacts están disponibles para download
4. Usa el script para flashear desde CI/CD

### Uso Manual
```bash
# Descargar artifact de GitHub Actions
# Flashear a dispositivo específico
python mcp_scripts/ci_flash.py <run_id> <device_ip>
```

## 📝 Crear tus propios scripts MCP

### Estructura básica
```javascript
// mcp_script.js
const { execSync } = require('child_process');

async function buildAndFlash(deviceIp) {
  // 1. Compilar
  console.log('🔨 Compilando...');
  execSync('idf.py build', { stdio: 'inherit' });
  
  // 2. Flashear
  console.log('📤 Flasheando a', deviceIp);
  execSync(`python ota_upload.py ${deviceIp} build/airbot-esp32c3.bin`, 
           { stdio: 'inherit' });
  
  console.log('✅ ¡Completado!');
}

// Ejecutar
buildAndFlash(process.argv[2]);
```

### Usar con MCP
```bash
# Crear el script MCP
# mcp-add y configurar
# Ejecutar con: node mcp_script.js 192.168.1.100
```

## 🔧 Scripts Disponibles

### `ota_upload.py`
Upload directo del binario compilado.

**Uso:**
```bash
python ota_upload.py <IP> <FIRMWARE_PATH>
```

**Ejemplo:**
```bash
python ota_upload.py 192.168.1.100 build/airbot-esp32c3.bin
```

### `ota_update.py`
OTA desde URL remota.

**Uso:**
```bash
python ota_update.py <IP> <FIRMWARE_URL>
```

**Ejemplo:**
```bash
python ota_update.py 192.168.1.100 http://example.com/firmware.bin
```

## 🚀 Workflow Recomendado

### Desarrollo Local
```bash
# 1. Hacer cambios en el código
vim main/main.c

# 2. Compilar + Flash OTA en un comando
idf.py build && python ota_upload.py 192.168.1.100 build/airbot-esp32c3.bin

# 3. Ver logs
idf.py monitor
```

### Producción
```bash
# 1. Push a GitHub
git push origin main

# 2. GitHub Actions compila automáticamente

# 3. Descargar artifact y flashear
# (o usar API de GitHub para automático)
```

## 🎯 Best Practices

1. **Versionado**: Incrementa `FIRMWARE_VERSION` en cada release
2. **Testing**: Prueba en un dispositivo antes de flashear múltiples
3. **Backup**: Mantén siempre un firmware funcional como fallback
4. **Logs**: Monitorea los logs durante OTA para detectar problemas
5. **Network**: Asegura conexión estable durante OTA

## 🔍 Debugging Scripts

### Ver todas las IPs en la red
```bash
# Linux/Mac
arp -a | grep -i "espressif\|esp"

# Windows
arp -a | findstr "espressif esp"
```

### Ping múltiples dispositivos
```bash
# Verificar que todos estén online
for ip in 192.168.1.{100..102}; do
  ping -c 1 $ip && echo "✅ $ip" || echo "❌ $ip"
done
```

### Ver versión de firmware
```bash
# En todos los dispositivos
for ip in 192.168.1.{100..102}; do
  echo "Device $ip:"
  curl -s http://$ip:8080/version
  echo
done
```

## 📚 Recursos Adicionales

- [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/)
- [OTA Updates Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/api-reference/system/ota.html)
- [GitHub Actions Workflow Syntax](https://docs.github.com/en/actions/using-workflows/workflow-syntax-for-github-actions)

#!/usr/bin/env python3
"""
OTA Upload Script for ESP32-C3
Uploads firmware binary directly to ESP32 via HTTP
"""

import sys
import os
import requests
from pathlib import Path

def upload_firmware(ip_address, firmware_path):
    """
    Upload firmware binary to ESP32 via OTA
    
    Args:
        ip_address: IP address of ESP32
        firmware_path: Path to firmware binary file
    """
    
    # Validate firmware file
    if not os.path.exists(firmware_path):
        print(f"❌ Error: Firmware file not found: {firmware_path}")
        return False
    
    file_size = os.path.getsize(firmware_path)
    print(f"📦 Firmware file: {firmware_path}")
    print(f"📏 File size: {file_size:,} bytes ({file_size/1024/1024:.2f} MB)")
    
    if file_size > 1024 * 1024:  # 1 MB
        print("⚠️  Warning: Firmware is larger than 1MB, may not fit in OTA partition")
    
    # Prepare URL
    url = f"http://{ip_address}:8080/upload"
    print(f"🌐 Target: {url}")
    
    # Read firmware binary
    try:
        with open(firmware_path, 'rb') as f:
            firmware_data = f.read()
    except Exception as e:
        print(f"❌ Error reading firmware file: {e}")
        return False
    
    # Upload firmware
    print("📤 Uploading firmware...")
    try:
        response = requests.post(
            url,
            data=firmware_data,
            headers={'Content-Type': 'application/octet-stream'},
            timeout=60
        )
        
        if response.status_code == 200:
            print("✅ OTA update successful!")
            print("🔄 ESP32 will reboot in a few seconds...")
            return True
        else:
            print(f"❌ Upload failed with status code: {response.status_code}")
            print(f"Response: {response.text}")
            return False
            
    except requests.exceptions.ConnectionError:
        print(f"❌ Connection error: Could not connect to {ip_address}")
        print("   Make sure ESP32 is connected to WiFi and reachable")
        return False
    except requests.exceptions.Timeout:
        print("❌ Upload timeout - firmware may be too large or connection is slow")
        return False
    except Exception as e:
        print(f"❌ Upload error: {e}")
        return False

def main():
    if len(sys.argv) != 3:
        print("ESP32-C3 OTA Firmware Upload")
        print("=" * 50)
        print(f"Usage: python {sys.argv[0]} <ESP32_IP> <FIRMWARE_PATH>")
        print()
        print("Example:")
        print(f"  python {sys.argv[0]} 192.168.1.100 build/airbot-esp32c3.bin")
        print()
        sys.exit(1)
    
    ip_address = sys.argv[1]
    firmware_path = sys.argv[2]
    
    print("ESP32-C3 OTA Firmware Upload")
    print("=" * 50)
    
    # Check ESP32 is reachable
    print(f"🔍 Checking ESP32 at {ip_address}...")
    try:
        version_url = f"http://{ip_address}:8080/version"
        response = requests.get(version_url, timeout=5)
        if response.status_code == 200:
            print(f"✅ ESP32 is reachable")
            print(f"📋 Current version: {response.text}")
        else:
            print(f"⚠️  ESP32 responded with status {response.status_code}")
    except:
        print(f"⚠️  Could not get version info (ESP32 may be running old firmware)")
    
    print()
    
    # Upload firmware
    success = upload_firmware(ip_address, firmware_path)
    
    if success:
        print()
        print("✨ Done! Check the serial monitor for boot messages.")
        sys.exit(0)
    else:
        sys.exit(1)

if __name__ == "__main__":
    main()

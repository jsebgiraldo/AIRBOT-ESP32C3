#!/usr/bin/env python3
"""
OTA Update Script for ESP32-C3
Triggers OTA update from a URL
"""

import sys
import requests
import time

def trigger_ota_update(ip_address, firmware_url):
    """
    Trigger OTA update from URL
    
    Args:
        ip_address: IP address of ESP32
        firmware_url: URL where firmware binary is hosted
    """
    
    url = f"http://{ip_address}:8080/ota"
    print(f"🌐 Target ESP32: {ip_address}")
    print(f"📥 Firmware URL: {firmware_url}")
    print(f"🔗 OTA endpoint: {url}")
    
    # Send OTA request
    print("📤 Sending OTA update request...")
    try:
        response = requests.post(
            url,
            data=firmware_url,
            headers={'Content-Type': 'text/plain'},
            timeout=10
        )
        
        if response.status_code == 200:
            print("✅ OTA update request sent successfully!")
            print("🔄 ESP32 is downloading and installing firmware...")
            print("⏳ This may take 30-60 seconds depending on connection speed")
            print()
            print("📋 Monitor the serial output to see update progress")
            return True
        else:
            print(f"❌ Request failed with status code: {response.status_code}")
            print(f"Response: {response.text}")
            return False
            
    except requests.exceptions.ConnectionError:
        print(f"❌ Connection error: Could not connect to {ip_address}")
        print("   Make sure ESP32 is connected to WiFi and reachable")
        return False
    except requests.exceptions.Timeout:
        print("❌ Request timeout")
        return False
    except Exception as e:
        print(f"❌ Error: {e}")
        return False

def main():
    if len(sys.argv) != 3:
        print("ESP32-C3 OTA Update from URL")
        print("=" * 50)
        print(f"Usage: python {sys.argv[0]} <ESP32_IP> <FIRMWARE_URL>")
        print()
        print("Example:")
        print(f"  python {sys.argv[0]} 192.168.1.100 http://example.com/firmware.bin")
        print()
        print("Note: The firmware binary must be accessible via HTTP/HTTPS")
        print("      You can use GitHub releases, AWS S3, or any web server")
        sys.exit(1)
    
    ip_address = sys.argv[1]
    firmware_url = sys.argv[2]
    
    print("ESP32-C3 OTA Update from URL")
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
        print(f"⚠️  Could not get version info")
    
    print()
    
    # Trigger OTA
    success = trigger_ota_update(ip_address, firmware_url)
    
    if success:
        print()
        print("✨ OTA update initiated!")
        print("💡 Tips:")
        print("   - Use 'idf.py monitor' to see real-time logs")
        print("   - Update takes 30-60 seconds typically")
        print("   - ESP32 will reboot automatically when done")
        sys.exit(0)
    else:
        sys.exit(1)

if __name__ == "__main__":
    main()
